// Brett Swiecicki
// 6/24/2017
// Given gameplay parameters, program will determine the best possible betting strategy to maximize ROI while 
// minimizing risk.

//If we really wanted to fuck with this thing long term, then we would make it so that it maximizes the net EV (which is always going to be negative)
//by maximizing the win EV (which would make it so that you win a lot when you do win)

#include <iostream>
#include <string>
#include "OptProcessor.h"
#include "Simulator.h"
using namespace std;

void printModes();

int main() {
	cout << "The Swiecicki Roulette Strategy Program." << endl;
	printModes();
	char mode;
	cin >> mode;
	cout << endl;
	while ((mode != 'Q') && (mode != 'q')) {
		if (mode == '1') {
			OptimalSolutionProcessor myProcessor;
			myProcessor.getInput();
			myProcessor.findSolution();
			myProcessor.queryForAdditionalTasks();
		}
		else if (mode == '2') {
			Simulator mySimulator;
			mySimulator.runSimulations();
			mySimulator.query_for_additional_simulations();
		}
		else {
			cout << "Sorry! That mode is not available at this time." << endl;
		}
		cout << endl;
		printModes();
		cin >> mode;
		cout << endl;
	}//End of while loop
}

void printModes() {
	cout << "Please select a mode: " << endl;
	cout << "	1: Compute a new optimal strategy." << endl;
	cout << "	2: Run simulations on an existing strategy." << endl;
	cout << "	3: Supplement an existing strategy with a new table strategy." << endl;
	cout <<	"	Q: Quit application." << endl;
}