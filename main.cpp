#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

constexpr int STEPS = 1000000;

typedef std::vector< std::vector<int> > PLANE;
constexpr int PLANE_SIZE = 64;

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

	for (auto x = 0; x < PLANE_SIZE; x++) {
		for (auto y = 0; y < PLANE_SIZE; y++) {
			ppm << colours[plane->at(x).at(y)].r << " "
				<< colours[plane->at(x).at(y)].g << " "
				<< colours[plane->at(x).at(y)].b << " ";
		}
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
		for (auto r = 0; r < 256; r++) {
			for (auto g = 0; g < 256; g++) {
				for (auto b = 0; b < 256; b++) {
					RGB col = { (short)r, (short)g, (short)b };
					l_colours.push_back(col);

					if (str.length() == l_colours.size()) {
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


int main() {
	generateNcodeImg("testpassword");
	return 0;
}
