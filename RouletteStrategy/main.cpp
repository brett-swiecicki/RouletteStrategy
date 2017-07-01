// Brett Swiecicki
// 6/24/2017
// Given gameplay parameters, program will determine the best possible betting strategy to maximize ROI while 
// minimizing risk.

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <wchar.h>
#include "OptProcessor.h"
using namespace std;

int main() {
	cout << "Optimal Roulette Strategy Finder." << endl;
	OptimalSolutionProcessor myProcessor;
	myProcessor.getInput();
	myProcessor.displaytime();
	myProcessor.findSolution();
	myProcessor.printOutputTable();
	myProcessor.displaytime();
	system("Pause");
}