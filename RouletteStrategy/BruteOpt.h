//Brett Swiecicki
#ifndef BRUTEOPT_H
#define BRUTEOPT_H

#include <iostream>
#include <vector>
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

			

			++total_rolls;
			//Limit is reached when not a single valid permuation resulted in a stake sequence which had positive EV after roll one
		}

		//WANT: Greatest number of rolls and greatest win EV possible for that roll #
	}

	void solutionFindRec() {
		//This is basically going to be like gen perms with a base case
		//Base case is when the level is equivalent to the total_rolls number

		//gen perm
		//check for constraint satisfaction
		//if satisfied
		//check for best
	}

	void printOutputTable() {
		//Output
		//Print out to table
		//Rolls, Stake, Cumulative Stake, Profit, Sum of p(win exact roll), p(lose on last roll), Win EV, Loss EV, Win EV Sum, Net EV
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

	}

	bool checkConstraintSatisfaction(vector<double> &stakes_in) {
		//the last stake wagered is less than or equal to the table max bet and following stakes are greater than or equal to previous stakes
	}
};

#endif
