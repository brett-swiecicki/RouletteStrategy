//Brett Swiecicki
#ifndef SOLUTION_H
#define SOLUTION_H

#include <mutex>
#include <vector>
using namespace std;

class Solution {
public:
	void change_best_stakes(vector<double>& stakes_in, double EV_sum_in) {
		std::lock_guard<std::mutex> guard(best_stakes_mutex);
		//This will have to do the checking and flag updated
		if ((stakes_in.size() > best_stakes.size()) || ((stakes_in.size() == best_stakes.size()) && (EV_sum_in > best_win_EV_sum))) {
			best_stakes = stakes_in;
			best_win_EV_sum = EV_sum_in;
			updated = true;
		}
		
	}
	double get_best_win_EV_sum() {
		return best_win_EV_sum;
	}
	size_t get_best_size() {
		return best_stakes.size();
	}
	vector<double>& reference_best_stakes() {
		return best_stakes;
	}
	void resetUpdated() {
		updated = false;
	}
	bool checkUpdated() {
		return updated;
	}
private:
	vector<double> best_stakes;
	double best_win_EV_sum = 0.0;
	std::mutex best_stakes_mutex;
	bool updated;
};

#endif