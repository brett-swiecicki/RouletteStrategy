//Brett Swiecicki
#ifndef OPTDESCENDING_H
#define OPTDESCENDING_H

#include "ProcessorCommons.h"
using namespace std;

class OptimalDescendingProcessor {
public:
	OptimalDescendingProcessor(ProcessorCommons& osp_commons_in)
	: osp_commons(osp_commons_in) {}

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

private:
	ProcessorCommons& osp_commons;

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
};

#endif
