#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

constexpr auto DEBUG = true;
constexpr int STEPS = 10000000;

typedef std::vector< std::vector<int> > PLANE;
constexpr int PLANE_SIZE = 32;

struct RGB {
	short r;
	short g;
	short b;
};

const std::vector<RGB> colours = {
	{  0,   0,   0},
	{255, 255, 255}
};


class Ant {
private:
	std::string rule;

	int dir; // 0 = North, 1 = East, 2 = South, 3 = West
	int x, y;

public:
	Ant(std::string rule) {
		this->rule = rule;

		dir = 0;
		x = y = PLANE_SIZE / 2;
	}

	void step(PLANE *plane) {
		if (rule[plane->at(x).at(y)] == 'L') dir--;
		if (rule[plane->at(x).at(y)] == 'R') dir++;

		if (dir == -1) dir = 3;
		if (dir ==  4) dir = 0;

		plane->at(x).at(y)++;
		if (plane->at(x).at(y) > rule.length() - 1) {
			plane->at(x).at(y) = 0;
		}

		if (dir == 0) y--;
		if (dir == 1) x++;
		if (dir == 2) y++;
		if (dir == 3) x--;

		if (x == -1) x = PLANE_SIZE - 1;
		if (y == -1) y = PLANE_SIZE - 1;
		if (x == PLANE_SIZE) x = 0;
		if (y == PLANE_SIZE) y = 0;
	}
};


void writeToPPM(PLANE *plane, std::vector<RGB> colours) {
	std::ofstream ppm("output.ppm");

	ppm << "P3" << std::endl;
	ppm << PLANE_SIZE << " " << PLANE_SIZE << std::endl;
	ppm << "255" << std::endl;

	int mr, mg, mb; // m = max
	mr = mg = mb = 0;

	for (auto x = 0; x < PLANE_SIZE; x++) {
		for (auto y = 0; y < PLANE_SIZE; y++) {
			ppm << colours[plane->at(x).at(y)].r << " "
				<< colours[plane->at(x).at(y)].g << " "
				<< colours[plane->at(x).at(y)].b << " ";

			if (DEBUG) {
				if (colours[plane->at(x).at(y)].r > mr) mr = colours[plane->at(x).at(y)].r;
				if (colours[plane->at(x).at(y)].g > mg) mg = colours[plane->at(x).at(y)].g;
				if (colours[plane->at(x).at(y)].b > mb) mb = colours[plane->at(x).at(y)].b;
			}
		}
	}

	if (DEBUG) {
		std::cout << "[DEBUG] Largest colour values written: " << mr << ", " << mg << ", " << mb << std::endl;
	}

	ppm.close();
}


std::string strToRule(std::string str) {
	std::string strAsBin = "";
	std::string rule = "";

	for (char c : str) {
		char binary[33];
		_itoa_s(c, binary, 2);
		strAsBin += binary;
	}

	for (char c : strAsBin) {
		if (c == '0') {
			rule += "L";
		}
		else {
			rule += "R";
		}
	}

	return rule;
}


void generateNcodeImg(std::string str) {
	// Create plane (grid) for ant
	PLANE plane;
	plane.resize(PLANE_SIZE);
	for (auto i = 0; i < PLANE_SIZE; i++) {
		plane[i].resize(PLANE_SIZE);
	}

	str = strToRule(str);
	Ant ant(str);

	for (auto i = 0; i < STEPS; i++) {
		ant.step(&plane);
	}

	// Ensure that there are enough colours defined
	std::vector<RGB> l_colours;

	if (str.length() > colours.size()) {
		for (auto r = 0; r < 256; r += 10) {
			for (auto g = 0; g < 256; g += 10) {
				for (auto b = 0; b < 256; b += 10) {
					RGB col = { (short)r, (short)g, (short)b };
					l_colours.push_back(col);

					if (str.length() == l_colours.size()) {
						if (DEBUG) {
							std::cout << "[DEBUG] Generated up to: " << r << ", " << g << ", " << b << std::endl;
						}

						goto escape;
					}
				}
			}
		}
	}
	else {
		l_colours = colours;
	}

escape:
	writeToPPM(&plane, l_colours);
}


int main(int argc, char **args) {
	if (argc == 3) {
		if (strcmp(args[1], "gen") == 0) { // Generate (ncode img)
			generateNcodeImg(args[2]); // Saves to output.ppm
			std::cout << "Generated output.ppm";
		}
		else {
			std::cout << "Usage: " << args[0] << " gen password";
		}
	}
	else if (argc == 4) {
		if (strcmp(args[1], "check") == 0) {
			generateNcodeImg(args[3]);

			std::ifstream given("output.ppm");
			std::stringstream given_str;
			given_str << given.rdbuf();

			std::ifstream existing(args[2]);
			std::stringstream existing_str;
			existing_str << existing.rdbuf();

			if (given_str.str() == existing_str.str()) {
				std::cout << "Password is correct.";
			}
			else {
				std::cout << "Password doesn't match given ncode img.";
			}
		}
		else {
			std::cout << "Usage: " << args[0] << " check ncode_img password";
		}
	}
	else {
		std::cout << "Invalid command given.";
	}

	return 0;
}
