#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

using orbit_tree = std::unordered_map<std::string, std::vector<std::string>>;

auto get_total_orbits(const orbit_tree& tree, const std::string& top, int depth, int& orbits) -> void
{
	orbits += depth;
	
	if (tree.count(top) == 0)
		return;

	const auto& childs = tree.at(top);

	for (const auto& child : childs)
		get_total_orbits(tree, child, depth + 1, orbits);
}

auto get_shortest_distance(const orbit_tree& tree, const std::string& top, int depth,
							const std::string& node1, const std::string& node2,
							int& distance) -> std::pair<int, int>
{
	std::pair<int, int> result{-1, -1};

	if (distance != -1)
		return result;
	
	if (top == node1)
		result.first = depth;

	if (top == node2)
		result.second = depth;

	if (tree.count(top) == 0)
		return result;

	const auto& childs = tree.at(top);

	for (const auto& child : childs)
	{
		auto child_result = get_shortest_distance(tree, child, depth + 1, node1, node2, distance);

		if (distance != -1)
			return child_result;

		if (child_result.first != -1)
			result.first = child_result.first;

		if (child_result.second != -1)
			result.second = child_result.second;

		if (result.first != -1 && result.second != -1)
		{
			distance = (result.first - depth) + (result.second - depth) - 2;
			return result;
		}
	}

	return result;
}

auto load_orbit_tree(std::ifstream& data_file) -> orbit_tree
{
	orbit_tree tree;

	std::string line;
	while (data_file >> line)
	{
		const auto separator_index = line.find(')');
		const auto parent = line.substr(0, separator_index);
		tree[parent].push_back(line.substr(separator_index + 1));
	}

	return tree;
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

	auto tree = load_orbit_tree(data_file);

	auto total_orbits = 0; 
	get_total_orbits(tree, "COM", 0, total_orbits);

	std::cout << "Total orbits: " << total_orbits << std::endl;

	auto distance = -1;
	get_shortest_distance(tree, "COM", 0, "YOU", "SAN", distance);
	std::cout << "Minimum number of orbital transfers: " << distance << std::endl;
		
	return 0;
}
