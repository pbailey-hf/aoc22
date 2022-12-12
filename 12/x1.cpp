#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

struct Vec2
{
    int x = 0;
    int y = 0;
};

bool operator==(const Vec2& a, const Vec2& b)
{
    return a.x == b.x && a.y == b.y;
}

Vec2 operator-(const Vec2& a, const Vec2& b)
{
    return {a.x - b.x, a.y - b.y};
}

Vec2 operator+(const Vec2& a, const Vec2& b)
{
    return {a.x + b.x, a.y + b.y};
}

std::ostream& operator<<(std::ostream& os, const Vec2& v)
{
    os << "{" << v.x << "," << v.y << "}";
    return os;
}

template<>
struct std::hash<Vec2>
{
    std::size_t operator()(const Vec2& v) const
    {
        std::hash<int> h{};
        return h(v.x) ^ (h(v.y) << 4);
    }
};

class Map
{
public:
    static const int inf = 1e6;

    Map(size_t nx, size_t ny)
        : start{-1, -1}
        , goal{-1, -1}
        , storage_{ny, std::vector<char>(nx, '.')}
    { }

    Map(std::istream& is)
    {
        for (std::string line; std::getline(is, line); ) {
            storage_.emplace_back();
            for (char c: line)
            {
                if (c == 'S') {
                    start = Vec2(storage_.back().size(), storage_.size()-1);
                    c = 'a';
                } else if (c == 'E') {
                    goal = Vec2(storage_.back().size(), storage_.size()-1);
                    c = 'z';
                }

                storage_.back().push_back(c);
            }
        }
    }

    char get(const Vec2& p) const
    {
        return storage_[p.y][p.x];
    }

    char& get(const Vec2& p)
    {
        return storage_[p.y][p.x];
    }

    int dist(const Vec2& a, const Vec2& b) const
    {
        const Vec2 diff = b - a;
        if (std::abs(diff.x) + std::abs(diff.y) > 1)
            return inf;
        if (get(b) - get(a) > 1)
            return inf;
        return 1;
    }

    std::vector<Vec2> neighbors(const Vec2& p) const
    {
        std::vector<Vec2> ns;
        static const std::vector<Vec2> offsets{
            {0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        for (const auto& o: offsets) {
            const auto n = p + o;
            if (0 <= (size_t)n.x && (size_t)n.x < width()
             && 0 <= (size_t)n.y && (size_t)n.y < height())
                ns.push_back(n);
        }
        return ns;
    }

    size_t height() const { return storage_.size(); }
    size_t width() const { return storage_[0].size(); }

    Vec2 start;
    Vec2 goal;

private:
    std::vector<std::vector<char>> storage_;
};

std::ostream& operator<<(std::ostream& os, const Map& map)
{
    for (size_t r = 0; r < map.height(); ++r) {
        for (size_t c = 0; c < map.width(); ++c) {
            const Vec2 pos(c, r);
            if (pos == map.start)
                os << "\x1b[1;32m";
            if (pos == map.goal)
                os << "\x1b[1;31m";
            os << map.get(pos) << "\x1b[0m";
        }
        os << '\n';
    }

    return os;
}

int main()
{
    Map map(std::cin);
    std::cout << map << '\n';

    // Dijkstra
    std::unordered_map<Vec2, int> dist;
    dist[map.start] = 0;
    std::unordered_map<Vec2, Vec2> prev;

    auto cmp = [&dist](const Vec2& a, const Vec2& b) {
        int ia = dist.contains(a) ? dist[a] : Map::inf;
        int ib = dist.contains(b) ? dist[b] : Map::inf;
        return ia > ib;
    };
    std::priority_queue<Vec2, std::vector<Vec2>, decltype(cmp)> q(cmp);
    q.push(map.start);

    while (!q.empty()) {
        const Vec2 cur = q.top();
        //std::cout << "curr " << cur << "\n";
        if (cur == map.goal) {
            //std::cout << "done\n";
            break;
        }

        q.pop();
        for (const Vec2& neighbor: map.neighbors(cur)) {
            const int alt = dist.at(cur) + map.dist(cur, neighbor);
            if (alt < (dist.contains(neighbor) ? dist[neighbor] : Map::inf)) {
                dist[neighbor] = alt;
                q.push(neighbor);
                prev[neighbor] = cur;
            }
        }
    }

    Map path(map.width(), map.height());
    path.start = map.start;
    path.goal = map.goal;
    Vec2 cur = map.goal;
    while (cur != map.start) {
        const Vec2 pr = prev[cur];
        const Vec2 diff = cur - pr;
        char dir;
        if (diff.x == 1)
            dir = '>';
        else if (diff.x == -1)
            dir = '<';
        else if (diff.y == 1)
            dir = 'v';
        else if (diff.y == -1)
            dir = '^';
        else
            throw std::runtime_error{"bad direction"};

        path.get(pr) = dir;
        cur = pr;
    }

    std::cout << path << '\n';
    std::cout << dist[map.goal] << "\n";
}
