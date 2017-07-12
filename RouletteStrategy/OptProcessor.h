//Brett Swiecicki
#ifndef OPTPROCESSOR_H
#define OPTPROCESSOR_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <future>
#include <thread>
#include <mutex>
#include <time.h>
#include <unordered_map>

#include "OutputTable.h"
#include "ProcessorCommons.h"
#include "Simulator.h"

using namespace std;

class OptimalSolutionProcessor {
public:

	void getInput() {
		cout << "Enter the table minimum bet: ";
		cin >> osp_commons.min_bet;
		cout << "Enter the table maximum bet: ";
		cin >> osp_commons.max_bet;
		cout << "Enter the table minimum bet increment: ";
		cin >> osp_commons.min_increment;
		cout << "American or European? A or E: ";
		char AmOrEur;
		cin >> AmOrEur;
		if ((AmOrEur == 'A') || (AmOrEur == 'a')) {
			osp_commons.board_size = 38;
		}
		else if ((AmOrEur == 'E') || (AmOrEur == 'e')) {
			osp_commons.board_size = 37;
		}
		else {
			cerr << "Incorrect selection was made: " << AmOrEur << endl;
			exit(1);
		}
		cout << "Enter the payout factor [__ to 1]: ";
		cin >> osp_commons.payout_factor;
		cout << "Enter the number of winning table positions: ";
		cin >> osp_commons.board_hits;
		char modeChoice;
		cout << "Descending Win EV or Maximum Win EV Sum? D or S: ";
		cin >> modeChoice;
		if ((modeChoice == 'D') || (modeChoice == 'd') || (modeChoice == '1')) {
			osp_commons.descendingWinEV = true;
		}
		else if ((modeChoice == 'S') || (modeChoice == 's') || (modeChoice == '0')) {
			osp_commons.descendingWinEV = false;
			cout << "	Are break even bets acceptable? Y or N: ";
			char breakEven;
			cin >> breakEven;
			if ((breakEven == 'Y') || (breakEven == 'y') || (breakEven == '1')) {
				osp_commons.allowBreakEven = true;
			}
			else if ((breakEven == 'N') || (breakEven == 'n') || (breakEven == '0')) {
				osp_commons.allowBreakEven = false;
			}
			else {
				cerr << "Incorrect selection was made: " << breakEven << endl;
				exit(1);
			}
			char upperChoice;
			cout << "	Would you like to compute the solution with the upper bound optimization? Y or N: ";
			cin >> upperChoice;
			if ((upperChoice == 'Y') || (upperChoice == 'y') || (upperChoice == '1')) {
				osp_commons.useUpperBound = true;
			}
			else if ((upperChoice == 'N') || (upperChoice == 'n') || (upperChoice == '0')) {
				osp_commons.useUpperBound = false;
			}
			else {
				cerr << "Incorrect selection was made: " << upperChoice << endl;
				exit(1);
			}
			char fixChoice;
			cout << "	Would you like to compute the solution with the early bets fixed at the minimum? Y or N: ";
			cin >> fixChoice;
			if ((fixChoice == 'Y') || (fixChoice == 'y') || (fixChoice == '1')) {
				osp_commons.fixStart = true;
			}
			else if ((fixChoice == 'N') || (fixChoice == 'n') || (fixChoice == '0')) {
				osp_commons.fixStart = false;
			}
			else {
				cerr << "Incorrect selection was made: " << fixChoice << endl;
				exit(1);
			}
		}
		else {
			cerr << "Incorrect selection was made: " << modeChoice << endl;
			exit(1);
		}
		char lowerBoundChoice;
		cout << "	Use lower bound to approximate number of rolls? Y or N: ";
		cin >> lowerBoundChoice;
		if ((lowerBoundChoice == 'Y') || (lowerBoundChoice == 'y') || (lowerBoundChoice == '1')) {
			osp_commons.useLowerBound = true;
		}
		else if ((lowerBoundChoice == 'N') || (lowerBoundChoice == 'n') || (lowerBoundChoice == '0')) {
			osp_commons.useLowerBound = false;
		}
		else {
			cerr << "Incorrect selection was made: " << lowerBoundChoice << endl;
			exit(1);
		}

		/*
		cout << "std::thread::hardware_concurrency() has determined that your system has access to ";
		cout << std::thread::hardware_concurrency() << " threads." << endl;
		cout << "Do you believe this number is accurate? Y or N: ";
		char useHardwareConcurrency;
		cin >> useHardwareConcurrency;
		if ((useHardwareConcurrency == 'Y') || (useHardwareConcurrency == 'y') || (useHardwareConcurrency == '1')) {
			num_threads = std::thread::hardware_concurrency();
		}
		else if ((useHardwareConcurrency == 'N') || (useHardwareConcurrency == 'n') || (useHardwareConcurrency == '0')) {
			cout << "Please enter the number of threads you would like the program to utilize: ";
			cin >> num_threads;
		}
		else {
			cerr << "Incorrect selection was made: " << useHardwareConcurrency << endl;
			exit(1);
		}
		*/
		cout << endl;
	} //End of getInput

