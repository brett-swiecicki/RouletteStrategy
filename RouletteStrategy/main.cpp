// Brett Swiecicki
// 6/24/2017
// Given gameplay parameters, program will determine the best possible betting strategy to maximize ROI while 
// minimizing risk.

//If we really wanted to fuck with this thing long term, then we would make it so that it maximizes the net EV (which is always going to be negative)
//by maximizing the win EV (which would make it so that you win a lot when you do win)

#include <iostream>
#include <string>
#include "OptProcessor.h"
using namespace std;

int main() {
	cout << "Optimal Roulette Strategy Finder." << endl;
	OptimalSolutionProcessor myProcessor;
	myProcessor.getInput();
	myProcessor.findSolution();
	myProcessor.queryForAdditionalTables();
}