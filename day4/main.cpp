#include <iostream>
#include <string>

auto is_valid_password_part1(const std::string& password) -> bool
{
	auto last_char = '0';
	auto adjacent = false;
	
	for (const auto new_char : password)
	{
		if (new_char < last_char)
			return false;

		if (last_char == new_char)
			adjacent = true;

		last_char = new_char;
	}

	return adjacent;
}

auto is_valid_password_part2(const std::string& password) -> bool
{
	auto last_char = '0';
	auto adjacent = false;
	auto adjacent_num = 1;

	for (const auto new_char : password)
	{
		if (new_char < last_char)
			return false;

		if (new_char == last_char)
			adjacent_num++;
		else
		{
			if (adjacent_num == 2)
				adjacent = true;
			adjacent_num = 1;
		}

		last_char = new_char;
	}

	if (adjacent_num == 2)
		return true;
	
	return adjacent;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "Error! Two ints are required!" << std::endl;
		return -1;
	}

	const auto start = atoi(argv[1]);
	const auto end = atoi(argv[2]);

	std::cout << "Passwords from " << start << " to " << end << std::endl;

	auto num_passwords = 0;
	for (auto password = start; password <= end; ++password)
	{
		if (is_valid_password_part1(std::to_string(password)))
			num_passwords++;
	}

	std::cout << "Part 1: " << num_passwords << " passwords found." << std::endl;

	num_passwords = 0;
	for (auto password = start; password <= end; ++password)
	{
		if (is_valid_password_part2(std::to_string(password)))
			num_passwords++;
	}

	std::cout << "Part 2: " << num_passwords << " passwords found." << std::endl;
	
	return 0;
}
