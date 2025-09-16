#include "munkres_algorithm.hpp"
#include <vector>
#include <limits>
#include <algorithm>

Matrix<int> run_munkres_algorithm(Matrix<int> cost)
{
    //  Check if cost matrix is square
    if (cost.nrows() != cost.ncols())
        {
        throw std::invalid_argument("Cost matrix must be square.");
    }

    const int n = cost.nrows();
    Matrix<int> mask(n, n, 0);

    // Output matrix: 1 = starred zero, 0 otherwise
    std::vector<bool> row_covered(n, false);
    std::vector<bool> col_covered(n, false);

    // Copy cost matrix to local 2D vector for easy manipulation
    std::vector<std::vector<int>> c(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            c[i][j] = cost(i, j);

    // Row reduction and  subtract min value in each row
    for (int i = 0; i < n; ++i) {
        int min_val = *std::min_element(c[i].begin(), c[i].end());
        for (int j = 0; j < n; ++j)
            c[i][j] -= min_val;
    }

    //  Column reduction and subtract min value in each column
    for (int j = 0; j < n; ++j)
        {
        int min_val = std::numeric_limits<int>::max();
        for (int i = 0; i < n; ++i)
            min_val = std::min(min_val, c[i][j]);
        for (int i = 0; i < n; ++i)
            c[i][j] -= min_val;
    }

    //  Star zeros if no other starred zero in row or column
    for (int i = 0; i < n; ++i)
        {
        for (int j = 0; j < n; ++j)
            {
            if (c[i][j] == 0 && !row_covered[i] && !col_covered[j])
                {
                mask(i, j) = 1;
                // star zero
                row_covered[i] = true;
                col_covered[j] = true;
            }
        }
    }
    // Clear covers for next steps
    std::fill(row_covered.begin(), row_covered.end(), false);
    std::fill(col_covered.begin(), col_covered.end(), false);

    // Function to cover columns containing starred zeros
    auto cover_columns_with_stars = [&]()
    {
        for (int j = 0; j < n; ++j) {
            col_covered[j] = false;
            for (int i = 0; i < n; ++i) {
                if (mask(i, j) == 1) {
                    col_covered[j] = true;
                    break;
                }
            }
        }
    };

    cover_columns_with_stars();

    // Check if all columns are covered then done it
    auto all_columns_covered = [&]() {
        return std::count(col_covered.begin(), col_covered.end(), true) == n;
    };

    // Find a zero which is not covered
    auto find_a_zero = [&]() -> std::pair<int, int>
    {
        for (int i = 0; i < n; ++i)
            {
            if (row_covered[i]) continue;
            for (int j = 0; j < n; ++j)
                {
                if (!col_covered[j] && c[i][j] == 0)
                    return {i, j};
            }
        }
        return {-1, -1};
    };

    // Finding the column of a starred zero in row r
    auto find_star_in_row = [&](int r) -> int {
        for (int j = 0; j < n; ++j)
            if (mask(r, j) == 1)
                return j;
        return -1;
    };

    // Finding the row of a starred zero in column c
    auto find_star_in_col = [&](int c_) -> int
    {
        for (int i = 0; i < n; ++i)
            if (mask(i, c_) == 1)
                return i;
        return -1;
    };

    // Finding the row of a primed zero in column c
    auto find_prime_in_row = [&](int r) -> int
    {
        for (int j = 0; j < n; ++j)
            if (mask(r, j) == 2)
                return j;
        return -1;
    };

    // Clear all primes
    auto clear_primes = [&]()
    {
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (mask(i, j) == 2)
                    mask(i, j) = 0;
    };

    // Augment path starting from uncovered zero found
    auto augment_path = [&](std::vector<std::pair<int,int>>& path)
    {
        for (auto& p : path) {
            if (mask(p.first, p.second) == 1)
                mask(p.first, p.second) = 0;
            else if (mask(p.first, p.second) == 2)
                mask(p.first, p.second) = 1;
        }
    };

    // Main loop variables
    while (!all_columns_covered())
        {
        std::pair<int, int> z = find_a_zero();
        while (z.first == -1) {
            // Step 6: Modify matrix if no uncovered zero found
            int min_uncovered = std::numeric_limits<int>::max();
            for (int i = 0; i < n; ++i) {
                if (row_covered[i]) continue;
                for (int j = 0; j < n; ++j) {
                    if (!col_covered[j] && c[i][j] < min_uncovered)
                        min_uncovered = c[i][j];
                }
            }
            for (int i = 0; i < n; ++i)
                {
                for (int j = 0; j < n; ++j)
                    {
                    if (row_covered[i])
                        c[i][j] += min_uncovered;
                    if (!col_covered[j])
                        c[i][j] -= min_uncovered;
                }
            }
            z = find_a_zero();
        }

        //  Prime found zero
        mask(z.first, z.second) = 2;

        int star_col = find_star_in_row(z.first);
        if (star_col != -1) {
            // Cover this row and uncover star_col
            row_covered[z.first] = true;
            col_covered[star_col] = false;
        } else {

            //  Augmenting path
            std::vector<std::pair<int,int>> path;
            path.push_back(z);
            int r = z.first;
            int c_ = z.second;
            while (true) {
                int star_row = find_star_in_col(c_);
                if (star_row == -1)
                    break;
                path.push_back({star_row, c_});
                int prime_col = find_prime_in_row(star_row);
                path.push_back({star_row, prime_col});
                r = star_row;
                c_ = prime_col;
            }

            augment_path(path);
            clear_primes();

            std::fill(row_covered.begin(), row_covered.end(), false);
            std::fill(col_covered.begin(), col_covered.end(), false);

            cover_columns_with_stars();
        }
    }

    return mask;
}