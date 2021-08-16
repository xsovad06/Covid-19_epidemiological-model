#include "automat.hpp"

using namespace std;

int main(int argc, char **argv) {
	Simulation sim(argc, argv);
	return 0;
}

/****************************************************************************/
/********************	Function declatation of class Cell	*****************/
/****************************************************************************/

Cell::Cell() {
	this->status = susceptible;
	this->days_infected = 0;
	this->new_infected = false;
	this->days_immune = 0;
}

void Cell::set_row(int r) {
	this->row = r;
}

void Cell::set_col(int c) {
	this->col = c;
}

void Cell::set_status(int status) {
	this->status = status;
}

void Cell::increment_days_infected() {
	this->days_infected++;
}

void Cell::reset_days_infected() {
	this->days_infected = 0;
}

void Cell::flip_new_infected() {
	this->new_infected = !this->new_infected;
}

void Cell::increment_days_immune() {
	this->days_immune++;
}

void Cell::reset_days_immune() {
	this->days_immune = 0;
}

int Cell::get_row() {
	return this->row;
}

int Cell::get_col() {
	return this->col;
}

int Cell::get_status() {
	return this->status;
}

int Cell::get_days_infected() {
	return this->days_infected;
}

bool Cell::get_new_infected() {
	return this->new_infected;
}

int Cell::get_days_immune() {
	return this->days_immune;
}

/****************************************************************************/
/***************	Function declatation of class Simulation	*************/
/****************************************************************************/

Simulation::Simulation(int argc, char **argv) {
	if (argc == 4) {
		this->max_day = atoi(argv[1]);						/*	Max days of simulation	*/
		this->initial_infected_cell_count = atoi(argv[2]);	/*	Initial number of infected cells	*/
		this->cell_travelling_count = atoi(argv[3]);		/*		*/
		this->travel_lenght_limit = 1;						/*	Not set 4th argument, value set to 1(no trips, only neighborhood)	*/
	}
	else if (argc == 5) {
		this->max_day = atoi(argv[1]);
		this->initial_infected_cell_count = atoi(argv[2]);
		this->cell_travelling_count = atoi(argv[3]);
		this->travel_lenght_limit = atoi(argv[4]);
	}
	else {
		cerr << "Incorrect  program arguments." << endl;
		print_help_message();
		exit(EXIT_FAILURE);
	}
	
	srand(time(NULL));
	simulation_init();

	// Initialize cells cordinates
	cells_init();
	// Randomly infect set number of cells
	random_cells_infects(this->initial_infected_cell_count);

	// Spreadind COVID-19 till maximal day or till infected cell occurs in the grid
	while(this->current_day < this->max_day && this->infection_queue.size() > 0) {
		// Main spreading function
		spread_infection();
		this->current_day++;
	}
	// Print simulation info to std output
	print_simulation_info();
}

void Simulation::cells_init() {
	for (int i = 0; i < DIMENSION; i++)
	{
		for (int j = 0; j < DIMENSION; j++)
		{
			this->cell[i][j].set_row(i);
			this->cell[i][j].set_col(j);
		}
	}
}

void Simulation::simulation_init() {
	this->susceptible_population = DIMENSION * DIMENSION;
	this->current_day = 0;
	this->infected_population = 0;
	this->detected_population = 0;
	this->immune_population = 0;
	this->dead_population = 0;
}

void Simulation::random_cells_infects(int count) {
	for (int i = 0; i < count; i++)
	{
		int r = rand() % DIMENSION;
		int c = rand() % DIMENSION;
		
		// Infect concrete cell
		infect_cell(r,c);
	}
}

void Simulation::infect_cell(int r, int c) {
	this->cell[r][c].set_status(infected);
	this->infection_queue.push_back(this->cell[r][c]);
	this->infected_population++;
	this->susceptible_population--;
}

void Simulation::try_infect_at(int r, int c) {
	// Try to infect only susceptible cells
	if (this->cell[r][c].get_status() == susceptible) {
		int chance = rand() % 100;
		
		// Should cell get infected?
		if (chance < TRANSMISSION_R) {
			this->cell[r][c].flip_new_infected();
			infect_cell(r,c);
		}
	}
}

