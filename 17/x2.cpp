#include <algorithm>
#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

const int64_t WIDTH = 7;

struct Point
{
    int64_t y;
    int64_t x;

    void operator+=(const Point& b)
    {
        y += b.y;
        x += b.x;
    }

    operator bool() const
    {
        return y || x;
    }
};

Point operator+(const Point& a, const Point& b)
{
    return {a.y+b.y, a.x+b.x};
}

using RockPoints = std::array<Point,5>;

class Rocks
{
public:
    Rocks()
        : rocks_({{}})
        , ytop_(0)
        , itop_(0)
    {
        rocks_[0] = {1,1,1,1,1,1,1};
    }

    void add(const RockPoints& pts)
    {
        for (const auto& p: pts)
        {
            if (p.y > ytop_)
            {
                const int64_t diff = p.y - ytop_;
                itop_ = (itop_ + diff)%rocks_.size();
                ytop_ += diff;
                for (int64_t y = ytop_-diff+1; y <= ytop_; ++y)
                    rocks_[yget(y)] = {};
            }
            rocks_[yget(p.y)][p.x] = 1;
        }
    }

    bool collision(const Point& p) const
    {
        return p.y <= ytop_ && rocks_[yget(p.y)][p.x];
    }

    bool collision(const RockPoints& pts) const
    {
        for (const auto& p: pts)
            if (collision(p))
                return true;
        return false;
    }

private:
    std::array<std::array<bool,WIDTH>,1024> rocks_;
    int64_t ytop_;
    size_t itop_;

    size_t yget(int64_t y) const
    {
        const size_t dy = ytop_ - y;
        if (itop_ >= dy)
            return itop_ - dy;

        if (dy >= rocks_.size())
            throw std::runtime_error("rock stack overflow");
        return (rocks_.size() + itop_) - dy;
    }
};

int shift(char c)
{
    static std::array<int,3> s{{-1, 0, 1}};
    return s[c-'<'];
}

class Rock
{
public:
    static constexpr char shapes[] = "-+j|o";
    static constexpr int nshapes = 5;

    Rock()=default;
    explicit Rock(int64_t maxheight, const std::string& dirs, int& idir)
        : down_{true}
        , ymin_{maxheight + 4}
        , xmin_{2}
        , maxheight_{maxheight}
    {
        const char shape = shapes[irock_%nshapes];
        ++irock_;
        int width;
        if (shape == '-')
            width = 4;
        else if (shape == '+')
            width = 3;
        else if (shape == 'j')
            width = 3;
        else if (shape == '|')
            width = 1;
        else if (shape == 'o')
            width = 2;

        // lower the rock just above the previous settled rocks
        const int64_t maxx = WIDTH-width;
        const int ndir = dirs.length();
        while (ymin_ > maxheight_)
        {
            xmin_ = std::max(0l, std::min(maxx, xmin_+shift(dirs[idir])));
            idir = (idir+1)%ndir;
            --ymin_;
        }
        ++ymin_;

        if (shape == '-') {
            points_[0] = {ymin_+0, xmin_+0};
            points_[1] = {ymin_+0, xmin_+1};
            points_[2] = {ymin_+0, xmin_+2};
            points_[3] = {ymin_+0, xmin_+3};
            points_[4] = {ymin_+0, xmin_+3};
            ymax_ = ymin_+0;
            xmax_ = xmin_+3;
        } else if (shape == '+') {
            points_[0] = {ymin_+0, xmin_+1};
            points_[1] = {ymin_+1, xmin_+0};
            points_[2] = {ymin_+1, xmin_+1};
            points_[3] = {ymin_+1, xmin_+2};
            points_[4] = {ymin_+2, xmin_+1};
            ymax_ = ymin_+2;
            xmax_ = xmin_+2;
        } else if (shape == 'j') {
            points_[0] = {ymin_+0, xmin_+0};
            points_[1] = {ymin_+0, xmin_+1};
            points_[2] = {ymin_+0, xmin_+2};
            points_[3] = {ymin_+2, xmin_+2};
            points_[4] = {ymin_+1, xmin_+2};
            ymax_ = ymin_+2;
            xmax_ = xmin_+2;
        } else if (shape == '|') {
            points_[0] = {ymin_+0, xmin_+0};
            points_[1] = {ymin_+1, xmin_+0};
            points_[2] = {ymin_+2, xmin_+0};
            points_[3] = {ymin_+3, xmin_+0};
            points_[4] = {ymin_+3, xmin_+0};
            ymax_ = ymin_+3;
            xmax_ = xmin_+0;
        } else if (shape == 'o') {
            points_[0] = {ymin_+0, xmin_+0};
            points_[1] = {ymin_+0, xmin_+1};
            points_[2] = {ymin_+1, xmin_+0};
            points_[3] = {ymin_+1, xmin_+1};
            points_[4] = {ymin_+1, xmin_+1};
            ymax_ = ymin_+1;
            xmax_ = xmin_+1;
        } else {
            throw std::runtime_error("unknown shape");
        }
    }

