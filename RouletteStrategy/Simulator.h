//Brett Swiecicki
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>
using namespace std;

class Simulator {
public:

	Simulator() { //Default constructor gets all input from user
		updateParameters();
		cout << "Enter the total amount of simulations you would like: ";
		cin >> total_sims;
	}

	Simulator(vector<double>& strat_stakes_in, int total_rolls_in, int board_size_in, int board_hits_in, double payout_factor_in) {
		strat_stakes = strat_stakes_in;
		total_rolls = total_rolls_in;
		board_size = board_size_in;
		payout_factor = payout_factor_in;
		cout << "Please enter the total number of simulations you would like to run: ";
		cin >> total_sims;
	}

	void runSimulations() {
		clock_t t; //Start clock
		t = clock();
		pair<int, int> sim_results = hergieSim();
		t = clock() - t;
		cout << "Currently running " << total_sims << " simulations..." << endl;
		cout << "Total running time: " << (((float)t) / (CLOCKS_PER_SEC)) << " seconds." << endl;
		cout << "Number of wins: " << sim_results.first << endl;
		cout << "Number of losses: " << sim_results.second << endl;
	}

	void query_for_additional_simulations() {
		char decision;
		while ((decision != 'N') && (decision != 'n') && (decision != '0')) {
			cout << "Would you like to run more simulations? Y or N: ";
			cin >> decision;
			if ((decision == 'Y') || (decision == 'y') || (decision == '1')) {
				char update;
				cout << "Would you like to update the strategy? Y or N: ";
				cin >> update;
				if ((update == 'Y') || (update == 'y') || (update == '1')) {
					updateParameters();
				}
				cout << "Please enter the total number of simulations you would like to run: ";
				cin >> total_sims;
				runSimulations();
			}
		}
	}

	void updateParameters() {
		cout << "Enter how many roles your strategy can have before breaking: ";
		cin >> total_rolls;
		strat_stakes.resize(total_rolls);
		cout << "Please enter each bet in order starting with the first: ";
		double input_bet;
		for (int i = 0; i < total_rolls; ++i) {
			cin >> input_bet;
			strat_stakes[i] = input_bet;
		}

		cout << "Enter the payout factor [__ to 1]: ";
		cin >> payout_factor;
		cout << "Enter the number of winning table positions: ";
		cin >> board_hits;
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
	}

private:
	vector<double> strat_stakes;
	int total_rolls;
	int board_size;
	int board_hits;
	int total_sims;
	double payout_factor;

	pair<int, int> hergieSim() {
		pair<int, int> return_pair;
		int num_wins = 0;
		int num_losses = 0;
		double bankroll = 0.0;
		for (int i = 0; i < (int)strat_stakes.size(); ++i) {
			bankroll += strat_stakes[i];
		}
		double dynamic_bankroll;
		double bet;
		int sim_count = 0;
		while (sim_count < total_sims) {
			dynamic_bankroll = 0.0;
			int roll_on_strat = 0;
			while ((dynamic_bankroll > 0.0) && (dynamic_bankroll < (bankroll * 2))) {
				int random_num = (rand() % board_size); // random_num in the range 0 to E 37 or A 38
				if (strat_stakes[roll_on_strat] <= dynamic_bankroll) { //Can you even make the bet?
					bet = strat_stakes[roll_on_strat];
					dynamic_bankroll -= bet;
				}
				else {
					bet = bankroll;
					dynamic_bankroll -= bet;
				}
				if (random_num < board_hits) { //HIT!
					dynamic_bankroll += (payout_factor * bet);
					roll_on_strat = 0;
				}
				else { //MISS!
					++roll_on_strat;
					if (roll_on_strat == (int)strat_stakes.size()) {
						//Hit the end of the strat? What do we do? Go back to first roll?
						roll_on_strat = 0;
					}
				}
			} //End of inner while loop (actual simulations)

			if (dynamic_bankroll <= 0.0) {
				++num_losses;
			}
			else {
				++num_losses;
			}
			++sim_count;
		}
		return_pair.first = num_wins;
		return_pair.second = num_losses;
		return return_pair;
	}
};

#endif