	void findSolution() {
		clock_t t; //Start Clock!
		t = clock();
		osp_commons.setupPossibleBets();
		if (osp_commons.descendingWinEV == false) {
			findMaxWinEVSum();
		}
		else {
			findDescendingWinEVSolution();
		}
		t = clock() - t; //Print running time!
		cout << "Total running time: " << (((float)t) / (CLOCKS_PER_SEC)) << " seconds." << endl;
		osp_commons.printTable((((int)osp_commons.all_solutions.size()) - 1));
	}

	void queryForAdditionalTasks() {
		osp_commons.current_table = ((int)osp_commons.all_solutions.size() - 1);
		char taskMode;
		cout << "Please select what you would like to do next: " << endl;
		cout << "	1: See the output table for a different number of rolls [DATA MAY NOT BE OPTIMAL!] " << endl;
		cout <<	"	2: Run simulations on this solution. " << endl;
		cout << "	3: Supplement this solution with an optimal solution from another table. " << endl;
		cout << "	4: Compute a different optimal strategy." << endl;
		cout << "	E: Exit optimal strategy finder." << endl;
		cin >> taskMode;
		if (taskMode == '1') {
			queryForAdditionalTables();
			queryForAdditionalTasks();
		}
		else if (taskMode == '2') {
			Simulator mySimulator = Simulator(osp_commons.all_solutions[osp_commons.current_table], 
				(int)osp_commons.all_solutions.back().size(), osp_commons.board_size, osp_commons.board_hits, osp_commons.payout_factor);
			mySimulator.runSimulations();
			mySimulator.query_for_additional_simulations();
			queryForAdditionalTasks();
		}
		else if (taskMode == '4') {
			osp_commons.clearPrivateData();
			getInput();
			findSolution();
			queryForAdditionalTasks();
		}
		else if ((taskMode == 'E') || (taskMode == 'e')){
			return;
		}
		else {
			cout << "Sorry! That mode is not available at this time." << endl;
			queryForAdditionalTasks();
		}
	}

	void queryForAdditionalTables() {
		int smallest_roll_count = (int)osp_commons.all_solutions.front().size();
		int largest_roll_count = (int)osp_commons.all_solutions.back().size();
		char printMore = 'Y';
		while ((printMore != 'N') && (printMore != 'n') && (printMore != '0')) {
			cout << "Enter the number of rolls for which you would like to see a solution (" << smallest_roll_count << " through " << largest_roll_count << "): ";
			int desiredSolution;
			cin >> desiredSolution;
			if ((desiredSolution >= smallest_roll_count) && (desiredSolution <= largest_roll_count)) {
				int actual_index = (((int)osp_commons.all_solutions.size()) - (largest_roll_count - desiredSolution) - 1);
				osp_commons.printTable(actual_index);
				osp_commons.current_table = actual_index;
			}
			else {
				cout << "Sorry! " << desiredSolution << " does not have a computed solution!" << endl;
			}
			cout << "Would you like to see the output for another solution? Y or N: ";
			cin >> printMore;
		}
	}

private:
	ProcessorCommons osp_commons;

