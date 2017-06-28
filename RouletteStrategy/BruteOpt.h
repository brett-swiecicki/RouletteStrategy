//Brett Swiecicki
#ifndef BRUTEOPT_H
#define BRUTEOPT_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <Windows.h>
#include <wchar.h>
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
		cin >> min_bet;
		cout << "Enter the table maximum bet: ";
		cin >> max_bet;
		cout << "Enter the table minimum bet increment: ";
		cin >> min_increment;
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
		cout << endl;
	} //End of getInput

	void findSolution() {
		double insertBet = min_bet;
		while (insertBet <= max_bet) {
			possible_bets.push_back(insertBet);
			insertBet += min_increment;
		}
		best_win_EV_sum = 0;
		bool limit_reached = false;
		total_rolls = getLowestBoundRolls(); //Linear incrementation

		if (total_rolls == 1) {
			best_stakes.resize(total_rolls);
			best_stakes[0] = max_bet;
			return;
		}
		
		while (limit_reached == false) {
			int startingStake;
			double startingCumulative = 0.0;
			dynamic_solution.resize(total_rolls);
			if (total_rolls >= ((payout_factor * 2) + 2)) {
				//First payout_factor + 1 numbers can be set to min
				if (allowBreakEven) {
					startingStake = (payout_factor + 1);
					for (int i = 0; i < (payout_factor + 1); ++i) {
						dynamic_solution[i] = min_bet;
						startingCumulative += min_bet;
					}
				}
				else {
					startingStake = payout_factor;
					for (int i = 0; i < payout_factor; ++i) {
						dynamic_solution[i] = min_bet;
						startingCumulative += min_bet;
					}
				}
			}
			else {
				startingStake = 1;
				dynamic_solution[0] = min_bet;
				startingCumulative += min_bet;
			}
			solutionUpdated = false;
			cout << "Currently computing strategies for " << total_rolls << " rolls." << endl;
			solutionFindRec(startingStake, startingCumulative, 0);
			++total_rolls;
			if (solutionUpdated == false) {
				limit_reached = true;
			}
		}
	}

	void solutionFindRec(int stake_number, double cumulative_stake, int lastBetAdded) {
		if (stake_number == (total_rolls - 1)) {
			dynamic_solution[stake_number] = max_bet;
			cumulative_stake += max_bet;
			bool profitable = checkIfProfitable(dynamic_solution, stake_number, cumulative_stake);
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
			bool profitable = checkIfProfitable(dynamic_solution, stake_number, cumulative_stake + possible_bets[i]);
			if (profitable) {
				solutionFindRec(stake_number + 1, cumulative_stake + possible_bets[i], i);
			}
		}
	}

	void printOutputTable() {
		cout << std::setprecision(2);
		cout << std::fixed; //Disable scientific notation for large numbers
		cout << endl;
		const char separator = ' ';
		printColumnHeaders();

		double cumulative_stake = 0.0;
		double p_win_single_roll = ((double)board_hits / (double)board_size);
		double p_loss_single_roll = 1.0 - p_win_single_roll;
		double p_loss_prev;
		double p_win_exact_sum = 0;
		double p_lose_on_final = 0;
		double loss_EV = 0;
		double p_win_exact;

		for (int i = 0; i < (int)best_stakes.size(); ++i) {
			//** Computations */
			cumulative_stake += best_stakes[i];
			double profit = (((payout_factor + 1) * best_stakes[i]) - cumulative_stake);

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
		cout << "Win EV Sum " << best_win_EV_sum << endl;
		cout << "Net EV " << (best_win_EV_sum - loss_EV) << endl;
		cout << endl;
	}

private:
	vector<double> possible_bets;
	vector<double> dynamic_solution;
	vector<double> best_stakes;
	double best_win_EV_sum;
	double min_bet;
	double max_bet;
	double min_increment;
	int payout_factor;
	int board_hits;
	int board_size;
	int total_rolls;
	bool solutionUpdated;
	bool allowBreakEven;

	bool checkIfProfitable(const vector<double> &stakes_in, int stake_number, double cumulative_stake) {
		double profit = (((payout_factor + 1) * stakes_in[stake_number]) - cumulative_stake);
		if (allowBreakEven) {
			if (profit < 0) {
				return false;
			}
		}
		else {
			if (profit <= 0) {
				return false;
			}
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
		cout << left << setw(8) << setfill(separator) << "Win EV";
		cout << left << setw(9) << setfill(separator) << "Loss EV";
		cout << endl;
		for (int i = 0; i < 107; ++i) {
			cout << "=";
		}
		cout << endl;
	}

	int getLowestBoundRolls() {
		int num_rolls = 0;
		double betFactor = 1 + (1.0 / payout_factor);
		double bet = min_bet;
		while (bet <= max_bet) {
			++num_rolls;
			bet *= betFactor;
		}
		return num_rolls;
	}
};

#endif