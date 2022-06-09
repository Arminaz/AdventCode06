#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

struct coordinate {
    int x;
    int y;
};

vector<coordinate> coordinates;

coordinate min_box{
	1000000,
	1000000
};

coordinate max_box{
	0,
	0
};

coordinate sum{
		0,
		0
};

struct block_distance {
	int block;
	int distance;
};

void read_coordinate_from_file(const string &file)
{
	string line;
	ifstream infile(file);
	while (getline(infile, line))
	{
		char skip_white_space;
		coordinate temp_coordinate{};
		istringstream in_stream(line);
		in_stream >> temp_coordinate.x >> skip_white_space >> temp_coordinate.y;
		coordinates.push_back(temp_coordinate);
	}
}

void calculate_distance()
{
	for (coordinate &coordinate : coordinates) {
		min_box = {
			min(min_box.x, coordinate.x),
			min(min_box.y, coordinate.y)
		};
		max_box = {
			max(max_box.x, coordinate.x),
			max(max_box.y, coordinate.y)
		};
	}
	for (coordinate &coordinate : coordinates) {

		coordinate = {
			coordinate.x - min_box.x,
			coordinate.y - min_box.y
		};
		sum = {
			sum.x + coordinate.x,
			sum.y + coordinate.y
		};
	}
}

void calculate_sum_distance(std::vector<int> &final_sums, int n_points, int sum)
{
	int distance_sum = sum;
	int temp_distance = n_points;
	for (int &final_sum : final_sums)
	{
		temp_distance -= std::exchange(final_sum, distance_sum) * 2;
		distance_sum -= temp_distance;
	}
}

void calculate_final_distance(std::vector<int> distance_point_x, std::vector<int> distance_point_y, int &total_distance)
{
	int point_y = distance_point_y.size() - 1;
	for (int x : distance_point_x) {
		while (distance_point_y[point_y] + x >= distance_point_x.size()) {
			if (point_y-- == 0) {
				return;
			}
		}
		total_distance += point_y + 1;
	}
}

int main()
{
	read_coordinate_from_file("Input.txt");
	max_box = {
		max_box.x + 1 - min_box.x,
		max_box.y + 1 - min_box.y
	};

	calculate_distance();
	vector<block_distance> a_block(max_box.x * max_box.y);
	vector<coordinate> coords = coordinates, coords_items;
	for (int i = 0; i < coordinates.size(); i++) 
	{
		coordinate &coordinate = coordinates[i];
		a_block[coordinate.y * max_box.x + coordinate.x].block = i + 1;
	}

	auto expand_area = [&coords_items](const block_distance &current_distance_block, block_distance &new_distance_block, int point_x, int point_y, int temp_distance)
	{
		if (new_distance_block.block)
		{
			if (new_distance_block.block != current_distance_block.block)
			{
				if (new_distance_block.distance == temp_distance)
				{
					new_distance_block.block = -1;
				}
			}
		}
		else
		{
			new_distance_block.block = current_distance_block.block;
			new_distance_block.distance = temp_distance;
			coordinate temp_coordinate = {
				point_x,
				point_y
			};
			coords_items.emplace_back(temp_coordinate);
		}
	};

	for (int dist = 1; !coords.empty(); dist++) {
		for (const coordinate &coords_alt : coords) 
		{
			const int block = coords_alt.y * max_box.x + coords_alt.x;
			auto &cur = a_block[block];
			if (coords_alt.y > 0) 
			{
				expand_area(cur, a_block[block - max_box.x], coords_alt.x, coords_alt.y - 1, dist);
			}
			if (coords_alt.x > 0)
			{
				expand_area(cur, a_block[block - 1], coords_alt.x - 1, coords_alt.y, dist);
			}
			if (coords_alt.x < max_box.x - 1) 
			{
				expand_area(cur, a_block[block + 1], coords_alt.x + 1, coords_alt.y, dist);
			}
			if (coords_alt.y < max_box.y - 1)
			{
				expand_area(cur, a_block[block + max_box.x], coords_alt.x, coords_alt.y + 1, dist);
			}
		}
		coords.swap(coords_items);
		coords_items.clear();
	}

	vector<int> block_count(coordinates.size() + 2);
	int block = 0;
	for (int x = 0; x < max_box.x; x++) 
	{
		block_count[a_block[block++].block + 1] = 0;
	}
	for (int y = 2; y < max_box.y; y++) 
	{
		block_count[a_block[block++].block + 1] = 0;
		for (int x = 2; x < max_box.x; x++) {
			block_count[a_block[block++].block + 1]++;
		}
		block_count[a_block[block++].block + 1] = 0;
	}
	for (int x = 0; x < max_box.x; x++) 
	{
		block_count[a_block[block++].block + 1] = 0;
	}

	int area = 0;
	for (int count : block_count) {
		area = max(area, count);
	}
	cout << "Part 1: " << area << '\n';

	std::vector<int> distance_point_x(10000), distance_point_y(10000);
	for_each(begin(coordinates), end(coordinates), [&distance_point_x, &distance_point_y](const coordinate point) mutable {distance_point_x[point.x]++; distance_point_y[point.y]++; });

	calculate_sum_distance(distance_point_x, coordinates.size(), sum.x);
	sort(begin(distance_point_x), end(distance_point_x));

	calculate_sum_distance(distance_point_y, coordinates.size(), sum.y);
	sort(begin(distance_point_y), end(distance_point_y));

	int total_distance = 0;
	calculate_final_distance(distance_point_x, distance_point_y, total_distance);

	cout << "Part 2: " << total_distance << '\n';
	return 0;
}
