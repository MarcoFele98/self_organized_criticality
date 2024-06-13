#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <list>


//*** parameters ****************************************

int dim = 500;
int threshold = 4;

int maxTime = 10000000;


//*** cell *********************************************

class Cell {
public:
	// constructor
	Cell(int n) { nGrains = n; isInvolved = false; };

	// functions
	void addGrain(const int, const int, std::vector< std::vector<Cell> >&, int&);
	void reset();

private:
	int nGrains;
	bool isInvolved;
};


//*** functions *****************************************

void Cell::addGrain(const int x, const int y, std::vector< std::vector<Cell> >& field, int& avalangeEntity)
{
	++nGrains;

	// redistribute grains (avalangeee)
	while (nGrains > threshold) {
		// calculate neigbours
		int x1 = x - 1;
		int x2 = x + 1;
		int y1 = y - 1;
		int y2 = y + 1;

		// update cell state
		nGrains -= 4;
		if (nGrains < 0) throw std::runtime_error("impossibly negative grain values");

		// add grain to neighbour if neighbour exists
		if (x1 > 0 && y1 > 0) field[x1][y1].addGrain(x1, y1, field, avalangeEntity);
		if (x2 < dim && y2 < dim) field[x2][y2].addGrain(x2, y2, field, avalangeEntity);
		if (x1 > 0 && y2 < dim) field[x1][y2].addGrain(x1, y2, field, avalangeEntity);
		if (x2 < dim && y1 > 0) field[x2][y1].addGrain(x2, y1, field, avalangeEntity);

		// increase the avalage entity only for spatial extension
		if (!isInvolved) {
			isInvolved = true;
			++avalangeEntity;
		}

		// increase the avalage entity each time the cell surpasses the limit
		//++avalangeEntity;
	}
}

void Cell::reset()
{
	isInvolved = false;
}


//*** main ***************************

int main() {
	try {
		// initialise field, avalange counter
		std::vector< std::vector<Cell> > field(dim, std::vector <Cell>(dim, Cell(0)));
		std::list< std::pair<int, int> > counter; // first -> avalange entity, second -> frequency

		// output directory
		std::ofstream ofs("sandPile_data.csv");
		if (!ofs.is_open())
			throw std::logic_error("unable to open output directory");
		ofs << "entity" << "," << "count" << "\n";

		// run simulation
		for (int time = 0; time < maxTime; ++time) {
			// screen output
			if (!(time % 1000))
				std::cout << time << "\n";

			// add grain and let the avalanges go
			int avalangeEntity = 0;

			int x = rand() % dim;
			int y = rand() % dim;

			field[y][x].addGrain(x, y, field, avalangeEntity);

			// store avalange entity if the avalange is not zero
			if (avalangeEntity) {
				std::list< std::pair<int, int> >::iterator it = counter.begin();
				for (; it != counter.end(); ++it) {
					if (it->first == avalangeEntity) {
						++(it->second);
						break;
					}
				}
				// add new value if it was not present
				if (it == counter.end()) {
					std::pair<int, int> newAvalange(avalangeEntity, 1);
					counter.push_back(newAvalange);
				}
			}

			// reset avalange
			for (int y = 0; y < dim; ++y)
				for (int x = 0; x < dim; ++x) {
					field[y][x].reset();
				}
			avalangeEntity = 0;
		}

		// save output
		for (std::list< std::pair<int, int> >::iterator it = counter.begin(); it != counter.end(); ++it)
			ofs << it->first << "," << it->second << std::endl;
	}
	catch (std::exception& error) {
		std::cerr << "Error: " << error.what() << "\n";
		exit(1);
	}
	catch (...) {
		std::cerr << "Error: unknown error accured.\n";
		exit(1);
	}

	return 0;
}