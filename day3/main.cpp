#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>
#include <optional>
#include <sstream>
#include <vector>

auto get_inc_from_direction(const char direction) -> std::pair<int, int>
{
	switch (direction)
	{
		case 'U':
			return { 0, -1};
		case 'D':
			return { 0,  1};
		case 'L':
			return {-1,  0};
		case 'R':
			return { 1,  0};
	}
	return {0, 0};
}

auto add_next_section(const std::string& section, std::vector<std::pair<int, int>>& wire) -> void
{
	const auto size = std::stoi(section.substr(1));
	const auto direction = section[0];		

	const auto[inc_x, inc_y] = get_inc_from_direction(direction);

	const auto last_point = wire.back();
	wire.emplace_back(size * inc_x + last_point.first, 
					  size * inc_y + last_point.second);
}

auto get_wire_vector(const std::string& wire_raw) -> std::vector<std::pair<int, int>>
{
	std::vector<std::pair<int, int>> wire;

	std::stringstream ss_wire_raw(wire_raw);
	std::string section;

	wire.emplace_back(0, 0);

	while (std::getline(ss_wire_raw, section, ','))
		add_next_section(section, wire);

	return wire;
}

auto get_intersection_points(const std::pair<int, int>& wire1_start, const std::pair<int, int>& wire1_end,
							 const std::pair<int, int>& wire2_start, const std::pair<int, int>& wire2_end) -> std::vector<std::pair<int, int>>
{
	if (wire1_start.first == wire1_end.first)
	{
		if (wire2_start.first == wire2_end.first)
		{
			if (wire1_start.first == wire2_start.first)
			{
				const auto[w1_min_pos, w1_max_pos] = std::minmax(wire1_start.second, wire1_end.second);
				const auto[w2_min_pos, w2_max_pos] = std::minmax(wire2_start.second, wire2_end.second);
				const auto start_pos = std::max(w1_min_pos, w2_min_pos);
				const auto end_pos = std::min(w1_max_pos, w2_max_pos);

				std::vector<std::pair<int, int>> result;
				for (auto i = start_pos; i <= end_pos; ++i)
					result.emplace_back(wire1_start.first, i);
				return result;	
			}
		}
		else
		{
			const auto[w1_min_pos, w1_max_pos] = std::minmax(wire1_start.second, wire1_end.second);
			const auto[w2_min_pos, w2_max_pos] = std::minmax(wire2_start.first, wire2_end.first);
			if (w1_min_pos <= wire2_start.second && w1_max_pos >= wire2_start.second &&
				w2_min_pos <= wire1_start.first && w2_max_pos >= wire1_start.first)
				return {{wire1_start.first, wire2_start.second}};
		}
	}
	else
	{
		if (wire2_start.second == wire2_end.second)
		{
			if (wire1_start.second == wire2_start.second)
			{
				const auto[w1_min_pos, w1_max_pos] = std::minmax(wire1_start.first, wire1_end.first);
				const auto[w2_min_pos, w2_max_pos] = std::minmax(wire2_start.first, wire2_end.first);
				const auto start_pos = std::max(w1_min_pos, w2_min_pos);
				const auto end_pos = std::min(w1_max_pos, w2_max_pos);

				std::vector<std::pair<int, int>> result;
				for (auto i = start_pos; i <= end_pos; ++i)
					result.emplace_back(i, wire1_start.second);
				return result;
			}
		}
		else
		{
			const auto[w1_min_pos, w1_max_pos] = std::minmax(wire1_start.first, wire1_end.first);
			const auto[w2_min_pos, w2_max_pos] = std::minmax(wire2_start.second, wire2_end.second);
			if (w1_min_pos <= wire2_start.first && w1_max_pos >= wire2_start.first &&
				w2_min_pos <= wire1_start.second && w2_max_pos >= wire1_start.second)
				return {{wire2_start.first, wire1_start.second}};
		}
	}
	
	return {};
}

