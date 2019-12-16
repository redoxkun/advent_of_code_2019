#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

using asteroid_vector = std::vector<std::pair<double, double>>;

enum class quadrant
{
	first,
	fourth,
	third,
	second
};

struct asteroid_relative_pos
{
	std::pair<double, double> asteroid;

	double tan;
	double distance;
	quadrant quad;

	explicit asteroid_relative_pos(const std::pair<double, double>& asteroid, const double tan, const double distance, const quadrant quad) :
		asteroid{asteroid}, tan{tan}, distance{distance}, quad{quad}
	{}

	bool operator<(const asteroid_relative_pos& r) const
	{
		if (quad == r.quad)
		{
			if (tan == r.tan)
				return distance < r.distance;
		
			switch (quad)
			{
				case quadrant::first:
				case quadrant::fourth:
					return r.tan < tan;
					
				case quadrant::third:
				case quadrant::second:
					return tan < r.tan;
			}
		}
		return quad < r.quad;
	}

	bool operator==(const asteroid_relative_pos& r) const
	{
		return (quad == r.quad) && (tan == r.tan);
	}
};

template<typename Container>
auto print_container(const Container& container) -> void
{
	std::cout << "[ ";
	for (const auto& element : container)
		std::cout << element << " ";
	std::cout << "]" << std::endl;
}

template<typename Type1, typename Type2>
auto print_pair(const std::pair<Type1, Type2>& pair) -> void
{
	std::cout << "(" << pair.first << ", " << pair.second << ") ";
}

auto print_asteroid_vector(const asteroid_vector& asteroid_data) -> void
{
	std::cout << "[ ";
	for (const auto& asteroid : asteroid_data)
		print_pair(asteroid);
	std::cout << "]" << std::endl;
}

auto print_relative_pos(const asteroid_relative_pos& pos) -> void
{
	print_pair(pos.asteroid);
	std::cout << ": ";
	switch (pos.quad)
	{
		case quadrant::first: std::cout << "[quadrant: first] "; break;
		case quadrant::second: std::cout << "[quadrant: second] "; break;
		case quadrant::third: std::cout << "[quadrant: third] "; break;
		case quadrant::fourth: std::cout << "[quadrant: fourth] "; break;
	}
	std::cout << "[tan: " << pos.tan << "] [distance: " << pos.distance << "]";
}

auto load_asteroid_data(std::ifstream& data_file) -> asteroid_vector
{
	asteroid_vector asteroid_data;

	std::string row_data;
	auto x = 0;
	auto y = 0;
	
	while (data_file >> row_data)
	{
		for (const auto cell : std::as_const(row_data))
		{
			if (cell == '#')
				asteroid_data.emplace_back(double(x), double(y));
			++x;
		}
		++y;
		x = 0;
	}

	return asteroid_data;
}

constexpr auto get_quadrant(const double x, const double y) -> quadrant
{
	if (x <= 0.0 && y > 0.0)
		return quadrant::first;
	else if (x > 0.0 && y > 0.0)
		return quadrant::second;
	else if (x > 0.0 && y <= 0.0)
		return quadrant::third;
	return quadrant::fourth;	
}

auto get_asteroid_relative_pos(const std::pair<double, double>& asteroid1, const std::pair<double, double>& asteroid2) -> asteroid_relative_pos
{
	const auto x = asteroid1.first - asteroid2.first;
	const auto y = asteroid1.second - asteroid2.second;

	const auto tan = y / x;
	const auto distance = std::sqrt(x * x + y * y);
	const auto quad = get_quadrant(x, y);

	return asteroid_relative_pos(asteroid2, tan, distance, quad);
}