	void findMaxWinEVSum() {
		if (osp_commons.useLowerBound) {
			osp_commons.total_rolls = osp_commons.getLowestBoundRolls(); //Linear incrementation of total_rolls
		}
		else {
			osp_commons.total_rolls = 1;
		}
		if (osp_commons.getLowestBoundRolls() == 1) {
			osp_commons.best_stakes.resize(osp_commons.total_rolls);
			osp_commons.best_stakes[0] = osp_commons.max_bet;
			osp_commons.all_solutions.push_back(osp_commons.best_stakes);
			osp_commons.printTable(0);
			return;
		}
		if (osp_commons.useUpperBound) {
			osp_commons.constructUpperBounds();
		}
		else {
			//Set the upper bound vector to all max
			osp_commons.upper_bound_bets.resize(osp_commons.total_rolls, (((int)osp_commons.possible_bets.size()) - 1));
		}
		bool limit_reached = false;
		osp_commons.mode = "Phase1";
		while (limit_reached == false) {
			osp_commons.prepDynamicSolution();
			osp_commons.solutionUpdated = false;
			cout << "Currently computing strategies for " << osp_commons.total_rolls << " rolls.";
			if (osp_commons.allowBreakEven) {
				solutionFindRecBreakEven(osp_commons.starting_stake, osp_commons.starting_cumulative, 0);
			}
			else {
				solutionFindRecNoBreakEven(osp_commons.starting_stake, osp_commons.starting_cumulative, 0);
			}
			++osp_commons.total_rolls;
			if (osp_commons.solutionUpdated == false) {
				if (osp_commons.mode == "Phase2") {
					limit_reached = true;
				}
				cout << endl;
			}
			else {
				osp_commons.all_solutions.push_back(osp_commons.best_stakes);
				osp_commons.appendUpperBounds();
				if (osp_commons.mode == "Phase1") {
					osp_commons.mode = "Phase2";
				}
				cout << " Solution found!" << endl;
			}
		}
	}

	void findDescendingWinEVSolution() {
		if (osp_commons.useLowerBound) {
			osp_commons.total_rolls = (osp_commons.getLowestBoundRolls() - osp_commons.payout_factor); //Really not a lowestBound in this case, just an approximation
		}
		else {
			osp_commons.total_rolls = 1;
		}
		osp_commons.p_win_single = ((double)osp_commons.board_hits / (double)osp_commons.board_size);
		osp_commons.construct_p_win_exacts();
		bool limit_reached = false;
		bool decreasing = false;
		while (limit_reached == false) {
			osp_commons.solutionUpdated = false;
			cout << "Currently computing strategies for " << osp_commons.total_rolls << " rolls." << endl;
			osp_commons.dynamic_solution.resize(osp_commons.total_rolls);
			osp_commons.dynamic_EV_solution.resize(osp_commons.total_rolls);
			solutionFindDescendingWinEV(0, 0.0, 0);
			if (osp_commons.solutionUpdated == false) {
				if (osp_commons.total_rolls == 2) { //If there can only be one bet then it must be table max
					osp_commons.best_stakes.resize(1);
					osp_commons.best_stakes[0] = osp_commons.possible_bets[osp_commons.possible_bets.size() - 1];
					osp_commons.all_solutions.push_back(osp_commons.best_stakes);
				}
				if (osp_commons.all_solutions.empty()) {
					--osp_commons.total_rolls;
					decreasing = true;
				}
				else {
					limit_reached = true;
				}
			}
			else {
				osp_commons.all_solutions.push_back(osp_commons.best_stakes);
				osp_commons.append_p_win_exacts();
				++osp_commons.total_rolls;
				if (decreasing) {
					limit_reached = true;
				}
			}
		}
	}

