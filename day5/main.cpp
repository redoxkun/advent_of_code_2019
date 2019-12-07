#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

auto load_program(const std::string& data) -> std::vector<int>
{
	std::vector<int> program;

	std::stringstream ss_data(data);
	std::string int_data;
	
	while (std::getline(ss_data, int_data, ',')) 
		program.push_back(std::stoi(int_data));
	
	return program;
}

constexpr auto immediate_1st_param(int value) -> int
{
	return (value / 100) % 10; 
}

constexpr auto immediate_2nd_param(int value) -> int
{
	return (value / 1000) % 10; 
}

auto add(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	const auto second_param = (immediate_2nd_param(memory[pc])) ? memory[pc + 2] : memory[memory[pc + 2]];	
	memory[memory[pc + 3]] = first_param + second_param;
	pc += 4;
}

auto mul(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	const auto second_param = (immediate_2nd_param(memory[pc])) ? memory[pc + 2] : memory[memory[pc + 2]];
	memory[memory[pc + 3]] = first_param * second_param;
	pc += 4;
}

auto in(std::vector<int>& memory, int& pc) -> void
{
	int input_value;
	std::cout << ">";
	std::cin >> input_value;
	memory[memory[pc + 1]] = input_value;
	pc += 2;
}

auto out(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	std::cout << first_param << std::endl;
	pc += 2;
}

auto jmp_if_true(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	const auto second_param = (immediate_2nd_param(memory[pc])) ? memory[pc + 2] : memory[memory[pc + 2]];
	if (first_param != 0)
		pc = second_param;
	else
		pc += 3;
}

auto jmp_if_false(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	const auto second_param = (immediate_2nd_param(memory[pc])) ? memory[pc + 2] : memory[memory[pc + 2]];
	if (first_param == 0)
		pc = second_param;
	else
		pc += 3;
}

auto lt(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	const auto second_param = (immediate_2nd_param(memory[pc])) ? memory[pc + 2] : memory[memory[pc + 2]];
	memory[memory[pc + 3]] = (first_param < second_param) ? 1 : 0;	
	pc += 4;
}

auto eq(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	const auto second_param = (immediate_2nd_param(memory[pc])) ? memory[pc + 2] : memory[memory[pc + 2]];
	memory[memory[pc + 3]] = (first_param == second_param) ? 1 : 0;
	pc += 4;
}

auto run_program(std::vector<int>& memory) -> void
{
	auto pc = 0;
	auto end = false;

	while (!end)
	{
		const auto opcode = memory[pc] % 100;
		switch (opcode)
		{
			case 1:
				add(memory, pc);
				break;

			case 2:
				mul(memory, pc);
				break;

			case 3:
				in(memory, pc);
				break;

			case 4:
				out(memory, pc);
				break;

			case 5:
				jmp_if_true(memory, pc);
				break;

			case 6:
				jmp_if_false(memory, pc);
				break;

			case 7:
				lt(memory, pc);
				break;

			case 8:
				eq(memory, pc);
				break;

			case 99:
				pc++;
				end = true;
				break;

			default:
				std::cerr << "1202 program alarm" << std::endl;
				end = true;
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
	
	auto memory = load_program(data_program);

	run_program(memory);
	
	return 0;
}
