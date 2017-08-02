// Brett Swiecicki
// Project Started: 6/24/2017

#include <iostream>
#include <string>
#include "OptController.h"
#include "PlaythroughSimulator.h"
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
			OptimalSolutionController myProcessor;
			myProcessor.getInput();
			myProcessor.findSolution();
			myProcessor.queryForAdditionalTasks();
		}
		else if (mode == '2') {
			Simulator mySimulator(true);
			mySimulator.runSimulations();
			mySimulator.query_for_additional_simulations();
		}
		else if (mode == '3') {
			PlaythroughSimulator myPlaythroughSim(true);
			myPlaythroughSim.runSimulations();
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
	cout << "	3: Run simulations on an existing strategy with playthrough requirements for bonus." << endl;
	cout << "	4: Supplement an existing strategy with a new table strategy." << endl;
	cout <<	"	Q: Quit application." << endl;
}