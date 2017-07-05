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

using namespace std;

class OptimalSolutionProcessor {
public:

	void getInput() {
		cout << "Enter the table minimum bet: ";
		cin >> min_bet;
		cout << "Enter the table maximum bet: ";
		cin >> max_bet;
		cout << "Enter the table minimum bet increment: ";
		cin >> min_increment;
		cout << "American or European? A or E: ";
		char AmOrEur;
		cin >> AmOrEur;
		if ((AmOrEur == 'A') || (AmOrEur == 'a')) {
			board_size = 38;
		}
		else if ((AmOrEur == 'E') || (AmOrEur == 'e')) {
			board_size = 37;
		}
		else {
			cerr << "Incorrect selection was made: " << AmOrEur << endl;
			exit(1);
		}
		cout << "Enter the payout factor [__ to 1]: ";
		cin >> payout_factor;
		cout << "Enter the number of winning table positions: ";
		cin >> board_hits;
		cout << "Are break even bets acceptable? Y or N: ";
		char breakEven;
		cin >> breakEven;
		if ((breakEven == 'Y') || (breakEven == 'y') || (breakEven == '1')) {
			allowBreakEven = true;
		}
		else if ((breakEven == 'N') || (breakEven == 'n') || (breakEven == '0')) {
			allowBreakEven = false;
		}
		else {
			cerr << "Incorrect selection was made: " << breakEven << endl;
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
		setupPossibleBets();
		total_rolls = getLowestBoundRolls(); //Linear incrementation of total_rolls

		if (total_rolls == 1) {
			best_stakes.resize(total_rolls);
			best_stakes[0] = max_bet;
			all_solutions.push_back(best_stakes);
			printOutputTable(0);
			return;
		}
		
		bool limit_reached = false;
		while (limit_reached == false) {
			prepDynamicSolution();
			solutionUpdated = false;
			cout << "Currently computing strategies for " << total_rolls << " rolls." << endl;
			if (allowBreakEven) {
				solutionFindRecBreakEven(starting_stake, starting_cumulative, 0);
			}
			else {
				solutionFindRecNoBreakEven(starting_stake, starting_cumulative, 0);
			}
			++total_rolls;
			if (solutionUpdated == false) {
				limit_reached = true;
			}
			else {
				all_solutions.push_back(best_stakes);
			}
		}
		t = clock() - t; //Print running time!
		cout << "Total running time: " << (((float)t) / (CLOCKS_PER_SEC)) << " seconds." << endl;
		printOutputTable(((int)all_solutions.size()) - 1); //Convert to actual index
	}

	void printOutputTable(int index_to_print) {
		cout << std::setprecision(2);
		cout << std::fixed; //Disable scientific notation for large numbers
		cout << endl;
		const char separator = ' ';
		printColumnHeaders();

		double cumulative_stake = 0.0;
		double p_win_single_roll = ((double)board_hits / (double)board_size);
		double p_loss_single_roll = 1.0 - p_win_single_roll;
		double p_loss_prev = 0;
		double p_win_exact_sum = 0;
		double p_lose_on_final = 0;
		double loss_EV = 0;
		double p_win_exact;
		vector<double>& desired_solution = all_solutions[index_to_print];
		double desired_win_EV_sum = getWinEV(desired_solution);

		for (int i = 0; i < (int)desired_solution.size(); ++i) {
			//** Computations */
			cumulative_stake += desired_solution[i];
			double profit = (((payout_factor + 1) * desired_solution[i]) - cumulative_stake);

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
			cout << left << setw(10) << setfill(separator) << (desired_solution[i]); //Stake
			cout << left << setw(12) << setfill(separator) << cumulative_stake;
			cout << left << setw(10) << setfill(separator) << profit;
			cout << std::setprecision(5);
			cout << left << setw(14) << setfill(separator) << p_win_exact;
			cout << left << setw(18) << setfill(separator) << p_win_exact_sum;
			if (i != ((int)desired_solution.size() - 1)) {
				cout << left << setw(19) << setfill(separator) << "-------";
			}
			else {
				cout << std::setprecision(5);
				p_lose_on_final = 1.0 - p_win_exact_sum;
				cout << left << setw(19) << setfill(separator) << p_lose_on_final;
			}
			cout << left << setw(10) << setfill(separator) << win_EV;

			if (i != ((int)desired_solution.size() - 1)) {
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
		cout << "Win EV Sum " << desired_win_EV_sum << endl;
		cout << "Net EV " << (desired_win_EV_sum - loss_EV) << endl;
		cout << endl;
	}

	void queryForAdditionalTables() {
		
		int smallest_roll_count = (int)all_solutions.front().size();
		int largest_roll_count = (int)all_solutions.back().size();

		cout << "Would you like to see the output for another solution? Y or N: ";
		char printMore;
		cin >> printMore;
		if ((printMore == 'Y') || (printMore == 'y') || (printMore == '1')) {
			while ((printMore != 'N') && (printMore != 'n') && (printMore != '0')) {
				cout << "Enter the number of rolls for which you would like to see a solution (" << smallest_roll_count << " through " << largest_roll_count << "): ";
				int desiredSolution;
				cin >> desiredSolution;
				if ((desiredSolution >= smallest_roll_count) && (desiredSolution <= largest_roll_count)) {
					int actual_index = (((int)all_solutions.size()) - (largest_roll_count - desiredSolution) - 1);
					printOutputTable(actual_index);
				}
				else {
					cout << "Sorry! " << desiredSolution << " does not have a computed solution!" << endl;
				}
				cout << "Would you like to see the output for another solution? Y or N: ";
				cin >> printMore;
			}
		}
	}

private:
	vector<vector<double>> all_solutions;
	vector<double> possible_bets;
	vector<double> dynamic_solution;
	vector<double> best_stakes;
	double best_win_EV_sum;
	double min_bet;
	double max_bet;
	double min_increment;
	double starting_cumulative;
	int starting_stake;
	int payout_factor;
	int board_hits;
	int board_size;
	int total_rolls;
	bool solutionUpdated;
	bool allowBreakEven;

	void solutionFindRecBreakEven(int stake_number, double cumulative_stake, int lastBetAdded) {
		if (stake_number == (total_rolls - 1)) {
			dynamic_solution[stake_number] = max_bet;
			cumulative_stake += max_bet;
			bool profitable = checkIfProfitableBreakEven(dynamic_solution, stake_number, cumulative_stake);
			if (profitable) {
				double dynamic_win_EV_sum = getWinEV(dynamic_solution);
				if ((dynamic_solution.size() > best_stakes.size()) ||
					(((dynamic_solution.size() == best_stakes.size()) && (dynamic_win_EV_sum > best_win_EV_sum)))) {
					best_stakes = dynamic_solution;
					best_win_EV_sum = dynamic_win_EV_sum;
					solutionUpdated = true;
				}
			}
			return;
		}

		for (int i = lastBetAdded; i < (int)possible_bets.size(); ++i) {
			dynamic_solution[stake_number] = possible_bets[i];
			bool profitable = checkIfProfitableBreakEven(dynamic_solution, stake_number, cumulative_stake + possible_bets[i]);
			if (profitable) {
				solutionFindRecBreakEven(stake_number + 1, cumulative_stake + possible_bets[i], i);
			}
		}
	}

	void solutionFindRecNoBreakEven(int stake_number, double cumulative_stake, int lastBetAdded) {
		if (stake_number == total_rolls) {
			double dynamic_win_EV_sum = getWinEV(dynamic_solution);
			if ((dynamic_solution.size() > best_stakes.size()) ||
				(((dynamic_solution.size() == best_stakes.size()) && (dynamic_win_EV_sum > best_win_EV_sum)))) {
				best_stakes = dynamic_solution;
				best_win_EV_sum = dynamic_win_EV_sum;
				solutionUpdated = true;
			}
			return;
		}
		for (int i = lastBetAdded; i < (int)possible_bets.size(); ++i) {
			dynamic_solution[stake_number] = possible_bets[i];
			bool profitable = checkIfProfitableNoBreakEven(dynamic_solution, stake_number, cumulative_stake + possible_bets[i]);
			if (profitable) {
				solutionFindRecNoBreakEven(stake_number + 1, cumulative_stake + possible_bets[i], i);
			}
		}
	}

	bool checkIfProfitableBreakEven(const vector<double> &stakes_in, int stake_number, double cumulative_stake) {
		double profit = (((payout_factor + 1) * stakes_in[stake_number]) - cumulative_stake);
		if (profit < 0) {
			return false;
		}
		return true;
	}

	bool checkIfProfitableNoBreakEven(const vector<double> &stakes_in, int stake_number, double cumulative_stake) {
		double profit = (((payout_factor + 1) * stakes_in[stake_number]) - cumulative_stake);
		if (profit <= 0) {
			return false;
		}
		return true;
	}

	double getWinEV(const vector<double> &stakes_in) {
		double winEVsum = 0;
		double p_win_single_roll = ((double)board_hits / (double)board_size);
		double p_loss_single_roll = 1.0 - p_win_single_roll;

		double cumulative_stake = 0.0;
		double p_loss_prev;
		double p_win_exact_roll;

		for (int i = 0; i < (int)stakes_in.size(); ++i) {
			cumulative_stake += stakes_in[i];
			double profit = (((payout_factor + 1) * stakes_in[i]) - cumulative_stake);

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
		double betFactor = 1 + (1.0 / (double)payout_factor);
		double bet = min_bet;
		while (bet <= max_bet) {
			++num_rolls;
			bet *= betFactor;
		}
		return num_rolls;
	}

	void setupPossibleBets() {
		double insertBet = min_bet;
		while (insertBet <= max_bet) {
			possible_bets.push_back(insertBet);
			insertBet += min_increment;
		}
	}

	void prepDynamicSolution() {
		dynamic_solution.resize(total_rolls);
		starting_cumulative = 0.0;

		if (total_rolls >= ((payout_factor * 2) + 2)) {
			//First payout_factor + 1 numbers can be set to min
			if (allowBreakEven) {
				starting_stake = (payout_factor + 1);
				for (int i = 0; i < (payout_factor + 1); ++i) {
					dynamic_solution[i] = min_bet;
					starting_cumulative += min_bet;
				}
			}
			else {
				starting_stake = payout_factor;
				for (int i = 0; i < payout_factor; ++i) {
					dynamic_solution[i] = min_bet;
					starting_cumulative += min_bet;
				}
			}
		}
		else {
			starting_stake = 1;
			dynamic_solution[0] = min_bet;
			starting_cumulative += min_bet;
		}
	}

};

#endif