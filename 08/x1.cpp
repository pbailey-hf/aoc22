#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>


int main()
{
    std::vector<std::vector<char>> heights;
    std::vector<std::vector<char>> visible;
    for (std::string line; std::getline(std::cin, line);)
    {
        heights.emplace_back();
        std::transform(line.begin(), line.end(), std::back_inserter(heights.back()), [](const char c)
            {
                return c - '0';
            });
        visible.emplace_back(heights.back().size(), false);
    }

    const int nrows = heights.size();
    const int ncols = heights[0].size();

    int max = -1;
    auto check = [&visible, &max, &heights](int r, int c)
    {
        if (heights[r][c] > max)
        {
            visible[r][c] = true;
            max = heights[r][c];
        }
    };

    for (int r = 0; r < nrows; ++r)
    {
        max = -1;
        for (int c = 0; c < ncols; ++c)
            check(r, c);

        max = -1;
        for (int c = ncols-1; c >= 0; --c)
            check(r, c);
    }

    for (int c = 0; c < ncols; ++c)
    {
        max = -1;
        for (int r = 0; r < nrows; ++r)
            check(r, c);

        max = -1;
        for (int r = nrows-1; r >= 0; --r)
            check(r, c);
    }

    int nvisible = 0;
    for (int r = 0; r < nrows; ++r)
    {
        for (int c = 0; c < ncols; ++c)
        {
            if (visible[r][c]) std::cout << "\033[32m";
            std::cout << (int)heights[r][c];
            if (visible[r][c]) std::cout << "\033[m";
            nvisible += visible[r][c];
        }
        std::cout << "\n";
    }
    std::cout << nvisible << "\n";

    return 0;
}
