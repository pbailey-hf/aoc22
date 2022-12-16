#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

const int MAXTIME = 26;

template <typename T>
std::enable_if<std::is_pointer<T>::value,std::ostream&>::type
operator<<(std::ostream& os, const std::vector<T>& v)
{
    os << "{";
    for (const auto& i: v) os << *i << " ";
    os << "}";
    return os;
}

template <typename T>
std::enable_if<!std::is_pointer<T>::value,std::ostream&>::type
operator<<(std::ostream& os, const std::vector<T>& v)
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
            for (const Valve* n: curr->to_valves_)
            {
                const int alt = d + 1;
                const auto& it = dist_.find(n);
                int curn = it == dist_.end() ? INF : it->second;
                if (alt < curn)
                {
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
    os << v.name_;
    return os;
}

int expected(int time, const Valve& loc, const Valve& nextloc)
{
    return (MAXTIME - time - loc.dist(&nextloc) - 1)*nextloc.rate();
}

struct Agent
{
    int time;
    const Valve* loc;
};

std::ostream& operator<<(std::ostream& os, const Agent& a)
{
    os << a.loc->name()<<","<<a.time;
    return os;
}

struct StackItem
{
    int pressure;
    std::vector<Agent> agents;
    std::vector<const Valve*> opened;
};

int path(const Valve* start, const std::unordered_map<std::string,Valve>& valves)
{
    int maxpres = std::numeric_limits<int>::min();
    std::vector<const Valve*> maxpath;

    std::vector<StackItem> stack{{0, {}, {}}};
    const int nagents = 2;
    for (int i = 0; i<nagents; ++i)
        stack.back().agents.emplace_back(0, start);

    std::vector<const Valve*> vlist;
    for (const auto& v: valves)
        vlist.push_back(&v.second);
    const int nwithrate = std::count_if(vlist.begin(), vlist.end(), [](const Valve* v) {return v->rate();});

    //int iter = 0;
    while (stack.size())
    {
        const auto si = stack.back();
        stack.pop_back();
        //std::cout << "POP " << si.agents << " " << si.pressure << '\n';

        if (si.pressure > maxpres)
        {
            maxpres = si.pressure;
            maxpath = si.opened;
        }

        std::vector<std::vector<const Valve*>> nexts;
        std::vector<size_t> iv(nagents);
        while (true)
        {
            std::vector<const Valve*> next(nagents);
            std::transform(iv.begin(), iv.end(), next.begin(), [&vlist](int i) {return vlist[i];});
            const int nvisited = std::count_if(si.opened.begin(), si.opened.end(), [](const Valve* v) {return v->rate();});
            const int nrem = nwithrate - nvisited;
            if (nrem == 1)
            {
                const Valve* lastv = nullptr;
                for (const Valve* v: vlist)
                {
                    if (std::count(si.opened.begin(), si.opened.end(), v) == 0 and v->rate())
                    {
                        lastv = v;
                        break;
                    }
                }

                //std::cout << "SPECIAL " << lastv->name() << " " << si.opened << "\n";
                auto ni = si;
                ni.pressure += std::max(
                        expected(si.agents[0].time, *si.agents[0].loc, *lastv),
                        expected(si.agents[1].time, *si.agents[1].loc, *lastv));
                ni.opened.push_back(lastv);
                stack.push_back(ni);
                break;
            }

            bool good = true;
            for (int ia = 0; ia < nagents; ++ia)
            {
                //std::cout << ia << " " << next[ia]->name() << " "
                //        << (next[ia]->rate() == 0)
                //        << (std::count(si.opened.begin(), si.opened.end(), next[ia]) != 0)
                //        << (std::count(next.begin(), next.end(), next[ia]) > 1)
                //        << (si.agents[ia].time + si.agents[ia].loc->dist(next[ia]) + 1 >= MAXTIME)
                //        << "\n";

                if (next[ia]->rate() == 0
                        || std::count(si.opened.begin(), si.opened.end(), next[ia]) != 0
                        || std::count(next.begin(), next.end(), next[ia]) > 1
                        || si.agents[ia].time + si.agents[ia].loc->dist(next[ia]) + 1 >= MAXTIME)
                {
                    good = false;
                }
            }


            if (good)
                nexts.emplace_back(std::move(next));

            // increase by one
            bool incremented = false;
            for (int ia = 0; ia < nagents; ++ia)
            {
                if (iv[ia] < vlist.size()-1)
                {
                    ++iv[ia];
                    incremented = true;
                    break;
                }
                else
                {
                    iv[ia] = 0;
                }
            }

            if (!incremented)
                break;
        }

        for (const auto& next: nexts)
        {
            StackItem ni = si;

            for (int ia=0; ia<nagents; ++ia)
            {
                Agent& a = ni.agents[ia];
                if (!next[ia])
                {
                    a.time += MAXTIME;
                    continue;
                }
                ni.pressure += expected(a.time, *a.loc, *next[ia]);
                a.time += a.loc->dist(next[ia]) + 1;
                a.loc = next[ia];
                ni.opened.push_back(a.loc);
            }
            //std::cout << "    NEXT " << ni.agents << " " << ni.pressure << '\n';
            stack.push_back(ni);
        }
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
