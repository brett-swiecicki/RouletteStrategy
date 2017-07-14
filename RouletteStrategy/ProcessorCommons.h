//Brett Swiecicki
#ifndef PROCESSORCOMMONS_H
#define PROCESSORCOMMONS_H

#include <unordered_map>
#include "OutputTable.h"
using namespace std;

class ProcessorCommons {
public:
	vector<vector<double>> all_solutions;
	unordered_map<double, int> bets_to_indices;
	vector<double> possible_bets;
	vector<double> dynamic_solution;
	vector<double> dynamic_EV_solution;
	vector<double> best_stakes;
	vector<double> best_stakes_EV;
	vector<double> p_win_exacts;
	vector<int> upper_bound_bets; //Inclusive
	string mode;
	double best_win_EV_sum;
	double best_ROI;
	double min_bet;
	double max_bet;
	double min_increment;
	double starting_cumulative;
	double dynamic_profit;
	double p_win_single;
	int starting_stake;
	int payout_factor;
	int board_hits;
	int board_size;
	int total_rolls;
	int current_table;
	int num_threads;
	bool solutionUpdated;
	bool allowBreakEven;
	bool useLowerBound;
	bool useUpperBound;
	bool fixStart;

	void printTable(int idx_to_print) {
		TablePrinter my_table_printer = TablePrinter(board_hits, board_size, payout_factor);
		my_table_printer.printOutputTable(all_solutions[idx_to_print]);
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
		int index = 0;
		while (insertBet <= max_bet) {
			possible_bets.push_back(insertBet);
			bets_to_indices[insertBet] = index;
			insertBet += min_increment;
			++index;
		}
		if (possible_bets.back() != max_bet) {
			possible_bets.push_back(max_bet);
			bets_to_indices[max_bet] = index;
		}
	}

	void prepDynamicSolution() {
		dynamic_solution.resize(total_rolls);
		starting_cumulative = 0.0;

		if (fixStart) {
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
		else {
			starting_stake = 0;
			starting_cumulative = 0.0;
		}
	}

	void constructUpperBounds() {
		upper_bound_bets.resize(total_rolls);
		for (int i = 0; i < total_rolls; ++i) {
			double raw_upper_bound = pow((1.0 + (1.0 / (double)payout_factor)), i); //Calculate raw y = (1 + (1/payout_factor))^x
			double rounded_upper_bound = roundUp(raw_upper_bound);
			if ((rounded_upper_bound >= min_bet) && (rounded_upper_bound <= max_bet)) {
				int index = bets_to_indices[rounded_upper_bound];
				upper_bound_bets[i] = index;
			}
			else if (rounded_upper_bound > max_bet) { //Set to max_bet
				upper_bound_bets[i] = (((int)possible_bets.size()) - 1);
			}
			else { //Set to min_bet
				upper_bound_bets[i] = 0;
			}
		}
	}

	void appendUpperBounds() {
		if (upper_bound_bets[total_rolls - 2] == (((int)possible_bets.size()) - 1)) {
			upper_bound_bets.push_back(((int)possible_bets.size()) - 1); //The last upper bound bet should usually be the max_bet
		}
		else {
			double raw_upper_bound = pow((1.0 + (1.0 / (double)payout_factor)), (double)upper_bound_bets.size()); //Calculate raw y = (1 + (1/payout_factor))^x
			double rounded_upper_bound = roundUp(raw_upper_bound);
			if (rounded_upper_bound > max_bet) {
				upper_bound_bets.push_back(((int)possible_bets.size()) - 1);
			}
			else {
				int index = bets_to_indices[rounded_upper_bound];
				upper_bound_bets.push_back(index);
			}
		}
	}

	double roundUp(double num_to_round) {
		double remainder = fmod(num_to_round, min_increment);
		if (remainder != 0.0) {
			return (num_to_round + min_increment - remainder);
		}
		else {
			return num_to_round;
		}
	}

	double singleRollEV(int stake_number, double cumulative_stake, double bet_in) {
		double profit = (((payout_factor + 1) * bet_in) - cumulative_stake - bet_in);
		return (p_win_exacts[stake_number] * profit);
	}

	void construct_p_win_exacts() {
		//This function is fucked!!!
		p_win_exacts.resize(total_rolls);
		p_win_exacts[0] = p_win_single;
		for (int i = 1; i < (int)p_win_exacts.size(); ++i) {
			double p_loss_on_prior = pow((double)(1.0 - p_win_single), (double)i);
			p_win_exacts[i] = (p_loss_on_prior * p_win_single);
		}
	}

	void append_p_win_exacts() {
		double p_loss_on_prior = pow((double)(1.0 - p_win_single), (double)p_win_exacts.size());
		p_win_exacts.push_back(p_loss_on_prior * p_win_single);
	}

	void clearPrivateData() {
		all_solutions.clear();
		bets_to_indices.clear();
		possible_bets.clear();
		dynamic_solution.clear();
		dynamic_EV_solution.clear();
		best_stakes.clear();
		best_stakes_EV.clear();
		p_win_exacts.clear();
		upper_bound_bets.clear();
		mode = "";
		best_win_EV_sum = 0.0;
		min_bet = 0.0;
		max_bet = 0.0;
		min_increment = 0.0;
		starting_cumulative = 0.0;
		dynamic_profit = 0.0;
		p_win_single = 0.0;
		starting_stake = 0;
		payout_factor = 0;
		board_hits = 0;
		board_size = 0;
		total_rolls = 0;
		current_table = 0;
		solutionUpdated = false;
		allowBreakEven = false;
		useLowerBound = false;
		useUpperBound = false;
		fixStart = false;
	}

private:
	
};

#endif
