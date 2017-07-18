//Brett Swiecicki
#ifndef OPTSUM_H
#define OPTSUM_H

#include "ProcessorCommons.h"
using namespace std;

class OptimalSumProcessor {
public:
	OptimalSumProcessor(ProcessorCommons& osp_commons_in)
		: osp_commons(osp_commons_in) {}

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
			cout << flush;
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

private:
	ProcessorCommons& osp_commons;

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
