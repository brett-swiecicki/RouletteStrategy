//Brett Swiecicki
#ifndef PLAYTHROUGHSIMULATOR_H
#define PLAYTHROUGHSIMULATOR_H

#include <iomanip>
#include <iostream>
#include <random>
#include <time.h>
#include <vector>
using namespace std;

class PlaythroughSimulator {
public:

	PlaythroughSimulator(bool get_data = false) {
		if (get_data) {
			updateParameters();
		}
		getSimulationCount();
	}

	void runSimulations() {
		clock_t t = clock(); //Start clock
		cout << endl;
		cout << "Currently running " << total_sims << " simulations..." << endl;
		slycickSim();
		cout << std::setprecision(5);
		printRuntime(t);
		int total = num_finishes + num_busts;
		double finish_percent = ((((double)num_finishes) / ((double)total)) * 100.0);
		double bust_percent = ((((double)num_busts) / ((double)total)) * 100.0);
		double average_final_finish = getAverageFinalFinish();
		cout << std::setprecision(5);
		cout << "Number of times playthrough requirement was reached: " << num_finishes << ", (" << finish_percent << "%)." << endl;
		cout << "Number of times starting bankroll hit $0.00: " << num_busts << ", (" << bust_percent << "%)." << endl;
		cout << std::setprecision(2);
		cout << "Average final amount when playthrough requirement was reached: $" << average_final_finish << endl;
	}

	void updateParameters() {
		getRolls();
		getBets();
		getPayoutFactor();
		getBoardHits();
		getTableType();
		getStartingBankroll();
		getBonusAmount();
		getPlaythroughRequirement();
		getContributionRate();
	}

private:
	vector<double> strat_stakes;
	vector<double> final_amounts;
	int total_rolls;
	int board_size;
	int board_hits;
	int total_sims;
	int num_busts;
	int num_finishes;
	double payout_factor;
	double starting_bankroll;
	double total_bonus;
	double playthrough_rec_factor;
	double bet_contribution_rate;

	void slycickSim() {
		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		std::mt19937 engine(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<> dist(0, (board_size - 1));

		num_busts = 0;
		num_finishes = 0;
		final_amounts.clear();

		double min_bankroll = 0.0;
		for (int i = 0; i < (int)strat_stakes.size(); ++i) {
			min_bankroll += strat_stakes[i];
		}
		double total_required_playthrough = playthrough_rec_factor * total_bonus;

		int sim_count = 0;
		while (sim_count < total_sims) {
			double dynamic_bankroll = starting_bankroll;
			double lower_bound = starting_bankroll - min_bankroll;
			double upper_bound = starting_bankroll + min_bankroll;
			double dynamic_playthrough = 0.0;
			int roll_on_strat = 0;
			double bet;

			while ((dynamic_bankroll > 0) && (dynamic_playthrough < total_required_playthrough)) {
				int random_num = dist(engine);
				if (strat_stakes[roll_on_strat] <= (dynamic_bankroll - lower_bound)) {
					bet = strat_stakes[roll_on_strat];
				}
				else {
					bet = (dynamic_bankroll - lower_bound);
				}
				dynamic_bankroll -= bet;
				dynamic_playthrough += (bet * (bet_contribution_rate / 100.0));

				if (random_num < board_hits) { //HIT!
					dynamic_bankroll += ((payout_factor + 1) * bet);
					roll_on_strat = 0;
				}
				else { //MISS!
					++roll_on_strat;
					if (roll_on_strat == (int)strat_stakes.size()) {
						roll_on_strat = 0;
					}
				}
				if ((dynamic_bankroll >= upper_bound) || (dynamic_bankroll == lower_bound)) {
					upper_bound = dynamic_bankroll + min_bankroll;
					lower_bound = dynamic_bankroll - min_bankroll;
					if (lower_bound < 0) {
						lower_bound = 0;
					}
					roll_on_strat = 0;
				}
			} // End of simulation

			if (dynamic_bankroll <= 0.0) {
				++num_busts;
			}
			else {
				++num_finishes;
				final_amounts.push_back(dynamic_bankroll);
			}
			++sim_count;
		}
	} // End of sylcickiSim

	void getRolls() {
		cout << "Enter how many rolls your strategy can have before breaking: ";
		cin >> total_rolls;
		if (total_rolls < 0) {
			cout << "Sorry! You must enter an integer greater than zero." << endl;
			getRolls();
		}
		else {
			strat_stakes.resize(total_rolls);
		}
	}

	void getBets() {
		cout << "Please enter each bet in order starting with the first: " << endl;
		double input_bet;
		for (int i = 0; i < total_rolls; ++i) {
			cin >> input_bet;
			strat_stakes[i] = input_bet;
		}
	}

	void getPayoutFactor() {
		cout << "Enter the payout factor [__ to 1]: ";
		cin >> payout_factor;
		if ((payout_factor < 0) || (payout_factor > 35)) {
			cout << "Sorry! You must enter a payout factor greater than 0 and less than or equal to 35." << endl;
			getPayoutFactor();
		}
	}

	void getBoardHits() {
		cout << "Enter the number of winning table positions: ";
		cin >> board_hits;
		if ((board_hits < 0) || (board_hits > 38)) {
			cout << "Sorry! The number of winning table positions must be greater than 0 and less than or equal to 38." << endl;
			getBoardHits();
		} 
	}

	void getTableType() {
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
			cout << "Invalid selection: " << AmOrEur << endl;
			getTableType();
		}
	}

	void getStartingBankroll() {
		cout << "Enter the starting bankroll: ";
		cin >> starting_bankroll;
	}

	void getBonusAmount() {
		cout << "Enter the amount of the bankroll that is bonus money: ";
		cin >> total_bonus;
	}

	void getPlaythroughRequirement() {
		cout << "Enter the playthrough requirement to acquire the entire bonus [__x]: ";
		cin >> playthrough_rec_factor;
	}

	void getContributionRate() {
		cout << "Enter the contribution rate for each bet in roulette [__%]: ";
		cin >> bet_contribution_rate;
	}

	void getSimulationCount() {
		cout << "Please enter the total number of simulations you would like to run: ";
		cin >> total_sims;
	}

	void printRuntime(clock_t& t) {
		t = clock() - t;
		double seconds = (((float)t) / (CLOCKS_PER_SEC));
		double hours = (((seconds) / 60.0) / 60.0);
		seconds -= (((int)hours) * 3600);
		double minutes = ((seconds) / 60.0);
		seconds -= (((int)minutes) * 60);
		cout << "Total running time: ";
		cout << (int)hours << " Hours: ";
		cout << (int)minutes << " Minutes: ";
		cout << (int)seconds << " Seconds" << endl;
	}
	
	double getAverageFinalFinish() {
		double finish_sum = 0.0;
		for (int i = 0; i < (int)final_amounts.size(); ++i) {
			finish_sum += final_amounts[i];
		}
		return ((finish_sum) / ((double)final_amounts.size()));
	}

};

#endif
