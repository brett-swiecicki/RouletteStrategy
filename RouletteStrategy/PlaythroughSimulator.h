//Brett Swiecicki
#ifndef PLAYTHROUGHSIMULATOR_H
#define PLAYTHROUGHSIMULATOR_H

#include <iostream>
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
	int total_rolls;
	int board_size;
	int board_hits;
	int total_sims;
	double payout_factor;
	double starting_bankroll;
	double total_bonus;
	double playthrough_rec_factor;
	double bet_contribution_rate;

	void slycickSim() {

	}

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
	
};

#endif
