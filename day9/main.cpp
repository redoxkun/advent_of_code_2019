#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

constexpr auto show_asm = false;

using int64 = long long;

template<typename Container>
auto print_container(const Container& container) -> void
{
	std::cout << "[ ";

	for (const auto& item : container)
		std::cout << item << " ";

	std::cout << "]" << std::endl;
}

struct computer_state
{
	bool halt;
	int pc;

	std::vector<int64> memory;
	
	std::vector<int64> in_data;
	int in_data_index;
	
	std::vector<int64> out_data;

	int relative_base;

	explicit computer_state() : halt{false}, pc{0}, in_data_index{0}, relative_base{0}
	{}
};

auto print_computer_state(const computer_state& computer, const bool print_memory) -> void
{
	std::cout << "--- Computer state ---" << std::endl;
	std::cout << " halt: " << computer.halt << std::endl;
	std::cout << " pc:   " << computer.pc << std::endl;
	std::cout << " in:   ";
	print_container(computer.in_data);
	std::cout << " in_i: " << computer.in_data_index << std::endl;
	std::cout << " out:  ";
	print_container(computer.out_data);
	std::cout << " relative_base: " << computer.relative_base << std::endl;
	if (print_memory)
	{
		std::cout << "memory: ";
		print_container(computer.memory);
	}
	std::cout << "----------------------" << std::endl;
}

auto load_program(const std::string& data) -> std::vector<int64>
{
	std::vector<int64> program;

	std::stringstream ss_data(data);
	std::string int_data;
	
	while (std::getline(ss_data, int_data, ',')) 
		program.push_back(std::stoll(int_data));
	
	return program;
}

auto get_address(const computer_state& computer, const int param_number) -> int64
{
	const auto mode_position = int(std::pow(10, param_number)) * 10;
	const auto mode = (computer.memory[computer.pc] / mode_position) % 10;

	switch (mode)
	{
		case 1:
			return computer.pc + param_number;
		case 2:
			return computer.memory[computer.pc + param_number] + computer.relative_base;
		default:
			return computer.memory[computer.pc + param_number];
	}
}

auto get_param(const computer_state& computer, const int param_number) -> int64
{
	return computer.memory[get_address(computer, param_number)];
}

auto store_value(computer_state& computer, const unsigned int address, const int64 value)
{
	if (address < computer.memory.size())
		computer.memory[address] = value;
	else
	{
		const auto extension = address - computer.memory.size();
		for (unsigned int i = 0; i < extension; ++i)
			computer.memory.push_back(0);

		computer.memory.push_back(value);
	}
}

auto add(computer_state& computer) -> void
{
	const auto first_param = get_param(computer, 1);
	const auto second_param = get_param(computer, 2);
	const auto result_address = get_address(computer, 3);
	store_value(computer, result_address, first_param + second_param);

	computer.pc += 4;
	
	if constexpr (show_asm)
		std::cout << "add " << result_address << ", " << first_param << ", " << second_param << std::endl;
}

auto mul(computer_state& computer) -> void
{
	const auto first_param = get_param(computer, 1);
	const auto second_param = get_param(computer, 2);
	const auto result_address = get_address(computer, 3);
	store_value(computer, result_address, first_param * second_param);

	computer.pc += 4;
	
	if constexpr (show_asm)
		std::cout << "mul " << result_address << ", " << first_param << ", " << second_param << std::endl;
}

auto in(computer_state& computer) -> void
{
	const auto result_address = get_address(computer, 1);
	store_value(computer, result_address, computer.in_data[computer.in_data_index++]);

	computer.pc += 2;

	if constexpr (show_asm)
		std::cout << "in " << result_address << std::endl;
}

auto out(computer_state& computer) -> void
{
	const auto first_param = get_param(computer, 1);
	computer.out_data.push_back(first_param);

	computer.pc += 2;
	
	if constexpr (show_asm)
		std::cout << "out " << first_param << std::endl;
}

auto jmp_if_true(computer_state& computer) -> void
{
	const auto first_param = get_param(computer, 1);
	const auto second_param = get_param(computer, 2);
	
	if (first_param != 0)
		computer.pc = second_param;
	else
		computer.pc += 3;
		
	if constexpr (show_asm)
			std::cout << "jit " << first_param << ", " << second_param << std::endl;
}

auto jmp_if_false(computer_state& computer) -> void
{
	const auto first_param = get_param(computer, 1);
	const auto second_param = get_param(computer, 2);
	
	if (first_param == 0)
		computer.pc = second_param;
	else
		computer.pc += 3;
		
	if constexpr (show_asm)
		std::cout << "jif " << first_param << ", " << second_param << std::endl;
}

auto lt(computer_state& computer) -> void
{
	const auto first_param = get_param(computer, 1);
	const auto second_param = get_param(computer, 2);
	const auto result_address = get_address(computer, 3);
	store_value(computer, result_address, ((first_param < second_param) ? 1 : 0));

	computer.pc += 4;

	if constexpr (show_asm)
		std::cout << "lt  " << result_address << ", " << first_param << ", " << second_param << std::endl;
}

auto eq(computer_state& computer) -> void
{
	const auto first_param = get_param(computer, 1);
	const auto second_param = get_param(computer, 2);
	const auto result_address = get_address(computer, 3);
	store_value(computer, result_address, ((first_param == second_param) ? 1 : 0));

	computer.pc += 4;
	
	if constexpr (show_asm)
		std::cout << "eq  " << result_address << ", " << first_param << ", " << second_param << std::endl;
}

auto adjust_relative_base(computer_state& computer) -> void
{
	const auto first_param = get_param(computer, 1);
	computer.relative_base += first_param;

	computer.pc += 2;

	if constexpr (show_asm)
		std::cout << "arb " << first_param << std::endl;
}

auto run_program(computer_state& computer, bool return_on_output) -> void
{
	while (!computer.halt)
	{
		switch (const auto opcode = computer.memory[computer.pc] % 100; 
				opcode)
		{
			case 1: add(computer); break;
			case 2: mul(computer); break;
			case 3: in(computer); break;
			case 4:
				out(computer); 
				if (return_on_output) return;
				break;
			case 5: jmp_if_true(computer); break;
			case 6: jmp_if_false(computer); break;
			case 7: lt(computer); break;
			case 8: eq(computer); break;
			case 9: adjust_relative_base(computer); break;
			
			case 99:
				computer.pc++;
				computer.halt = true;
				break;

			default:
				std::cerr << "1202 program alarm" << std::endl;
				computer.halt = true;
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Error! An input file is required!" << std::endl;
		return -1;
	}

	std::ifstream data_file(argv[1]);

	if (!data_file.is_open())
	{
		std::cerr << "Error! Cannot open file: " << argv[1] << std::endl;
		return -1;
	}

	std::string data_program;
	data_file >> data_program;

	computer_state computer;
	computer.memory = load_program(data_program);
	computer.in_data = {1};

	run_program(computer, false);
	std::cout << "BOOST keycode: " << computer.out_data[0] << std::endl;

	computer_state computer2;
	computer2.memory = load_program(data_program);
	computer2.in_data = {2};

	run_program(computer2, false);
	std::cout << "Coordinates of the distress signal: " << computer2.out_data[0] << std::endl;
	
	return 0;
}
