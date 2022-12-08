#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>


int main()
{
    std::vector<std::vector<char>> heights;
    std::vector<std::vector<int>> scores;
    for (std::string line; std::getline(std::cin, line);)
    {
        heights.emplace_back();
        std::transform(line.begin(), line.end(), std::back_inserter(heights.back()), [](const char c)
            {
                return c - '0';
            });
        scores.emplace_back(heights.back().size(), false);
    }

    const int nrows = heights.size();
    const int ncols = heights[0].size();

    int maxscore = 0;
    for (int r0 = 1; r0 < nrows-1; ++r0)
    for (int c0 = 1; c0 < ncols-1; ++c0)
    {
        int score = 1;
        const int height = heights[r0][c0];
        const bool debug = r0 == 3 && c0 == 2;

        int dist = 0;
        for (int r = r0+1; r < nrows; ++r)
        {
            ++dist;
            if (heights[r][c0] >= height)
                break;
        }
        if (debug) std::cout << "down " << dist << "\n";
        score *= dist;

        dist = 0;
        for (int r = r0-1; r >= 0; --r)
        {
            ++dist;
            if (heights[r][c0] >= height)
                break;
        }
        if (debug) std::cout << "up " << dist << "\n";
        score *= dist;

        dist = 0;
        for (int c = c0+1; c < ncols; ++c)
        {
            ++dist;
            if (heights[r0][c] >= height)
                break;
        }

        if (debug) std::cout << "right " << dist << "\n";
        score *= dist;

        dist = 0;
        for (int c = c0-1; c >= 0; --c)
        {
            ++dist;
            if (heights[r0][c] >= height)
                break;
        }
        if (debug) std::cout << "left " << dist << "\n";
        score *= dist;

        scores[r0][c0] = score;
        maxscore = std::max(score, maxscore);
    }

    for (int r = 0; r < nrows; ++r)
    {
        for (int c = 0; c < ncols; ++c)
        {
            std::cout << (int)heights[r][c];
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    for (int r = 0; r < nrows; ++r)
    {
        for (int c = 0; c < ncols; ++c)
        {
            std::cout << (int)scores[r][c] << " ";
        }
        std::cout << "\n";
    }

    std::cout << maxscore << "\n";

    return 0;
}