    static int64_t irock() {return irock_;}
    bool down() const {return down_;}
    int64_t ymax() const {return ymax_;}
    int64_t ymin() const {return ymin_;}
    const RockPoints& points() const {return points_;}

    bool move_lr(const char direction, const Rocks& rocks)
    {
        down_ = true;

        // check if we'll hit the boundary
        if (direction == '<' && xmin_ == 0) {
            return false;
        } else if (direction == '>' && xmax_ == WIDTH-1) {
            return false;
        }

        const int offset = shift(direction);
        for (auto p: points_)
        {
            p.x += offset;
            if (rocks.collision(p))
                return false;
        }

        for (auto& p: points_)
            p.x += offset;
        xmin_ += offset;
        xmax_ += offset;

        return true;
    }

    bool move_down(const Rocks& rocks)
    {
        down_ = false;

        for (auto p: points_)
        {
            --p.y;
            if (rocks.collision(p))
                return false;
        }

        for (auto& p: points_)
            --p.y;

        --ymin_;
        --ymax_;

        return true;
    }

private:
    bool down_;
    int64_t ymin_;
    int64_t ymax_;
    int64_t xmin_;
    int64_t xmax_;
    int64_t maxheight_;
    RockPoints points_;

    static int64_t irock_;
};

int64_t Rock::irock_ = 0;

void run(const int64_t nrocks, const std::string& dirs, int& idir, Rocks& rocks, int64_t& maxheight)
{
    //std::cout << "running " << nrocks << " rocks; " << idir << " " << Rock::irock()%Rock::nshapes << " " << Rock::irock() << "\n";
    const int64_t istart = Rock::irock();
    Rock rock(maxheight, dirs, idir);
    const int ndirs = dirs.length();

    for (;;) {
        if (!rock.down()) {
            rock.move_lr(dirs[idir], rocks);
            idir = (idir+1)%ndirs;
            //std::cout << "   " << dirs[(idir-1)%dirs.length()] << " " << success << " " << rock.ymax() << "\n";
        } else {
            const bool success = rock.move_down(rocks);
            //std::cout << "   d " << success << " " << rock.ymax() << "\n";
            if (!success) {
                maxheight = std::max(maxheight, rock.ymax());
                rocks.add(rock.points());
                if (Rock::irock() - istart == nrocks) break;
                // constructing in place is a bit faster than creating a temporary and copying
                //rock = Rock(maxheight);
                new(&rock)Rock(maxheight, dirs, idir);
                //std::cout << "==> ROCK " << Rock::irock() << "\n";
            }
        }
    }
}

int main()
{
    std::string dirs;
    std::getline(std::cin, dirs);
    std::cout << dirs << "\n";
    int idir = 0;
    const int ndirs = dirs.length();

    int64_t maxheight = 0;
    Rocks rocks;
    const auto time_start = std::chrono::steady_clock::now();
#ifdef TEST
    const int64_t nrocks = 2022;
#else
    const int64_t nrocks = 1000000000000;
#endif
    std::vector<std::array<bool,Rock::nshapes>> seen(ndirs, {{}});

    // reach steady state
    run(200, dirs, idir, rocks, maxheight);

    // find a repeat
    while (!seen[idir][Rock::irock()%Rock::nshapes]) {
        seen[idir][Rock::irock()%Rock::nshapes] = true;
        run(1, dirs, idir, rocks, maxheight);
    }
    const auto marker_idir = idir;
    const auto marker_shape = Rock::irock()%Rock::nshapes;
    const auto marker_irock = Rock::irock();
    std::cout << "found a repeat " << marker_idir << " " << marker_shape << " @ rock " << Rock::irock() <<  "\n";

    // find the height gain per cycle
    const auto prevheight = maxheight;
    do {
        run(1, dirs, idir, rocks, maxheight);
    } while (Rock::irock() <= nrocks && (idir != marker_idir || Rock::irock()%Rock::nshapes != marker_shape));
    const auto rocks_per_cycle = Rock::irock() - marker_irock;
    std::cout << "rocks per cycle " << rocks_per_cycle << "\n";
    const auto height_per_cycle = maxheight - prevheight;
    std::cout << "height per cycle " << height_per_cycle << "\n";

    // skipped cycles
    const auto skipped_cycles = (nrocks-Rock::irock())/rocks_per_cycle;
    std::cout << "skipped cycles " << skipped_cycles << "\n";

    // run the remainder
    run((nrocks-Rock::irock())%rocks_per_cycle, dirs, idir, rocks, maxheight);

    // now add the big chunk in the middle that we skipped
    maxheight += skipped_cycles*height_per_cycle;

    std::cout << maxheight << "\n";
}
