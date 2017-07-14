//Brett Swiecicki
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>
#include <random>
using namespace std;

class Simulator {
public:

	Simulator() { //Default constructor gets all input from user
		updateParameters();
		cout << "Please enter the total number of simulations you would like to run: ";
		cin >> total_sims;
	}

	Simulator(vector<double>& strat_stakes_in, int total_rolls_in, int board_size_in, int board_hits_in, double payout_factor_in) {
		strat_stakes = strat_stakes_in;
		total_rolls = total_rolls_in;
		board_size = board_size_in;
		board_hits = board_hits_in;
		payout_factor = payout_factor_in;
		cout << "Please enter the total number of simulations you would like to run: ";
		cin >> total_sims;
	}

	Simulator(int board_size_in, int board_hits_in, double payout_factor_in) 
		: board_size(board_size_in), board_hits(board_hits_in), payout_factor(payout_factor_in){}

	void runSimulations() {
		clock_t t; //Start clock
		t = clock();
		cout << endl;
		cout << "Currently running " << total_sims << " simulations..." << endl;
		pair<int, int> sim_results = hergieSim();
		t = clock() - t;
		cout << "Total running time: " << (((float)t) / (CLOCKS_PER_SEC)) << " seconds." << endl;
		int total = (sim_results.first + sim_results.second);
		double win_percent = ((((double)sim_results.first)/((double)total)) * 100.0);
		double loss_percent = ((((double)sim_results.second) / ((double)total)) * 100.0);
		cout << std::setprecision(5);
		cout << "Number of wins (minimum required bankroll doubled): " << sim_results.first << ", (" << win_percent << "%)." << endl;
		cout << "Number of losses (minimum required bankroll hit $0.00): " << sim_results.second << ", (" << loss_percent << "%)." << endl;
		cout << "Predicted ROI: " << (win_percent - loss_percent) << "%" << endl;
	}

	double getSimulationROI(vector<double>& strat_stakes_in, int total_sims_in) {
		total_rolls = (int)strat_stakes_in.size();
		strat_stakes = strat_stakes_in; //THIS COULD BE BAD!
		pair<int, int> sim_results = hergieSim();
		int total = (sim_results.first + sim_results.second);
		double win_percent = ((((double)sim_results.first) / ((double)total)) * 100.0);
		double loss_percent = ((((double)sim_results.second) / ((double)total)) * 100.0);
		return (win_percent - loss_percent);
	}

	void query_for_additional_simulations() {
		char decision = 'Y';
		while ((decision != 'N') && (decision != 'n') && (decision != '0')) {
			cout << endl;
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
		cout << endl;
	}

	void updateParameters() {
		cout << "Enter how many rolls your strategy can have before breaking: ";
		cin >> total_rolls;
		strat_stakes.resize(total_rolls);
		cout << "Please enter each bet in order starting with the first: " << endl;
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
		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		std::mt19937 engine(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<> dist(0, (board_size - 1));

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
			dynamic_bankroll = bankroll; //reset the bankroll amount to run the whole strat
			int roll_on_strat = 0;
			while ((dynamic_bankroll > 0.0) && (dynamic_bankroll < (bankroll * 2))) {
				//int random_num = (rand() % board_size); // random_num in the range 0 to E 37 or A 38
				int random_num = dist(engine);
				if (strat_stakes[roll_on_strat] <= dynamic_bankroll) { //Can you even make the bet?
					bet = strat_stakes[roll_on_strat];
					dynamic_bankroll -= bet;
				}
				else {
					bet = bankroll;
					dynamic_bankroll -= bet;
				}
				if (random_num < board_hits) { //HIT!
					dynamic_bankroll += ((payout_factor + 1) * bet);
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
				++num_wins;
			}
			++sim_count;
		}
		return_pair.first = num_wins;
		return_pair.second = num_losses;
		return return_pair;
	}
};

#endif
