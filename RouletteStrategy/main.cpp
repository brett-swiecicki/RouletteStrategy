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

int main() {
	cout << "The Swiecicki Roulette Strategy Program." << endl;
	cout << "Please select a mode: " << endl;
	cout << "1. Compute a new optimal strategy." << endl;
	cout << "2. Run simulations on an existing strategy." << endl;
	int mode;
	cin >> mode;
	if (mode == 1) {
		OptimalSolutionProcessor myProcessor;
		myProcessor.getInput();
		myProcessor.findSolution();
		myProcessor.queryForAdditionalTasks();
	}
	else if (mode == 2) {
		Simulator mySimulator;
		mySimulator.runSimulations();
		mySimulator.query_for_additional_simulations();
	}
	else {
		cerr << "Incorrect selection was made: " << mode << endl;
		exit(1);
	}
}