pair<int, int> Simulation::generate_trip_position(int c_x, int c_y) {
	int x;
	int y;
	// Do while positions are out of range or are the same as current cell
	do {
		// Generate x position from grid
		x = rand() % (this->travel_lenght_limit * 2 +1) + (c_x - this->travel_lenght_limit);
		// Generate y position from grid
		y = rand() % (this->travel_lenght_limit * 2 +1) + (c_y - this->travel_lenght_limit);
	} while ((x < 0 || DIMENSION <= x) || (y < 0 || DIMENSION <= y) || (x == c_x && y == c_y));
	return {x, y};
}

void Simulation::spread_infection() {
	int chance = 0;
	// Loop through all cells in the infection queue
	for (list<Cell>::iterator cell = this->infection_queue.begin(); cell != this->infection_queue.end(); ++cell)
	{
		// If the cell is newly infected, I will not attempt infecting others
		if((*cell).get_new_infected()) {
			// Set this cell to be NOT newly infected so that the next day this cell will be able to attempt infecting others
			(*cell).flip_new_infected();
		}
		// 3 days after infection can be cell detected and start cured(put into quarantine)
		else if ( 5 <= (*cell).get_days_infected() && (*cell).get_status() != detected) {
			// Detection chance
			chance = rand() % 100;
			if (chance < INFECTED_DETECTION_R) {
				remove_cell((*cell).get_row(), (*cell).get_col(), detected);

				// Remove this cell from the queue
				this->infection_queue.erase(cell);				
			}
		}
		// Should this infected cell enter the removed state?
		else if ((*cell).get_days_infected() > INFECTIVENESS_LASTS) {
			// This infected cell should enter the removed state.
			removing_type((*cell).get_row(),(*cell).get_col());
			
			// Remove this cell from the queue
			this->infection_queue.erase(cell);
		}
		// cell is ready to attempt infecting other entities!
		else {
			int current_x = (*cell).get_col();
			int current_y = (*cell).get_row();
			// start attempting to infect, Moore neighborhood
			for (int x = current_x -1; x <= current_x + 1; ++x)
			{
				for (int y = current_y - 1; y <= current_y + 1 ; ++y)
				{
					// Out of grid
					if (x < 0 || y < 0 || x == DIMENSION || y == DIMENSION) {
						continue;
					}
					// Current cell
					else if (x == current_x && y == current_y) {
						continue;
					}
					// Try infect cell in Moore neighborhood
					else {
						try_infect_at(x,y);
					} 
				}
			}
			
			// Traveling per day based on traveling rate
			if (this->travel_lenght_limit != 0) {
				chance = rand() % 100;
				pair<int,int> position = generate_trip_position(current_x, current_y);
				if (chance < TRAVEL_R) {
					// Cell will make number of trips randomly generate in range <0,6> + "cell_travelling_count"
					int traveling_count = rand() % 6 + (this->cell_travelling_count - 3);
					for (int i = 0; i < traveling_count; i++)
					{
						// Executing this trips randomly without limitation on location
						try_infect_at(position.first, position.second);
					}
				}
			}

			// Increment this cell days infected since cell is an actively infecting cell
			(*cell).increment_days_infected();
		}
	}

	// Loop through all cells in the detected queue
	for (list<Cell>::iterator cell = this->detected_queue.begin(); cell != this->detected_queue.end(); ++cell)
	{
		// After detection cell can be cured even before infectivness ends
		if ((*cell).get_days_infected() < INFECTIVENESS_LASTS) {
			chance = rand() % 100;
			
			// Cell is cured, put cell to immune queue, increment immune population, remove from detected queue
			if (chance < CURE_R) {
				(*cell).set_status(immune);
				this->immune_queue.push_back((*cell));
				this->immune_population++;

				// Remove this cell from the queue, decrement detected population
				this->detected_queue.erase(cell);
				this->detected_population--;
			}
			// else increase number of days cell is infected
			(*cell).increment_days_infected();
		}
		// After infectivness ends, chance of death, otherwise cell change state to immune
		else {
			chance = rand() % 100;

			if (chance < DEATH_R) {
				(*cell).set_status(dead);
				this->dead_population++;
			}  
			else {
				(*cell).set_status(immune);
				(*cell).reset_days_infected();
				this->immune_queue.push_back((*cell));
				this->immune_population++;
			};

			// Remove this cell from the queue
			this->detected_queue.erase(cell);
			this->detected_population--;
		}
	}

	// Loop through all cells in the immune queue
	for (list<Cell>::iterator cell = this->immune_queue.begin(); cell != this->immune_queue.end(); ++cell)
	{
		// Switch cell state from immune to susceptible when immunity expired, reset immune days counter, remove from queue
		if ((*cell).get_days_immune() >= IMMUNITY_LASTS) {
			(*cell).set_status(susceptible);
			(*cell).reset_days_immune();
			this->immune_queue.erase(cell);
		}
		// Cell is in detected state, but still counting as infected one
		(*cell).increment_days_immune();
	}
}

