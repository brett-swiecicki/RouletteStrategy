//Brett Swiecicki
#ifndef SHAREDPARAMETERS_H
#define SHAREDPARAMETERS_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <thread>
#include <mutex>
using namespace std;

struct SharedParameters {
	//FIXED parameters only.
	vector<double> possible_bets;
	double max_bet;
	double min_bet;
	double cumulative_stake_starting;
	int payout_factor;
	//int total_rolls;
	int starting_stake;
	int lastBetAdded_starting;
	int board_hits;
	int board_size;
	bool allowBreakEven;
};

#endif
