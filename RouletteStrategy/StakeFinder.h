//Brett Swiecicki
#ifndef STAKEFINDER_H
#define STAKEFINDER_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <thread>
#include <mutex>
#include "Solution.h"
#include "SharedParameters.h"
using namespace std;

class stakeFinder {
public:
	stakeFinder(Solution& solution_in, SharedParameters& local_parameters_in)
		: bestSolution(solution_in), local_parameters(local_parameters_in) {}

	void setParametersForProcessing(vector<double>& dynamic_solution_in, int last_bet_in) {
		dynamic_solution = dynamic_solution_in;
		starting_last_bet = last_bet_in;
	}

	void operator()() {
		solutionFindRec(local_parameters.starting_stake, local_parameters.cumulative_stake_starting, starting_last_bet);
	}

private:
	Solution& bestSolution;
	SharedParameters& local_parameters;
	vector<double> dynamic_solution;
	int starting_last_bet;
	

	void solutionFindRec(int stake_number, double cumulative_stake, int lastBetAdded) {
		if (stake_number == (local_parameters.total_rolls - 1)) {
			dynamic_solution[stake_number] = local_parameters.max_bet;
			cumulative_stake += local_parameters.max_bet;
			bool profitable = checkIfProfitable(dynamic_solution, stake_number, cumulative_stake);
			if (profitable) {
				double dynamic_win_EV_sum = getWinEV(dynamic_solution);
				if ((dynamic_solution.size() > bestSolution.get_best_size()) ||
					(((dynamic_solution.size() == bestSolution.get_best_size()) && (dynamic_win_EV_sum > bestSolution.get_best_win_EV_sum())))) {
					bestSolution.change_best_stakes(dynamic_solution);
					bestSolution.change_best_win_EV_sum(dynamic_win_EV_sum);
					bestSolution.solutionUpdated();
				}
			}
			return;
		}

		for (int i = lastBetAdded; i < (int)bestSolution.get_best_size(); ++i) {
			dynamic_solution[stake_number] = local_parameters.possible_bets[i];
			bool profitable = checkIfProfitable(dynamic_solution, stake_number, cumulative_stake + local_parameters.possible_bets[i]);
			if (profitable) {
				solutionFindRec(stake_number + 1, cumulative_stake + local_parameters.possible_bets[i], i);
			}
		}
	}

	bool checkIfProfitable(vector<double>& stakes_in, int stake_number, double cumulative_stake) {
		double profit = (((local_parameters.payout_factor + 1) * stakes_in[stake_number]) - cumulative_stake);
		if (local_parameters.allowBreakEven) {
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
