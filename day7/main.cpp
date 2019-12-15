#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

constexpr auto show_asm = false;

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

	std::vector<int> memory;
	
	std::vector<int> in_data;
	int in_data_index;
	
	std::vector<int> out_data;

	explicit computer_state() : halt{false}, pc{0}, in_data_index{0}
	{}
};

auto print_computer_state(const computer_state& computer) -> void
{
	std::cout << "--- Computer state ---" << std::endl;
	std::cout << " halt: " << computer.halt << std::endl;
	std::cout << " pc:   " << computer.pc << std::endl;
	std::cout << " in:   ";
	print_container(computer.in_data);
	std::cout << " in_i: " << computer.in_data_index;
	std::cout << " out:  ";
	print_container(computer.out_data);
	std::cout << "----------------------" << std::endl;
}

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

	if constexpr (show_asm)
		std::cout << "add " << memory[pc + 3] << ", " << first_param << ", " << second_param << std::endl;

	pc += 4;
}

auto mul(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	const auto second_param = (immediate_2nd_param(memory[pc])) ? memory[pc + 2] : memory[memory[pc + 2]];
	memory[memory[pc + 3]] = first_param * second_param;

	if constexpr (show_asm)
		std::cout << "mul " << memory[pc + 3] << ", " << first_param << ", " << second_param << std::endl;
	
	pc += 4;
}

auto in(std::vector<int>& memory, int& pc, const std::vector<int>& input_data, int& input_index) -> void
{
	memory[memory[pc + 1]] = input_data[input_index++];

	if constexpr (show_asm)
		std::cout << "in " << memory[pc + 1] << std::endl;

	pc += 2;
}

auto out(std::vector<int>& memory, int& pc, std::vector<int>& output_data) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	output_data.push_back(first_param);

	if constexpr (show_asm)
		std::cout << "out " << first_param << std::endl;

	pc += 2;
}

auto jmp_if_true(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	const auto second_param = (immediate_2nd_param(memory[pc])) ? memory[pc + 2] : memory[memory[pc + 2]];

	if constexpr (show_asm)
		std::cout << "jit " << first_param << ", " << second_param << std::endl;
	
	if (first_param != 0)
		pc = second_param;
	else
		pc += 3;
}

auto jmp_if_false(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	const auto second_param = (immediate_2nd_param(memory[pc])) ? memory[pc + 2] : memory[memory[pc + 2]];

	if constexpr (show_asm)
		std::cout << "jif " << first_param << ", " << second_param << std::endl;
	
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

	if constexpr (show_asm)
		std::cout << "lt  " << memory[pc + 3] << ", " << first_param << ", " << second_param << std::endl;
		
	pc += 4;	
}

auto eq(std::vector<int>& memory, int& pc) -> void
{
	const auto first_param  = (immediate_1st_param(memory[pc])) ? memory[pc + 1] : memory[memory[pc + 1]];
	const auto second_param = (immediate_2nd_param(memory[pc])) ? memory[pc + 2] : memory[memory[pc + 2]];
	memory[memory[pc + 3]] = (first_param == second_param) ? 1 : 0;

	if constexpr (show_asm)
		std::cout << "eq  " << memory[pc + 3] << ", " << first_param << ", " << second_param << std::endl;
	
	pc += 4;
}

auto run_program(computer_state& computer, bool return_on_output) -> void
{
	while (!computer.halt)
	{
		switch (const auto opcode = computer.memory[computer.pc] % 100; 
				opcode)
		{
			case 1: add(computer.memory, computer.pc); break;
			case 2: mul(computer.memory, computer.pc); break;
			case 3: in(computer.memory, computer.pc, computer.in_data, computer.in_data_index); break;
			case 4:
				out(computer.memory, computer.pc, computer.out_data); 
				if (return_on_output) return;
				break;
			case 5: jmp_if_true(computer.memory, computer.pc); break;
			case 6: jmp_if_false(computer.memory, computer.pc); break;
			case 7: lt(computer.memory, computer.pc); break;
			case 8: eq(computer.memory, computer.pc); break;
			
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

auto get_thruster_signal_from_sequence(const std::string& sequence, const std::string& program) -> int
{
	auto value = 0;
	
	for (const auto phase_setting : sequence)
	{
		computer_state computer;
		computer.memory = load_program(program);
		computer.in_data = {int(phase_setting - '0'), value};
		run_program(computer, false);
		value = computer.out_data[0];
	}
	
	return value;
}

auto init_amplifiers_feedback_loop(const std::string& sequence, const std::string& program) -> std::vector<computer_state>
{
	std::vector<computer_state> amplifiers;

	for (const auto phase_setting : sequence)
	{
		computer_state computer;
		computer.memory = load_program(program);
		computer.in_data = {int(phase_setting - '0')};
		amplifiers.push_back(std::move(computer));
	}

	return amplifiers;
}

auto get_thruster_signal_from_sequence_feedback_loop(const std::string& sequence, const std::string& program) -> int
{
	auto value = 0;
	auto stop = false;

	std::vector<computer_state> amplifiers = init_amplifiers_feedback_loop(sequence, program);

	while (!stop)
	{
		for (auto& amplifier : amplifiers)
		{
			if (amplifier.halt)
			{
				stop = true;
				break;
			}
			
			amplifier.in_data.push_back(value);
			//print_computer_state(amplifier);
			run_program(amplifier, true);
			value = amplifier.out_data.back();
			//std::cout << "last output: " << value << std::endl;
		}
	}	

	return value;
}

auto is_valid_phase_setting_sequence(const std::string& sequence, const char from_char, const char to_char) -> bool
{
	std::array<int, 5> found{0, 0, 0, 0, 0};

	for (const auto phase_setting : sequence)
	{
		if (phase_setting < from_char || phase_setting > to_char)
			return false;

		const auto pos = int(phase_setting - from_char);

		if (found[pos])
			return false;

		found[pos] = 1;
	}

	return true;
}

auto get_max_thruster_signal(const std::string& program) -> int
{
	auto max_thruster = 0;
	for (auto phase = 100000; phase < 150000; ++phase)
	{
		const auto phase_seq = std::to_string(phase).substr(1);
		if (is_valid_phase_setting_sequence(phase_seq, '0', '4'))
		{
			const auto thruster = get_thruster_signal_from_sequence(phase_seq, program);
			max_thruster = std::max(max_thruster, thruster);
		}
	}
	return max_thruster;
}

auto get_max_thruster_signal_feedback_loop(const std::string& program) -> int
{
	auto max_thruster = 0;
	for (auto phase = 150000; phase < 200000; ++phase)
	{
		const auto phase_seq = std::to_string(phase).substr(1);
		if (is_valid_phase_setting_sequence(phase_seq, '5', '9'))
		{
			const auto thruster = get_thruster_signal_from_sequence_feedback_loop(phase_seq, program);
			max_thruster = std::max(max_thruster, thruster);
		}
	}
	return max_thruster;
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

	const auto max_thruster = get_max_thruster_signal(data_program);
	std::cout << "Max thruster signal: " << max_thruster << std::endl;

	const auto max_thruster_feedback_loop = get_max_thruster_signal_feedback_loop(data_program);
	std::cout << "Max thruster signal feedback loop: " << max_thruster_feedback_loop << std::endl;
	
	return 0;
}
