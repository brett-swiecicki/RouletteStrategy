//Brett Swiecicki
#ifndef OPTPROCESSOR_H
#define OPTPROCESSOR_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <wchar.h>

#include "ThreadPool.h"
#include "Solution.h"
#include "StakeFinder.h"
#include "SharedParameters.h"
using namespace std;

class OptimalSolutionProcessor {
public:

	void displaytime() {
		//display current time - possibly convert to clock time and not military
		SYSTEMTIME time;
		GetLocalTime(&time);
		wprintf(L"The Local Time: %02d:%02d:%02d\n", time.wHour, time.wMinute, time.wSecond);
		cout << endl;
	}

	void getInput() {
		cout << "Enter the table minimum bet: ";
		cin >> local_parameters.min_bet;
		cout << "Enter the table maximum bet: ";
		cin >> local_parameters.max_bet;
		cout << "Enter the table minimum bet increment: ";
		cin >> min_increment;
		cout << "American or European? A or E: ";
		char AmOrEur;
		cin >> AmOrEur;
		if ((AmOrEur == 'A') || (AmOrEur == 'a')) {
			local_parameters.board_size = 38;
		}
		else if ((AmOrEur == 'E') || (AmOrEur == 'e')) {
			local_parameters.board_size = 37;
		}
		else {
			cerr << "Incorrect selection was made: " << AmOrEur << endl;
			exit(1);
		}
		cout << "Enter the payout factor [__ to 1]: ";
		cin >> local_parameters.payout_factor;
		cout << "Enter the number of winning table positions: ";
		cin >> local_parameters.board_hits;
		cout << "Are break even bets acceptable? Y or N: ";
		char breakEven;
		cin >> breakEven;
		if ((breakEven == 'Y') || (breakEven == 'y') || (breakEven == '1')) {
			local_parameters.allowBreakEven = true;
		}
		else if ((breakEven == 'N') || (breakEven == 'n') || (breakEven == '0')) {
			local_parameters.allowBreakEven = false;
		}
		else {
			cerr << "Incorrect selection was made: " << breakEven << endl;
			exit(1);
		}
		cout << "std::thread::hardware_concurrency() has determined that your system has access to ";
		cout << std::thread::hardware_concurrency() << " threads." << endl;
		cout << "Is this number of threads acceptable? Y or N: ";
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
		cout << endl;
	} //End of getInput

	void findSolution() {
		setupPossibleBets();
		local_parameters.total_rolls = getLowestBoundRolls(); //Linear incrementation of total_rolls

		if (local_parameters.total_rolls == 1) {
			dynamic_solution_start.push_back(local_parameters.max_bet);
			optimalSolution.change_best_stakes(dynamic_solution_start);
			return;
		}
		
		buildTasksVector();
		thread_pool processing_pool(num_threads);
		bool limit_reached = false;
		while (limit_reached == false) {
			prepDynamicSolution();
			optimalSolution.resetUpdated();
			cout << "Currently computing strategies for " << local_parameters.total_rolls << " rolls." << endl;
			findSolutionWithThreadPool(processing_pool);
			++(local_parameters.total_rolls);
			if (optimalSolution.checkUpdated() == false) {
				limit_reached = true;
			}
		}
	}

	void findSolutionWithThreadPool(thread_pool& processing_pool) {
		for (int i = 0; i < ((int)local_parameters.possible_bets.size()); ++i) { //Modify the stakeFinder objects for processing
			tasks_vector[i].setParametersForProcessing(dynamic_solution_start, i);
		}
		vector<std::future<void>> futures(local_parameters.possible_bets.size()); //Populate vector of futures by sending into the pool
		for (int j = 0; j < ((int)local_parameters.possible_bets.size()); ++j) {
			futures[j] = processing_pool.submit(tasks_vector[j]);
		}
		for (int p = 0; p < ((int)futures.size()); ++p) { //Program can't continue until all tasks complete in pool
			futures[p].get();
		}
	}

