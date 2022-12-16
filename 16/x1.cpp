#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

const int MAXTIME = 30;

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    os << "{";
    for (const auto& i: v) os << i << " ";
    os << "}";
    return os;
}


class Valve
{
public:
    Valve()=default;

    Valve(const std::string& name, int rate, const std::vector<std::string>& to_names)
        : name_{name}
        , rate_{rate}
        , to_names_{to_names}
    { }

    void connect(const std::unordered_map<std::string,Valve>& valves)
    {
        for (const std::string& name: to_names_)
            to_valves_.push_back(&valves.at(name));
    }

    void dijkstra()
    {
        const int INF = std::numeric_limits<int>::max();
        auto cmp = [this](const Valve*& v1, const Valve*& v2)
            { return dist_.at(v1) > dist_.at(v2); };
        std::priority_queue<const Valve*, std::vector<const Valve*>, decltype(cmp)> q(cmp);

        dist_[this] = 0;
        q.push(this);

        while (q.size())
        {
            const Valve* curr = q.top();
            const int d = dist_.at(curr);
            q.pop();
            //if (name_ == "AA") std::cout << "POP " << curr->name_ << " " << d << "\n";
            for (const Valve* n: curr->to_valves_)
            {
                const int alt = d + 1;
                const auto& it = dist_.find(n);
                int curn = it == dist_.end() ? INF : it->second;
                //if (name_ == "AA") std::cout << "    CMP " << n->name_ << " " << alt << " " << curn << "\n";
                if (alt < curn)
                {
                    //if (name_ == "AA") std::cout << "    PUSH " << n->name_ << " " << alt << "\n";
                    dist_[n] = alt;
                    q.push(n);
                }
            }
        }
    }

    const std::string& name() const { return name_; }
    int dist(const Valve* v) const { return dist_.at(v); }
    int rate() const { return rate_; }
    const std::vector<const Valve*>& connected() const { return to_valves_; }

    friend std::ostream& operator<<(std::ostream&, const Valve&);

private:
    std::string name_;
    int rate_;
    std::vector<std::string> to_names_;
    std::vector<const Valve*> to_valves_;
    std::unordered_map<const Valve*,int> dist_;
};

std::ostream& operator<<(std::ostream& os, const Valve& v)
{
    os << "Valve(" << v.name_ << ", " << v.rate_ << ", " << v.to_names_ << ", {";
    for (const auto& p: v.dist_)
        os << p.first->name() << ":" << p.second << " ";
    os << "}";
    return os;
}

int expected(int time, const Valve& loc, const Valve& nextloc)
{
    return (MAXTIME - time - loc.dist(&nextloc) - 1)*nextloc.rate();
}

struct StackItem
{
    int pressure;
    int time;
    const Valve* loc;
    std::vector<const Valve*> opened;
};

int path(const Valve* start, const std::unordered_map<std::string,Valve>& valves)
{
    int maxpres = std::numeric_limits<int>::min();
    std::vector<const Valve*> maxpath;

    std::vector<StackItem> stack{{0, 0, start, {}}};

    //int iter = 0;
    while (stack.size())
    {
        const auto si = stack.back();
        stack.pop_back();
        //std::cout << "POP " << si.loc->name() << " " << si.loc << " " << si.time << " " << si.pressure << ' ' << si.opened << '\n';

        bool anyreachable = false;
        for (const auto& p: valves)
        {
            StackItem ni;
            ni.loc = &p.second;

            if (std::find(si.opened.begin(), si.opened.end(), ni.loc) != si.opened.end()) continue;

            ni.time = si.time + si.loc->dist(ni.loc) + 1;
            //std::cout << "    NEXT " << ni.loc->name() << " " << ni.time << '\n';
            if (ni.time < MAXTIME && ni.loc->rate())
            {
                anyreachable = true;
                ni.pressure = si.pressure + expected(si.time, *si.loc, *ni.loc);
                ni.opened = si.opened;
                ni.opened.push_back(ni.loc);
                //std::cout << "    PUSH " << ni.loc->name() << " " << ni.pressure << '\n';
                stack.push_back(ni);
            }
        }

        // terminal state
        if (!anyreachable)
        {
            if (si.pressure > maxpres)
            {
                maxpres = si.pressure;
                maxpath = si.opened;
            }
        }

        //if (iter++ > 2) break;
    }

    std::cout << maxpres << "\n";
    for (const Valve* v: maxpath)
        std::cout << v->name() << " ";
    std::cout << '\n';
    return maxpres;

}

int main()
{
    std::unordered_map<std::string,Valve> valves;

    while (std::cin)
    {
        std::string ignore;
        std::string name;
        std::string rate;
        std::cin >> ignore >> name >> ignore >> ignore >> rate;
        if (!std::cin) break;
        std::cin >> ignore >> ignore >> ignore >> ignore;

        std::string line;
        std::cin >> std::ws;
        std::getline(std::cin, line);
        std::vector<std::string> to_names;
        while (line.size())
        {
            to_names.push_back(line.substr(0, 2));
            if (line.size() < 4) break;
            line = line.substr(4);
        }

        valves.emplace(name, Valve{name, std::stoi(rate.substr(5, rate.size()-2)), to_names});
    }

    for (std::pair<const std::string,Valve>& v: valves)
        v.second.connect(valves);

    for (std::pair<const std::string,Valve>& v: valves)
    {
        v.second.dijkstra();
        std::cout << v.second << "\n";
    }
    std::cout << '\n';

    path(&valves.at("AA"), valves);

    return 0;
}
