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

auto add(std::vector<int>& memory, int& pc) -> void
{
	//std::cout << "add " << memory[pc + 3] << ", " << memory[pc + 1] << ", " << memory[pc + 2] << std::endl;
	memory[memory[pc + 3]] = memory[memory[pc + 1]] + memory[memory[pc + 2]];
	pc += 4;
}

auto mul(std::vector<int>& memory, int& pc) -> void
{
	//std::cout << "mul " << memory[pc + 3] << ", " << memory[pc + 1] << ", " << memory[pc + 2] << std::endl;
	memory[memory[pc + 3]] = memory[memory[pc + 1]] * memory[memory[pc + 2]];
	pc += 4;
}

auto run_program(std::vector<int>& memory) -> void
{
	auto pc = 0;
	auto end = false;

	while (!end)
	{
		switch (memory[pc])
		{
			case 1:
				add(memory, pc);
				break;

			case 2:
				mul(memory, pc);
				break;

			case 99:
				//std::cout << "halt" << std::endl;
				pc++;
				end = true;
				break;

			default:
				std::cerr << "1202 program alarm" << std::endl;
				end = true;
		}
	}
}

auto find_result(const int result, const std::vector<int>& memory, int& noun, int& verb) -> bool
{
	for (noun = 0; noun < 100; ++noun)
	{
		for (verb = 0; verb < 100; ++verb)
		{
			auto memory_copy = memory;
			memory_copy[1] = noun;
			memory_copy[2] = verb;

			run_program(memory_copy);

			if (result == memory_copy[0])
				return true;
		}
	}
	return false;
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

	if (argc == 2)
	{
		run_program(memory);

		std::cout << "Value at position 0: " << memory[0] << std::endl;
	}
	else
	{
		const auto result = atoi(argv[2]);
		int noun;
		int verb;

		if (find_result(result, memory, noun, verb))
		{
			std::cout << "Result " << result << " found with noun: " << noun << " and verb: " << verb << std::endl;
			std::cout << "Answer: " << (100 * noun + verb) << std::endl; 
		}
		else
		{
			std::cout << "No noun and verb found for result: " << result << std::endl;
		}
	}
	
	return 0;
}