auto get_detectable_asteroids(const asteroid_vector& asteroid_data) -> std::vector<int>
{
	std::vector<int> num_detectable_asteroids;

	for (const auto& asteroid_checking : asteroid_data)
	{
		auto detectable_asteroids = 0;
				
		for (const auto& asteroid_detectable : asteroid_data)
		{
			if (asteroid_detectable == asteroid_checking)
				continue;

			auto blocked = false;

			const auto detectable_relative_pos = get_asteroid_relative_pos(asteroid_checking, asteroid_detectable);

			for (const auto& asteroid_blocking : asteroid_data)
			{
				if (asteroid_blocking == asteroid_detectable ||
					asteroid_blocking == asteroid_checking)
					continue;

				const auto blocking_relative_pos = get_asteroid_relative_pos(asteroid_checking, asteroid_blocking);

				if (blocking_relative_pos == detectable_relative_pos &&
					blocking_relative_pos.distance < detectable_relative_pos.distance)
				{
					blocked = true;
					break;
				}
			}
			if (!blocked)
				++detectable_asteroids;
		}
		num_detectable_asteroids.push_back(detectable_asteroids);
	}

	return num_detectable_asteroids;
}

auto get_asteroids_relative_pos(const asteroid_vector& asteroid_data, const std::pair<double, double>& asteroid) -> std::vector<asteroid_relative_pos>
{
	std::vector<asteroid_relative_pos> relative_pos;

	for (const auto& asteroid_relative : asteroid_data)
	{
		if (asteroid == asteroid_relative)
			continue;

		relative_pos.push_back(get_asteroid_relative_pos(asteroid, asteroid_relative));
	}
	
	return relative_pos;
}

template<typename Type>
auto vector_vectors_to_vector(const std::vector<std::vector<Type>>& vector_vectors) -> std::vector<Type>
{
	std::vector<Type> vector;

	for (const auto& current_vector : vector_vectors)
	{
		for (const Type& element : current_vector)
			vector.push_back(element);
	}

	return vector;
}

auto print_rel_pos_vector(const std::vector<asteroid_relative_pos>& vector) -> void
{
	for (const auto& element : vector)
	{
		std::cout << " * ";
		print_relative_pos(element);
		std::cout << std::endl;
	}
}

auto get_ordered_vaporized_asteroids(const asteroid_vector& asteroid_data, const std::pair<double, double> asteroid) -> asteroid_vector
{
	auto relative_pos = get_asteroids_relative_pos(asteroid_data, asteroid);

	/*std::cout << "Unsorted relative pos:" << std::endl;
	print_rel_pos_vector(relative_pos);*/

	std::sort(relative_pos.begin(), relative_pos.end());

	/*std::cout << "Sorted relative pos:" << std::endl;
	print_rel_pos_vector(relative_pos);*/

	std::vector<asteroid_vector> asteroid_vectors;
	auto prev_asteroid = asteroid_relative_pos(asteroid, 0.0, 0.0, quadrant::second);
	auto round = 0;

	asteroid_vectors.emplace_back();
	
	for (const auto& asteroid_relative_pos : std::as_const(relative_pos))
	{
		if (prev_asteroid == asteroid_relative_pos)
		{
			++round;
			if (int(asteroid_vectors.size()) >= round)
				asteroid_vectors.emplace_back();
		}
		else
			round = 0;
			
		asteroid_vectors[round].push_back(asteroid_relative_pos.asteroid);
		prev_asteroid = asteroid_relative_pos;
	}

	return vector_vectors_to_vector(asteroid_vectors);
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

	const auto asteroid_data = load_asteroid_data(data_file);

	//print_asteroid_vector(asteroid_data);

	const auto num_detectable_asteroids = get_detectable_asteroids(asteroid_data);

	//print_container(num_detectable_asteroids);

	const auto max_detectable = std::max_element(num_detectable_asteroids.begin(), num_detectable_asteroids.end());

	std::cout << "Detectable asteroids from best position: " << *max_detectable << std::endl;

	const auto asteroid = asteroid_data.at(std::distance(num_detectable_asteroids.begin(), max_detectable));

	/*std::cout << "Best position asteroid: ";
	print_pair(asteroid);
	std::cout << std::endl;*/

	const auto vaporized_asteroids = get_ordered_vaporized_asteroids(asteroid_data, asteroid);
	//print_asteroid_vector(vaporized_asteroids);

	std::cout << "200th asteroid to be vaporized: ";
	print_pair(vaporized_asteroids[199]);
	std::cout << std::endl;
	
	return 0;
}