	void solutionFindDescendingWinEV(int stake_number, double cumulative_stake, int lastAddedBet) {
		//New invariant: bets at the end can be break even, but others can't
		if (stake_number == (osp_commons.total_rolls - 1)) {
			//First, need to fix the last bet as the table max:
			osp_commons.dynamic_solution[stake_number] = osp_commons.max_bet;
			osp_commons.dynamic_EV_solution[stake_number] = osp_commons.singleRollEV(stake_number, cumulative_stake, osp_commons.max_bet);
			if (osp_commons.dynamic_EV_solution[stake_number] >= 0.0) {
				if (osp_commons.dynamic_solution.size() > osp_commons.best_stakes.size()) { //More rolls -> better solution
					osp_commons.best_stakes = osp_commons.dynamic_solution;
					osp_commons.best_stakes_EV = osp_commons.dynamic_EV_solution;
					osp_commons.solutionUpdated = true;
				}
				else { //Same number of rolls -> //Check EV one by one
					for (int i = 0; i < (int)osp_commons.dynamic_solution.size(); ++i) {
						if (osp_commons.best_stakes_EV[i] < osp_commons.dynamic_EV_solution[i]) {
							osp_commons.best_stakes = osp_commons.dynamic_solution;
							osp_commons.best_stakes_EV = osp_commons.dynamic_EV_solution;
							osp_commons.solutionUpdated = true;
							break;
						}
					}
				}
			}
			return;
		}
		for (int i = lastAddedBet; i < (int)osp_commons.possible_bets.size(); ++i) {
			osp_commons.dynamic_solution[stake_number] = osp_commons.possible_bets[i];
			osp_commons.dynamic_EV_solution[stake_number] = osp_commons.singleRollEV(stake_number, cumulative_stake, osp_commons.possible_bets[i]);
			if ((stake_number == 0) || ((osp_commons.dynamic_EV_solution[stake_number] <= osp_commons.dynamic_EV_solution[stake_number - 1]) && 
				(osp_commons.dynamic_EV_solution[stake_number] >= 0.0))) {
				solutionFindDescendingWinEV(stake_number + 1, cumulative_stake + osp_commons.possible_bets[i], i);
			}
			else if (osp_commons.dynamic_EV_solution[stake_number] > osp_commons.dynamic_EV_solution[stake_number - 1]) {
				break; //This makes it so EV is descending
			}
		}
	}

	void solutionFindRecBreakEven(int stake_number, double cumulative_stake, int lastBetAdded) {
		if (stake_number == (osp_commons.total_rolls - 1)) {
			osp_commons.dynamic_solution[stake_number] = osp_commons.max_bet;
			cumulative_stake += osp_commons.max_bet;
			bool profitable = osp_commons.checkIfProfitableBreakEven(osp_commons.dynamic_solution, stake_number, cumulative_stake);
			if (profitable) {
				double dynamic_win_EV_sum = osp_commons.getWinEV(osp_commons.dynamic_solution);
				if ((osp_commons.dynamic_solution.size() > osp_commons.best_stakes.size()) ||
					(((osp_commons.dynamic_solution.size() == osp_commons.best_stakes.size()) && (dynamic_win_EV_sum > osp_commons.best_win_EV_sum)))) {
					osp_commons.best_stakes = osp_commons.dynamic_solution;
					osp_commons.best_win_EV_sum = dynamic_win_EV_sum;
					osp_commons.solutionUpdated = true;
				}
			}
			return;
		}

		for (int i = lastBetAdded; i <= osp_commons.upper_bound_bets[stake_number]; ++i) {
			osp_commons.dynamic_solution[stake_number] = osp_commons.possible_bets[i];
			bool profitable = osp_commons.checkIfProfitableBreakEven(osp_commons.dynamic_solution, stake_number, cumulative_stake + osp_commons.possible_bets[i]);
			if (profitable) {
				solutionFindRecBreakEven(stake_number + 1, cumulative_stake + osp_commons.possible_bets[i], i);
			}
		}
	}

	void solutionFindRecNoBreakEven(int stake_number, double cumulative_stake, int lastBetAdded) {
		if (stake_number == osp_commons.total_rolls) {
			double dynamic_win_EV_sum = osp_commons.getWinEV(osp_commons.dynamic_solution);
			if ((osp_commons.dynamic_solution.size() > osp_commons.best_stakes.size()) ||
				(((osp_commons.dynamic_solution.size() == osp_commons.best_stakes.size()) && (dynamic_win_EV_sum > osp_commons.best_win_EV_sum)))) {
				osp_commons.best_stakes = osp_commons.dynamic_solution;
				osp_commons.best_win_EV_sum = dynamic_win_EV_sum;
				osp_commons.solutionUpdated = true;
			}
			return;
		}
		for (int i = lastBetAdded; i <= osp_commons.upper_bound_bets[stake_number]; ++i) {
			osp_commons.dynamic_solution[stake_number] = osp_commons.possible_bets[i];
			bool profitable = osp_commons.checkIfProfitableNoBreakEven(osp_commons.dynamic_solution, stake_number, cumulative_stake + osp_commons.possible_bets[i]);
			if (profitable) {
				solutionFindRecNoBreakEven(stake_number + 1, cumulative_stake + osp_commons.possible_bets[i], i);
			}
		}
	}

};

#endif