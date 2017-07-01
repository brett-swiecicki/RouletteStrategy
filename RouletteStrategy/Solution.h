//Brett Swiecicki
#ifndef SOLUTION_H
#define SOLUTION_H

#include <mutex>
#include <vector>
using namespace std;

class Solution {
public:
	void change_best_stakes(vector<double>& stakes_in) {
		std::lock_guard<std::mutex> guard(best_stakes_mutex);
		best_stakes = stakes_in;
	}
	void change_best_win_EV_sum(double EV_sum_in) {
		std::lock_guard<std::mutex> guard(best_stakes_mutex);
		best_win_EV_sum = EV_sum_in;
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
	void solutionUpdated() {
		updated = true;
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