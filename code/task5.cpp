#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <cmath>
#include "matrix.hpp"

using Graph = Matrix<int>;

static Graph read_graph()
{
    std::vector<int> buf;
    for (int v; std::cin >> v;) buf.push_back(v);
    if (buf.empty()) throw std::runtime_error("no input");

    if (buf.size() >= 2 &&
        buf[0] > 0 &&
        std::size_t(buf[0]) * std::size_t(buf[0]) == buf.size() - 1)
        buf.erase(buf.begin());                           // drop header

    while (!buf.empty()) {                                // trim to square
        std::size_t s = buf.size();
        std::size_t r = std::llround(std::sqrt(s));
        if (r * r == s) break;
        buf.pop_back();
    }
    if (buf.empty()) throw std::runtime_error("no square matrix");

    std::size_t n = std::sqrt(buf.size());
    Graph G(n, n);
    for (std::size_t i = 0, k = 0; i < n; ++i)
        for (std::size_t j = 0; j < n; ++j, ++k)
            G(i, j) = buf[k];
    return G;
}

/* Return N(v) as a set                                              */
static std::set<int> neigh(const Graph& G, int v)
{
    std::set<int> out;
    for (int j = 0; j < static_cast<int>(G.ncols()); ++j)
        if (G(v, j)) out.insert(j);
    return out;
}

/* Bron–Kerbosch with pivot, stores maximal cliques in ‘out’.        */
static void bk(const Graph&               G,
               std::set<int>              R,
               std::set<int>              P,
               std::set<int>              X,
               std::vector<std::vector<int>>& out)
{
    if (P.empty() && X.empty()) { out.emplace_back(R.begin(), R.end()); return; }

    /* choose pivot u with largest |P ∩ N(u)|                         */
    int u_best = -1; std::size_t bestDeg = 0;
    std::set<int> PuX; std::set_union(P.begin(), P.end(), X.begin(), X.end(),
                                      std::inserter(PuX, PuX.begin()));
    for (int u : PuX) {
        std::size_t deg = 0; for (int v : P) if (G(u, v)) ++deg;
        if (deg > bestDeg) { bestDeg = deg; u_best = u; }
    }
    std::set<int> Nu = (u_best >= 0) ? neigh(G, u_best) : std::set<int>{};

    /* iterate P \ N(u)                                               */
    std::vector<int> candidates;
    std::set_difference(P.begin(), P.end(), Nu.begin(), Nu.end(),
                        std::back_inserter(candidates));

    for (int v : candidates) {
        std::set<int> Nv = neigh(G, v);

        std::set<int> R2 = R; R2.insert(v);
        std::set<int> P2, X2;
        std::set_intersection(P.begin(), P.end(), Nv.begin(), Nv.end(),
                              std::inserter(P2, P2.begin()));
        std::set_intersection(X.begin(), X.end(), Nv.begin(), Nv.end(),
                              std::inserter(X2, X2.begin()));

        bk(G, R2, P2, X2, out);          // recurse

        P.erase(v); X.insert(v);         // move v from P to X
    }
}

int main()
{
    try {
        Graph G = read_graph();
        int n   = static_cast<int>(G.nrows());

        std::set<int> P; for (int i = 0; i < n; ++i) P.insert(i);

        std::vector<std::vector<int>> cliques;
        bk(G, {}, P, {}, cliques);

        std::sort(cliques.begin(), cliques.end());   // deterministic order

        for (const auto& C : cliques) {
            std::cout << '{';
            for (std::size_t i = 0; i < C.size(); ++i) {
                if (i) std::cout << ", ";
                std::cout << C[i];
            }
            std::cout << "}\n";
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}