	void printOutputTable() {
		cout << std::setprecision(2);
		cout << std::fixed; //Disable scientific notation for large numbers
		cout << endl;
		const char separator = ' ';
		printColumnHeaders();

		double cumulative_stake = 0.0;
		double p_win_single_roll = ((double)local_parameters.board_hits / (double)local_parameters.board_size);
		double p_loss_single_roll = 1.0 - p_win_single_roll;
		double p_loss_prev;
		double p_win_exact_sum = 0;
		double p_lose_on_final = 0;
		double loss_EV = 0;
		double p_win_exact;

		vector<double> best_stakes = optimalSolution.reference_best_stakes();
		for (int i = 0; i < (int)optimalSolution.get_best_size(); ++i) {
			//** Computations */
			cumulative_stake += best_stakes[i];
			double profit = (((local_parameters.payout_factor + 1) * best_stakes[i]) - cumulative_stake);

			if (i != 0) {
				p_win_exact = p_win_single_roll * p_loss_prev;
				p_loss_prev = p_loss_prev * p_loss_single_roll;
			}
			else {
				p_win_exact = p_win_single_roll;
				p_loss_prev = p_loss_single_roll;
			}
			p_win_exact_sum += p_win_exact;
			double win_EV = (profit * p_win_exact);

			//** Output */
			cout << std::setprecision(2);
			cout << left << setw(6) << setfill(separator) << (i + 1); //Roll number
			cout << left << setw(10) << setfill(separator) << (best_stakes[i]); //Stake
			cout << left << setw(12) << setfill(separator) << cumulative_stake;
			cout << left << setw(10) << setfill(separator) << profit;
			cout << std::setprecision(5);
			cout << left << setw(14) << setfill(separator) << p_win_exact;
			cout << left << setw(18) << setfill(separator) << p_win_exact_sum;
			if (i != ((int)best_stakes.size() - 1)) {
				cout << left << setw(19) << setfill(separator) << "-------";
			}
			else {
				cout << std::setprecision(5);
				p_lose_on_final = 1.0 - p_win_exact_sum;
				cout << left << setw(19) << setfill(separator) << p_lose_on_final;
			}
			cout << left << setw(10) << setfill(separator) << win_EV;

			if (i != ((int)best_stakes.size() - 1)) {
				cout << left << setw(11) << setfill(separator) << "-------";
			}
			else {
				cout << std::setprecision(5);
				loss_EV = p_lose_on_final * cumulative_stake;
				cout << left << setw(11) << setfill(separator) << loss_EV;
			}

			cout << endl;
		}

		cout << endl;
		cout << "Win EV Sum " << optimalSolution.get_best_win_EV_sum() << endl;
		cout << "Net EV " << (optimalSolution.get_best_win_EV_sum() - loss_EV) << endl;
		cout << endl;
	}

private:
	Solution optimalSolution; //Still need to keep this separate because of the mutex
	SharedParameters local_parameters;
	vector<double> dynamic_solution_start;
	vector<stakeFinder> tasks_vector;
	double min_increment;
	unsigned num_threads;

	void printColumnHeaders() {
		const char separator = ' ';
		cout << left << setw(6) << setfill(separator) << "Roll";
		cout << left << setw(10) << setfill(separator) << "Stake";
		cout << left << setw(12) << setfill(separator) << "Cum. Stake";
		cout << left << setw(10) << setfill(separator) << "Profit";
		cout << left << setw(14) << setfill(separator) << "p(win exact)";
		cout << left << setw(18) << setfill(separator) << "Sum p(win exact)";
		cout << left << setw(19) << setfill(separator) << "p(lose on final)";
		cout << left << setw(10) << setfill(separator) << "Win EV";
		cout << left << setw(11) << setfill(separator) << "Loss EV";
		cout << endl;
		for (int i = 0; i < 107; ++i) {
			cout << "=";
		}
		cout << endl;
	}

	int getLowestBoundRolls() {
		int num_rolls = 0;
		double betFactor = 1 + (1.0 / local_parameters.payout_factor);
		double bet = local_parameters.min_bet;
		while (bet <= local_parameters.max_bet) {
			++num_rolls;
			bet *= betFactor;
		}
		return num_rolls;
	}

	void setupPossibleBets() {
		double insertBet = local_parameters.min_bet;
		while (insertBet <= local_parameters.max_bet) {
			local_parameters.possible_bets.push_back(insertBet);
			insertBet += min_increment;
		}
	}

	void prepDynamicSolution() {
		dynamic_solution_start.resize(local_parameters.total_rolls);
		local_parameters.cumulative_stake_starting = 0.0;

		if (local_parameters.total_rolls >= ((local_parameters.payout_factor * 2) + 2)) {
			//First payout_factor + 1 numbers can be set to min
			if (local_parameters.allowBreakEven) {
				local_parameters.starting_stake = (local_parameters.payout_factor + 1);
				for (int i = 0; i < (local_parameters.payout_factor + 1); ++i) {
					dynamic_solution_start[i] = local_parameters.min_bet;
					local_parameters.cumulative_stake_starting += local_parameters.min_bet;
				}
			}
			else {
				local_parameters.starting_stake = local_parameters.payout_factor;
				for (int i = 0; i < local_parameters.payout_factor; ++i) {
					dynamic_solution_start[i] = local_parameters.min_bet;
					local_parameters.cumulative_stake_starting += local_parameters.min_bet;
				}
			}
		}
		else {
			local_parameters.starting_stake = 1;
			dynamic_solution_start[0] = local_parameters.min_bet;
			local_parameters.cumulative_stake_starting += local_parameters.min_bet;
		}
	}

	void buildTasksVector() {
		tasks_vector.resize(local_parameters.possible_bets.size(), stakeFinder(optimalSolution, local_parameters));
	}

	double getWinEV(const vector<double> &stakes_in) {
		double winEVsum = 0;
		double p_win_single_roll = ((double)local_parameters.board_hits / (double)local_parameters.board_size);
		double p_loss_single_roll = 1.0 - p_win_single_roll;

		double cumulative_stake = 0.0;
		double p_loss_prev;
		double p_win_exact_roll;

		for (int i = 0; i < (int)stakes_in.size(); ++i) {
			cumulative_stake += stakes_in[i];
			double profit = (((local_parameters.payout_factor + 1) * stakes_in[i]) - cumulative_stake);

			if (i != 0) {
				p_win_exact_roll = p_win_single_roll * p_loss_prev;
				p_loss_prev = p_loss_prev * p_loss_single_roll;
			}
			else {
				p_win_exact_roll = p_win_single_roll;
				p_loss_prev = p_loss_single_roll;
			}

			winEVsum += (p_win_exact_roll * profit);
		}
		return winEVsum;
	}
};

#endif