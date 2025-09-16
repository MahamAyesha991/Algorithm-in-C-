#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


static int minUnitsNeeded(const vector<int>& layout,
int unitType,
int range)
{
    if (range <= 0) return -1;
    const int n = static_cast<int>(layout.size());

    int firstUncovered = 0;
    int unitsPlaced    = 0;

    while (firstUncovered < n)
        {
        int bestHost = -1;


        int L = max(0, firstUncovered - range + 1);
        int R = min(n - 1, firstUncovered + range - 1);

        for (int i = R; i >= L; --i)
            if (layout[i] == unitType)
                {
                bestHost = i;
                break;
            }

        if (bestHost == -1)  //gap that cannot be covered
            return -1;

        ++unitsPlaced;
        firstUncovered = bestHost + range;   // jump to next uncovered city
    }
    return unitsPlaced;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k, l;
    if (!(cin >> n >> k >> l))
        {
        cerr << "Input error: need n k l.\n";
        return 1;
    }
    if (n <= 0 || k <= 0 || l <= 0)
        {
        cerr << "Input error: n, k, l must be positive.\n";
        return 1;
    }

    vector<int> city(n);
    for (int i = 0; i < n; ++i) {
        if (!(cin >> city[i]) || (city[i] != 0 && city[i] != 1 && city[i] != 2))
            {
            cerr << "Input error: city " << i << " must be 0, 1, or 2.\n";
            return 1;
        }
    }

    const int beamerCount = minUnitsNeeded(city, 1, k);

    vector<int> cataMask(n, 0);
    for (int i = 0; i < n; ++i)
        if (city[i] == 2) cataMask[i] = 2;

    const int cataCount = minUnitsNeeded(cataMask, 2, l);

    // Print output
    cout << "beamer:" << beamerCount << '\n';
    cout << "cata:"   << cataCount   << '\n';
    return 0;
}