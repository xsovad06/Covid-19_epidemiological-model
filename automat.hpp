#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <map>
#include <vector> 
#include <list> 
#include <math.h>
#include <time.h>

#define RUN_COUNT				200			/*	Number of simulations	*/

/*	Constant rates	*/
#define TRANSMISSION_R			2.95		/*	Chance that infected person infect susceptible one => 2.95%	*/
#define TRAVEL_R				20			/*	Chance that infected person will travel to other cells => %	*/
#define DEATH_R					3			/*	Chance that infected person will die  => 3%	*/			
#define INFECTED_DETECTION_R	60			/*	Rate that infected cell will be detected => 60%	*/
#define CURE_R					15			/*	Self cure chance => 15%	*/

/*	Disease constants	*/
#define INFECTIVENESS_BEGIN		5			/*	Incubation period	*/
#define INFECTIVENESS_LASTS		9			/*	Infectivness period 10 days(0-9)	*/
#define IMMUNITY_LASTS			90			/*	Immunity preriod 90 days	*/

/*	Represent width and lenght size of matrix consist of cells	*/
#define DIMENSION				450

/*	Represent posible cell states, can be seen in terminal after successful program run	*/
enum status { 
	susceptible, 							/*	susceptible => 0	*/
	infected, 								/*	infected 	=> 1	*/
	detected,								/*	detected	=> 2	*/
	immune, 								/*	immune 		=> 3	*/
	dead									/*	dead	 	=> 4	*/
};

/*	Class representing one person in the model	*/
class Cell {
	private:
		int row;							/*	Cell row position	*/
		int col;							/*	Cell column position	*/
		int status;							/*	Status of the cell{susceptible, infected, immune}	*/
		int days_infected;					/*	Number of days cell is infected	*/
		bool new_infected;					/*	Wheather the cell was infected in previous cycle	*/
		int days_immune;					/*	Number of days cell is immune	*/

	public:
		Cell();								/*	Cell constructor	*/

		void set_row(int r);				/*	Set row value to value from r	*/
		void set_col(int c);				/*	Set	column value to value from c	*/
		void set_status(int status);		/*	Set cell status	*/
		void increment_days_infected();		/*	Increment number of days cell is infected	*/
		void reset_days_infected();			/*	Set number of days infected to 0	*/
		void flip_new_infected();			/*	Flip value in new_infected to opose value	*/
		void increment_days_immune();		/*	Increment number of days cell is immune	*/
		void reset_days_immune();			/*	Set number of days immune to 0	*/

		int get_row();						/*	Return row value	*/
		int get_col();						/*	Return column value	*/
		int get_status(); 					/*	Return cell status	*/
		int get_days_infected();			/*	Return number of days cell is infected	*/
		bool get_new_infected();			/*	Return whether cell is newly infected	*/
		int get_days_immune();				/*	Return number of days cells is immune	*/
};

/*	Class representing whole simulation, store basic information about COVID-19 spreading	*/
class Simulation {
	private:
		int current_day;					/*	Actual day in the model	*/
		int max_day;						/*	Maximum day doing simulation	*/
		int initial_infected_cell_count;	/*	Number of infected cell randomly put to the matrix	*/
		int cell_travelling_count;			/*	Cell will travel up to "value" times per day	*/
		int travel_lenght_limit;			/*	Cell can travel in matrix "value" cells to each direction	*/
		Cell cell[DIMENSION][DIMENSION];	/*	Matrix with all cells	*/
		std::list<Cell> infection_queue;	/*	Queue contain infected cells	*/
		std::list<Cell> immune_queue;		/*	Queue contain immune cells	*/
		std::list<Cell> detected_queue;		/*	Queue contain detected cells	*/
		
		// Population Totals
		int susceptible_population;			/*	Count of susceptible cells	*/
		int infected_population;			/*	Count of infected cells		*/
		int detected_population;			/*	Count of detected cells		*/
		int immune_population;				/*	Count of immune cells	*/
		int dead_population;				/*	Count of deadt cells	*/

		//  Initialization
		void cells_init();					/*	Initialize cells positions in matrix	*/
		void simulation_init();				/*	Initialize simulation counters	*/

		// Infection 
		void random_cells_infects(int count);	/*	Function randomly infect number of cell given as argument	*/
		void spread_infection();				/*	Main spreading function, in iteration spread disease with infected cells 	*/
		void try_infect_at(int r, int c);		/*	Function try infect cell on given coordinates	*/
		void infect_cell(int r, int c);			/*	Function just infect cell "without" questions	*/
		std::pair<int, int> generate_trip_position(int c_x, int c_y);	/*	Function generate position from grid for cell trip	*/

		// Removing from infected lists
		void removing_type(int r, int c);		/*	Choose by death rate chance of death of cell, otherwise => immunity	*/
		void remove_cell(int r, int c, int new_status);	/*	Change status of cell based on argument	*/

		// Print function
		void print_simulation_info();			/*	Simply print out important information about simulation	*/
		void print_help_message();				/*	Print help message to user, when incorrect arguments are set	*/
		void print_to_file(std::string file_name, int status);	/*	Print statistic to file	based on status*/

	public:
		/*	Constructor and heart of whole simulation	*/
		Simulation(int argc, char **argv);
};