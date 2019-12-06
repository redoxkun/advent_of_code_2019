#include <iostream>
#include <fstream>

constexpr auto fuel_required(long long mass)
{
	const auto fuel = mass / 3 - 2;
	return fuel < 0 ? 0 : fuel;
}

constexpr auto fuel_fuel_required(long long mass)
{
	long long fuel = 0;
	while (mass > 0)
	{
		mass = fuel_required(mass);
		fuel += mass;
	}
	return fuel;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
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
	
	long long modules_fuel = 0;
	long long total_fuel = 0;
	long long mass;

	while (data_file >> mass)
	{	
		const auto module_fuel = fuel_required(mass);
		const auto module_fuel_fuel = fuel_fuel_required(module_fuel);
		modules_fuel += module_fuel;
		total_fuel += module_fuel + module_fuel_fuel;
	}

	std::cout << "Fuel required for modules: " << modules_fuel << std::endl;
	std::cout << "Fuel required for modules and fuel: " << total_fuel << std::endl;
	
	return 0;
}