auto get_intersections(const std::vector<std::pair<int, int>>& wire1, const std::vector<std::pair<int, int>>& wire2) ->
	std::vector<std::pair<int, int>>
{
	std::vector<std::pair<int, int>> intersections;

	auto wire1_previous = wire1.front();
	auto wire2_previous = wire2.front();

	std::for_each(++wire1.begin(), wire1.end(), [&] (const std::pair<int, int>& wire1_point) {
	
		std::for_each(++wire2.begin(), wire2.end(), [&] (const std::pair<int, int>& wire2_point) {
		
			const auto new_intersections = get_intersection_points(wire1_previous, wire1_point, wire2_previous, wire2_point);
			std::copy(new_intersections.begin(), new_intersections.end(), std::back_inserter(intersections));
			
			wire2_previous = wire2_point;
		});
		
		wire1_previous = wire1_point;
		wire2_previous = wire2.front();
	});

	return intersections;
}

auto get_distance(const std::pair<int, int>& point) -> int
{
	return static_cast<long long>(std::abs(point.first)) + static_cast<long long>(std::abs(point.second));
}

auto get_closest_intersection(const std::vector<std::pair<int, int>>& intersections) -> std::pair<int, int>
{
	std::pair<int, int> closest{std::numeric_limits<int>::max()/2, std::numeric_limits<int>::max()/2};

	std::for_each(++intersections.begin(), intersections.end(), 
		[&closest] (const std::pair<int, int>& intersection) {
			if (get_distance(intersection) < get_distance(closest))
				closest = intersection;
		});
	
	return closest;
}

auto get_points_distance(const std::pair<int, int>& point1, const std::pair<int, int>& point2) -> int
{
	return std::abs(point1.first - point2.first) + std::abs(point1.second - point2.second);
}

auto intersects_segment(const std::pair<int, int>& point1, const std::pair<int, int>& point2,
						 const std::pair<int, int>& intersection) -> bool
{
	if (point1.first == point2.first && point1.first == intersection.first)
	{
		const auto[second_min, second_max] = std::minmax(point1.second, point2.second);
		return (second_min <= intersection.second && second_max >= intersection.second);
	}

	if (point1.second == point2.second && point1.second == intersection.second)
	{
		const auto[first_min, first_max] = std::minmax(point1.first, point2.first);
		return (first_min <= intersection.first && first_max >= intersection.first);
	}

	return false;
}

auto get_shortest_intersection_distance (const std::vector<std::pair<int, int>>& wire, const std::pair<int, int>& intersection) -> int
{
	auto distance = 0;
	auto first_elem = true;
	auto start_point = wire.front();

	for (const auto& end_point : wire) 
	{
		if (first_elem)
		{
			first_elem = false;
			continue;
		}
		
		if (intersects_segment(start_point, end_point, intersection))
			return distance + get_points_distance(start_point, intersection);
			
		distance += get_points_distance(start_point, end_point);
		start_point = end_point;
	}

	return distance;
}

auto get_shortest_intersections_distance(const std::vector<std::pair<int, int>>& wire1, const std::vector<std::pair<int, int>>& wire2,
										 const std::vector<std::pair<int, int>>& intersections) -> int
{
	auto shortest_distance = std::numeric_limits<int>::max();
	
	std::for_each(++intersections.begin(), intersections.end(), [&] (const std::pair<int, int>& intersection) {

		auto distance = get_shortest_intersection_distance (wire1, intersection);
		distance += get_shortest_intersection_distance (wire2, intersection);
		
		if (distance < shortest_distance)
			shortest_distance = distance;
	});
	
	return shortest_distance;
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

	std::string wire1_raw;
	data_file >> wire1_raw;
	const auto wire1 = get_wire_vector(wire1_raw);
	
	std::string wire2_raw;
	data_file >> wire2_raw;
	const auto wire2 = get_wire_vector(wire2_raw);

	const auto intersections = get_intersections(wire1, wire2);

	const auto closest_intersection = get_closest_intersection(intersections);	 

	std::cout << "Closest intersection to center point: (" << closest_intersection.first << ", " << closest_intersection.second << ")" << std::endl;
	std::cout << "Manhattan distance: " << get_distance(closest_intersection) << std::endl;

	const auto shortest_intersections_distance = get_shortest_intersections_distance(wire1, wire2, intersections);

	std::cout << "Shortest distance: " << shortest_intersections_distance << std::endl;

	return 0;
}
