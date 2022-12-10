#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

class Instruction
{
public:
    enum class Operation { noop, addx };
    Instruction(Operation t, int x)
        : op_{t}
        , arg_{x}
    {
        if (t == Operation::noop)
            throw std::runtime_error{"noop with argument"};
    }

    Instruction(Operation t)
        : op_{t}
    {
        if (t == Operation::addx)
            throw std::runtime_error{"addx without argument"};
    }

    static Instruction from_string(const std::string& s)
    {
        std::stringstream ss(s);
        std::string op;
        ss >> op;
        if (op == "noop")
        {
            return {Operation::noop};
        }
        else if (op == "addx")
        {
            int arg;
            ss >> arg;
            return {Operation::addx, arg};
        }

        throw std::runtime_error{"unknown op " + op};
    }

    Operation op() const { return op_; }

    int arg() const
    {
        if (op_ == Operation::noop)
            throw std::runtime_error{"noop argument requested"};
        return arg_;
    }

    int duration() const
    {
        switch (op_)
        {
            case Operation::noop:
                return 1;
                break;
            case Operation::addx:
                return 2;
                break;
            default:
                throw std::runtime_error{"missing duration"};
        }
    }

private:
    Operation op_;
    int arg_;
};

int main()
{
    std::vector<Instruction> instructions;
    for (std::string line; std::getline(std::cin, line); )
        instructions.push_back(Instruction::from_string(line));
    //std::cout << instructions.size()<<"\n";

    int64_t sum = 0;
    int reg = 1;
    size_t pc = 0;
    int finish_cycle = instructions[pc].duration();
    for (int cycle = 1; cycle <= 240; ++cycle)
    {
        const auto& ins = instructions[pc];

        // check if we're at one of the interesting cycles
        if ((cycle + 20)%40 == 0)
        {
            const int64_t str = cycle*reg;
            sum += str;
            //std::cout << "NEWSUM " << cycle << " " << reg << " " << str << " " << sum << "\n";
        }

        const int xpos = (cycle - 1)%40;
        if (xpos == 0) std::cout << '\n';
        std::cout << (std::abs(reg - xpos) <= 1 ? '#' : ' ');

        // complete the executing instruction
        if (cycle == finish_cycle)
        {
            switch (ins.op())
            {
                case Instruction::Operation::noop:
                    //std::cout << "NOOP " << cycle << "\n";
                    break;
                case Instruction::Operation::addx:
                    //std::cout << "UPDATE " << cycle << " " << reg << "->" << reg+ins.arg() << "\n";
                    reg += ins.arg();
                    break;
                default:
                    throw std::runtime_error{"missing operation definition"};
            }

            // advance the instruction
            ++pc;
            if (pc >= instructions.size()) break;
            finish_cycle = cycle + instructions[pc].duration();
        }
    }

    std::cout << "\n";

    return 0;
}
