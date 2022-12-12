#include <algorithm>
#include <deque>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void check(const std::string& a, const std::string& b)
{
    if (a != b)
        throw std::runtime_error{"invalid input.  expected " + a + " got " + b};
}

class Monkey
{
public:
    Monkey(std::istream& is)
    {
        std::string word;
        // Monkey 0:
        is >> word;
        check("Monkey", word);
        is >> id_;
        is >> word;

        // Starting items: ...
        is >> word;
        check("Starting", word);
        is >> word;
        check("items:", word);
        std::string line;
        std::getline(is, line);
        std::stringstream ss(line);
        while (ss)
        {
            int item;
            ss >> item;
            // burn a comma
            ss >> word;
            items_.push_back(item);
        }

        // Operation: new =
        is >> word;
        check("Operation:", word);
        is >> word;
        check("new", word);
        is >> word;
        check("=", word);
        // assume a binary operation
        is >> arg1_;
        is >> op_;
        is >> arg2_;

        // Test: divisible by ...
        is >> word;
        check("Test:", word);
        is >> word;
        check("divisible", word);
        is >> word;
        check("by", word);
        is >> divisor_;

        // If true: throw to monkey ...
        is >> word;
        is >> word;
        is >> word;
        is >> word;
        is >> word;
        is >> to_true_;

        // If false: throw to monkey ...
        is >> word;
        is >> word;
        is >> word;
        is >> word;
        is >> word;
        is >> to_false_;
    }

    int ninspect() const { return ninspect_; }
    int id() const { return id_; }
    const std::deque<int>& items() const { return items_; }

    std::pair<int, int> inspect_and_throw()
    {
        ++ninspect_;
        int item = items_.front();
        items_.pop_front();
        const auto get = [item](const std::string& s)
        {
            if (s == "old")
                return item;
            return std::stoi(s);
        };

        if (op_ == "+")
            item = get(arg1_) + get(arg2_);
        else if (op_ == "*")
            item = get(arg1_) * get(arg2_);
        else
            std::runtime_error{"unknown op " + op_};

        item /= 3;

        return {item, item % divisor_ == 0 ? to_true_ : to_false_};
    }

    void recv(int item)
    {
        items_.push_back(item);
    }

private:
    int id_;
    std::deque<int> items_;
    std::string arg1_;
    std::string op_;
    std::string arg2_;
    int divisor_;
    int to_true_;
    int to_false_;
    int ninspect_;
};

void print(std::ostream& os, const std::vector<Monkey>& monkeys)
{
    for (const auto& m: monkeys) {
        os << "Monkey " << m.id() << ":";
        for (const auto i: m.items())
            os << " " << i;
        os << "\n";
    }
}

int main()
{
    std::vector<Monkey> monkeys;
    int i = 0;
    while (std::cin) {
        try {
            monkeys.emplace_back(std::cin);
            std::cout << "read monkey " << i++ << "\n";
        } catch (const std::runtime_error& e) {
            if (!std::cin.eof()) {
                throw;
            }
        }
        if (monkeys.back().id() != monkeys.size() - 1)
            throw std::runtime_error{"invalid monkey id"};
    }

    std::cout << "==> start <==\n";
    print(std::cout, monkeys);
    std::cout << "\n";

    for (int i = 0; i < 20; ++i) {
        for (auto& m: monkeys) {
            while (m.items().size()) {
                const auto next = m.inspect_and_throw();
                monkeys[next.second].recv(next.first);
                //std::cout << "monkey " << m.id() << " throws " << next.first << " to monkey " << next.second << "\n";
            }
        }

        std::cout << "==> end of round " << i + 1 << " <==\n";
        print(std::cout, monkeys);
        std::cout << "\n";
    }

    std::vector<int> nins(monkeys.size());
    std::transform(monkeys.begin(), monkeys.end(), nins.begin(), [](const Monkey& m) {
            std::cout << "monkey " << m.id() << " inspected items " << m.ninspect() << " times.\n";
        return m.ninspect();
    });
    std::sort(nins.begin(), nins.end());
    std::cout << *(nins.end()-1) * *(nins.end()-2) << "\n";


    return 0;
}
