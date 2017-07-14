//Brett Swiecicki
#ifndef SIMSOLUTION_H
#define SIMSOLUTION_H

#include <mutex>
#include <vector>
using namespace std;

class Solution {
public:
	void submit_best_solution_candidate(vector<double>& solution_in, double ROI_in) {
		std::lock_guard<std::mutex> guard(best_stakes_mutex);
		if (ROI_in != -100.0) {
			if (!(best_stakes.empty())) {
				if (best_stakes.back().size() == solution_in.size()) {
					//Need to compare ROIs
					if (ROI_in > best_ROIs.back()) {
						best_ROIs.back() = ROI_in;
						best_stakes.back() = solution_in;
						updated = true;
						if (all_time_best_ROI < ROI_in) {
							all_time_best_ROI = ROI_in;
						}
						else if (ROI_in < all_time_best_ROI - 1.0) {
							updated = false;
						}
					}
				}
				else { //First solution for this roll count so have to do another push_back
					best_stakes.push_back(solution_in);
					best_ROIs.push_back(ROI_in);
					if (all_time_best_ROI <= ROI_in) {
						all_time_best_ROI = ROI_in;
					}
					updated = true;
				}
			}
			else { //Empty and new solution so set it
				best_stakes.push_back(solution_in);
				best_ROIs.push_back(ROI_in);
				all_time_best_ROI = ROI_in;
				updated = true;
			}
		}
		else {
			updated = false;
		}
		//Flag if updated, stop updating when latest best ROI is 1% less than the absolute best
	}
	vector<vector<double>>& get_all_solutions() {
		return best_stakes;
	}

	double get_latest_ROI() {
		return best_ROIs.back();
	}
	bool updated;
private:
	vector<vector<double>> best_stakes;
	vector<double> best_ROIs;
	std::mutex best_stakes_mutex;
	double all_time_best_ROI;
};

#endif
