//Brett Swiecicki
#ifndef BRUTEOPT_H
#define BRUTEOPT_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
using namespace std;

class OptimalSolutionProcessor {
public:

	void getInput() {
		cout << "Enter the table minimum bet: ";
		cin >> min_bet;
		cout << endl << "Enter the table maximum bet: ";
		cin >> max_bet;
		cout << endl << "Enter the table minimum bet increment: ";
		cin >> min_increment;
		cout << endl << "Enter the payout factor [__ to 1]: ";
		cin >> payout_factor;
		cout << endl << "Enter the number of winning table positions (out of 37): ";
		cin >> board_hits;
		cout << endl;
		//Possilbly enable some constraint satisfaction requirements here.
	} //End of getInput

	void findSolution() {
		double insertBet = min_bet;
		while (insertBet <= max_bet) {
			possible_bets.push_back(insertBet);
			insertBet += min_increment;
		}
		best_win_EV_sum = 0;
		bool limit_reached = false;
		total_rolls = 1;
		while (limit_reached == false) { 
			dynamic_solution.resize(total_rolls);
			solutionFindRec(0);
			++total_rolls;
			//Limit is reached when the size of the dynamic solution didn't increase as it is anticipated it will due to what is considered optimal
			//So when total_rolls becomes 2, it is anticipated that dynamic solution size is 1
			if ((dynamic_solution.size() != (total_rolls - 1)) || (total_rolls == 11)) {
				limit_reached = true;
			}
		}
	}

	void solutionFindRec(int stake_number) {
		if (stake_number == total_rolls) {
			double dynamic_win_EV_sum = getWinEV(dynamic_solution);
			if ((dynamic_solution.size() > best_stakes.size()) || 
				(((dynamic_solution.size() == best_stakes.size()) && (dynamic_win_EV_sum > best_win_EV_sum)))) {
				best_stakes = dynamic_solution;
				best_win_EV_sum = dynamic_win_EV_sum;
			}
			return;
		}
		for (int i = 0; i < (int)possible_bets.size(); ++i) {
			dynamic_solution[stake_number] = possible_bets[i];
			bool constraints_satisfied = checkConstraintSatisfaction(dynamic_solution);
			if (constraints_satisfied) {
				solutionFindRec(stake_number + 1);
			}
		}
	}

	void printOutputTable() {
		cout << std::setprecision(3);
		cout << std::fixed; //Disable scientific notation for large numbers
		const char separator = ' ';
		printColumnHeaders();

		//Win EV Sum, Net EV
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
	int total_rolls;
	
	double getWinEV(vector<double> &stakes_in) {
		double winEVsum = 0;
		double p_win_single_roll = ((double)board_hits / 37.0);
		double cumulative_stake = 0.0;
		double p_loss_prev = 1.0 - p_win_single_roll;
		for (int i = 0; i < (int)stakes_in.size(); ++i) {
			cumulative_stake += stakes_in[i];
			double profit = (((payout_factor + 1) * stakes_in[i]) - cumulative_stake);
			double p_win_exact_roll = p_win_single_roll * p_loss_prev;
			winEVsum += (p_win_exact_roll * profit);
			p_loss_prev = p_loss_prev * p_loss_prev;
		}
		return winEVsum;
	}

	bool checkConstraintSatisfaction(vector<double> &stakes_in) {
		double prev = stakes_in[0];
		if (stakes_in.size() > 1) {
			for (int i = 1; i < (int)stakes_in.size(); ++i) {
				if (prev > stakes_in[i]) {
					return false;
				}
				prev = stakes_in[i];
			}
		}
		//All bets must also produce profit or break even
		double cumulative_stake = 0.0;
		for (int j = 1; j < (int)stakes_in.size(); ++j) {
			cumulative_stake += stakes_in[j];
			double profit = (((payout_factor + 1) * stakes_in[j]) - cumulative_stake);
			if (profit < 0) {
				return false;
			}
		}
		return true;
	}

	void printColumnHeaders() {
		const char separator = ' ';
		cout << left << setw(6) << setfill(separator) << "Roll";
		cout << left << setw(7) << setfill(separator) << "Stake";
		cout << left << setw(12) << setfill(separator) << "Cum. Stake";
		cout << left << setw(8) << setfill(separator) << "Profit";
		cout << left << setw(14) << setfill(separator) << "p(win exact)";
		cout << left << setw(18) << setfill(separator) << "Sum p(win exact)";
		cout << left << setw(20) << setfill(separator) << "p(lose on final)";
		cout << left << setw(8) << setfill(separator) << "Win EV";
		cout << left << setw(9) << setfill(separator) << "Loss EV";
		cout << endl;
		for (int i = 0; i < 102; ++i) {
			cout << "=";
		}
		cout << endl;
	}
};

#endif