void Simulation::removing_type(int r, int c) {
	int chance = rand() % 100;
	chance < DEATH_R ? remove_cell(r,c,dead) : remove_cell(r,c,immune);
}

void Simulation::remove_cell(int r, int c, int new_status) {
	this->cell[r][c].set_status(new_status);
	// Based on argument change cell status
	switch(new_status) {
		case susceptible:
			this->susceptible_population++;
			break;
		case detected:
			this->detected_queue.push_back(this->cell[r][c]);
			this->detected_population++;
			break;
		case immune:
			this->cell[r][c].reset_days_infected();
			this->immune_queue.push_back(this->cell[r][c]);
			this->immune_population++;
			break;
		case dead:
			this->dead_population++;
			break;
	}
	this->infected_population--;
}

void Simulation::print_simulation_info() {
	cout << left << "\n+----------------------------------------------+" << endl;
	cout << "| " << setw(15) << " " << "Simulation Data" << setw(14) << " " << " |" << endl;
	cout << setw(40) << "| Start Infected Cells: " << setw(6) << right << this->initial_infected_cell_count << " |" << endl;
	cout << setw(40) << left << "| Trips per day: " << setw(6) << right << this->cell_travelling_count << " |" << endl;
	cout << setw(40) << left << "| Trips max distance: " << setw(6) << right << this->travel_lenght_limit << " |" << endl;
	cout << setw(40) << left << "| Current Day: " << setw(6) << right << this->current_day << " |" << endl;
	cout << setw(40) << left << "| Max Day: " << setw(6) << right << this->max_day << " |" << endl;
	cout << setw(40) << left << "| Susceptible Population: " << setw(6) << right << this->susceptible_population << " |" << endl;
	cout << setw(40) << left << "| Infected Population: " << setw(6) << right << this->infected_population << " |" << endl;
	cout << setw(40) << left << "| Detected Population: " << setw(6) << right << this->detected_population << " |" << endl;
	cout << setw(40) << left << "| Immune Population: " << setw(6) << right << this->immune_population << " |" << endl;
	cout << setw(40) << left << "| Deceased Population: " << setw(6) << right << this->dead_population << " |" << endl;
	cout << "+----------------------------------------------+" << endl;
	cout << endl;
}

void Simulation::print_help_message() {
	cout <<
	"				COVID-19 SIMULATOR\n\n"
	"		Program use:\n"
	"		./automat <MAX_DAYS> <START_INFECTED_COUNT> <TRIPS_PER_DAY> <TRIP_DISTANCE>\n"
	"		<MAX_DAYS>				Maximum days of simulation run \n"
	"		<START_INFECTED_COUNT>			Initial number of infected\n"
	"		<TRIPS_PER_DAY>				Trips per day\n"
	"		<TRIP_DISTANCE>				Maximal trip distance\n";
}

void Simulation::print_to_file(std::string file_name, int status) {
	std::ofstream out_file;
	out_file.open(file_name, std::ios_base::app); // append mode
	switch (status)
	{
		case infected:
			out_file << this->infected_population << endl; 
			break;
		case detected:
			out_file << this->detected_population << endl; 
			break;
		case immune:
			out_file << this->immune_population << endl; 
			break;
		case dead:
			out_file << this->dead_population << endl; 
			break;
	}
}