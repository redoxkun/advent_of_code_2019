#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

constexpr auto output_filename = "image.ppm";

auto get_color_at_pos(const std::vector<std::vector<int>>& image, const int pos) -> int
{
	for (const auto& layer : image)
	{
		const auto color = layer[pos];
		if (color != 2)
			return color;
	}	
	return 2;
}

auto space_color_to_ppm(const int color) -> std::string
{
	switch (color)
	{
		case 0:
			return "0 0 0";
		case 1:
			return "255 255 255";
		default:
			return "0 0 255";
	}
}

auto space_image_to_ppm(const std::vector<std::vector<int>>& image, const int width, const int height) -> void
{
	std::ofstream ppm_file(output_filename);
	ppm_file << "P3\n";
	ppm_file << width << " " << height << " 255\n";

	for (auto y = 0; y < height; ++y)
	{
		for (auto x = 0; x < width; ++x)
		{
			const auto pos = y * width + x;
			const auto color = get_color_at_pos(image, pos);
			ppm_file << space_color_to_ppm(color) << " ";
		}
		ppm_file << "\n";
	}
}

auto load_space_image(const std::string& data_image, const int width, const int height) -> std::vector<std::vector<int>>
{
	std::vector<std::vector<int>> image;

	const auto layer_size = width * height;

	auto current_layer_index = 0;
	image.emplace_back();

	for (const auto data_value : data_image)
	{
		if (current_layer_index == layer_size)
		{
			image.emplace_back();
			current_layer_index = 0;
		}
	
		image.back().push_back(int(data_value - '0'));
		++current_layer_index;
	}

	return image;
}

auto get_fewest_zero_digits_layer_index(const std::vector<std::vector<int>>& image) -> int
{
	auto layer_index = -1;
	auto fewest_zero_digits = std::numeric_limits<int>::max();

	const auto image_size = int(image.size());
	
	for (auto i = 0; i < image_size; ++i)
	{
		const auto zero_digits = std::count(image[i].begin(), image[i].end(), 0);

		if (zero_digits < fewest_zero_digits)
		{
			layer_index = i;
			fewest_zero_digits = zero_digits;
		}
	}

	return layer_index;
}

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		std::cerr << "Error! And input file and width and height are required!" << std::endl;
		return -1;
	}

	std::ifstream data_file(argv[1]);

	if (!data_file.is_open())
	{
		std::cerr << "Error! Cannot open file: " << argv[1] << std::endl;
		return -1;
	}

	std::string data_image;
	data_file >> data_image;

	const auto width = std::atoi(argv[2]);
	const auto height = std::atoi(argv[3]);

	const auto image = load_space_image(data_image, width, height);

	const auto fewest_zero_digits_layer = image[get_fewest_zero_digits_layer_index(image)];

	const auto one_digits = std::count(fewest_zero_digits_layer.begin(), fewest_zero_digits_layer.end(), 1);
	const auto two_digits = std::count(fewest_zero_digits_layer.begin(), fewest_zero_digits_layer.end(), 2);

	std::cout << "Fewest 0 digits layer, 1 digits * 2 digits: " << (one_digits * two_digits) << std::endl;

	space_image_to_ppm(image, width, height);
			
	return 0;
}
