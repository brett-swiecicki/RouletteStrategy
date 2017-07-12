//Brett Swiecicki
#ifndef OPTCONTROLLER_H
#define OPTCONTROLLER_H

#include <iostream>
#include <time.h>
#include <thread>

#include "OptDescendingProcessor.h"
#include "OptimalSimProcessor.h"
#include "OptSumProcessor.h"
#include "OutputTable.h"
#include "ProcessorCommons.h"
#include "Simulator.h"

using namespace std;

class InputRetriever {
public:
	InputRetriever(ProcessorCommons& osp_commons_in, char& processing_mode_in)
		: osp_commons(osp_commons_in), processing_mode(processing_mode_in) {}

	void get_common_table_data() {
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
	}

	void get_processing_mode() {
		cout << "Enter the processing mode (D: Descending Win EV, M: Maximum Win EV Sum, S: Simulator Optimal ROI): ";
		cin >> processing_mode;
	}

	void get_max_specific_data() {
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
	}

	void get_sim_specific_data() {
		cout << "std::thread::hardware_concurrency() has determined that your system has access to ";
		cout << std::thread::hardware_concurrency() << " threads." << endl;
		cout << "Do you believe this number is accurate? Y or N: ";
		char useHardwareConcurrency;
		cin >> useHardwareConcurrency;
		if ((useHardwareConcurrency == 'Y') || (useHardwareConcurrency == 'y') || (useHardwareConcurrency == '1')) {
			osp_commons.num_threads = std::thread::hardware_concurrency();
		}
		else if ((useHardwareConcurrency == 'N') || (useHardwareConcurrency == 'n') || (useHardwareConcurrency == '0')) {
			cout << "	Please enter the number of threads you would like the program to utilize: ";
			cin >> osp_commons.num_threads;
		}
		else {
			cerr << "Incorrect selection was made: " << useHardwareConcurrency << endl;
			exit(1);
		}
	}

	void get_descending_specific_data() {
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
	}

private:
	ProcessorCommons& osp_commons;
	char& processing_mode;

};

class OptimalSolutionController {
public:

	void getInput() {
		InputRetriever my_input_retriever(osp_commons, processing_mode);
		my_input_retriever.get_common_table_data();
		my_input_retriever.get_processing_mode();
		if ((processing_mode == 'M') || (processing_mode == 'm')) {
			my_input_retriever.get_max_specific_data();
		}
		else if ((processing_mode == 'D') || (processing_mode == 'd')) {
			my_input_retriever.get_descending_specific_data();
		}
		else if ((processing_mode == 'S') || (processing_mode == 's')) {
			my_input_retriever.get_sim_specific_data();
		}
		else {
			cout << "Sorry! That processing mode does not exist." << endl;
			return;
		}
		cout << endl;
	} //End of getInput

	void findSolution() {
		clock_t t; //Start Clock!
		t = clock();
		osp_commons.setupPossibleBets();
		if ((processing_mode == 'M') || (processing_mode == 'm')) {
			OptimalSumProcessor my_processor(osp_commons);
			my_processor.findMaxWinEVSum();
		}
		else if ((processing_mode == 'D') || (processing_mode == 'd')) {
			OptimalDescendingProcessor my_processor(osp_commons);
			my_processor.findDescendingWinEVSolution();
		}
		else {
			OptimalSimulatorProcessor my_processor(osp_commons);
			my_processor.find_max_ROI_solution_with_simulator();
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

private:
	ProcessorCommons osp_commons; //aka "Optimal Solution Processor common data and functions"
	char processing_mode;

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
};

#endif