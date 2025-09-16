#ifndef BLOOM_FILTER_HPP
#define BLOOM_FILTER_HPP

#include <bitset>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include "murmurhash.hpp"

// -----------------------------------------------------------------------------
// Small wrapper that adapts murmur3_32(keyBytes,len,seed) to the required
//   std::size_t operator()(Key,unsigned) interface.
// -----------------------------------------------------------------------------
template <typename Key>
struct BloomHash {
    std::size_t operator()(const Key& value, unsigned int seed) const {
        return murmur3_32(reinterpret_cast<const uint8_t*>(&value),
                          sizeof(Key),
                          seed);
    }
};

// -----------------------------------------------------------------------------
//  BloomFilter<Key , m , Hash>
//    • m     = number of bits (compile-time constant)
//    • k_    = number of hash functions (run-time constructor arg)
// -----------------------------------------------------------------------------
template <typename Key, unsigned int m, typename Hash = BloomHash<Key>>
class BloomFilter {
public:
    /* 1) empty filter ------------------------------------------------------- */
    explicit BloomFilter(unsigned int num_hashes) : k_{num_hashes ? num_hashes : 1} {}

    /* 2) initializer-list constructor -------------------------------------- */
    BloomFilter(std::initializer_list<Key> items, unsigned int num_hashes)
        : BloomFilter(num_hashes)
    {
        for (const auto& item : items) insert(item);
    }

    /* 3) iterator-range constructor ---------------------------------------- */
    template <typename Iterator>
    BloomFilter(Iterator begin, Iterator end, unsigned int num_hashes)
        : BloomFilter(num_hashes)
    {
        for (auto it = begin; it != end; ++it) insert(*it);
    }

    /* 4) insert ------------------------------------------------------------- */
    bool insert(const Key& key)
    {
        bool changed = false;
        for (unsigned int i = 0; i < k_; ++i) {
            std::size_t pos = hash_(key, i) % m;
            if (!bits_.test(pos)) {
                bits_.set(pos);
                changed = true;
            }
        }
        return changed;
    }

    /* 5) contains ----------------------------------------------------------- */
    [[nodiscard]]
    bool contains(const Key& key) const
    {
        for (unsigned int i = 0; i < k_; ++i)
            if (!bits_.test(hash_(key, i) % m))
                return false;
        return true;
    }

    /* 6) false-positive rate ----------------------------------------------- */
    template <typename Iterator>
    double false_positive_rate(Iterator pos_begin, Iterator pos_end,
                               Iterator neg_begin, Iterator neg_end) const
    {
        std::uint64_t negatives = std::distance(neg_begin, neg_end);
        if (negatives == 0) return 0.0;

        std::uint64_t false_pos = 0;
        for (auto it = neg_begin; it != neg_end; ++it)
            if (contains(*it)) ++false_pos;

        return static_cast<double>(false_pos) /
               static_cast<double>(negatives);
    }

    /* 7) space_ratio (fixed) ------------------------------------------------ */
    [[nodiscard]]
    double space_ratio(std::uint64_t num_elements) const
    {
        if (num_elements == 0) return 0.0;

        // bytes occupied by the Bloom filter bitset
        const double bytes_filter = static_cast<double>(m) / 8.0;

        // bytes required to store all elements explicitly
        const double bytes_raw = static_cast<double>(num_elements) *
                                 sizeof(Key);

        return bytes_filter / bytes_raw;   // dimensionless ratio
    }

    /* 8) approximate element count ----------------------------------------- */
    [[nodiscard]]
    std::uint64_t approx_size() const
    {
        std::size_t X = bits_.count();              // bits set to 1
        if (X == 0) return 0;

        double p = 1.0 - static_cast<double>(X) / static_cast<double>(m);
        double est = -static_cast<double>(m) / static_cast<double>(k_) *
                     std::log(p);

        return static_cast<std::uint64_t>(std::round(est));
    }

private:
    std::bitset<m> bits_;
    unsigned int   k_;
    Hash           hash_;
};

#endif // BLOOM_FILTER_HPP