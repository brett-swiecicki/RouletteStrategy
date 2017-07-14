//Brett Swiecicki
#ifndef OPTSIM_H
#define OPTSIM_H

#include <future>
#include <thread>

#include "ProcessorCommons.h"
#include "SimSolution.h"
#include "Simulator.h"
using namespace std;


class SimStakeFinder {
public:
	SimStakeFinder(ProcessorCommons& p_commons_in, Solution* global_sol_in)
		: osp_commons(p_commons_in), global_threads_solution(global_sol_in) 
	{
		my_local_sim = Simulator(osp_commons.board_size, osp_commons.board_hits, osp_commons.payout_factor);
	} //Overloaded constructor for local data copies

	void set_parameters_for_processing(vector<int>& starting_bets_in, int total_rolls_in) {
		starting_bets = starting_bets_in;
		osp_commons.total_rolls = total_rolls_in;
	}

	void operator()() {
		osp_commons.best_ROI = -100.0;
		osp_commons.dynamic_solution.resize(osp_commons.total_rolls);
		for (int i = 0; i < (int)starting_bets.size(); ++i) { //Loop through everything to update local solution
			solution_find_max_ROI_rec(0, 0.0, starting_bets[i]);
		}
		global_threads_solution->submit_best_solution_candidate(osp_commons.best_stakes, osp_commons.best_ROI); //Update global solution
	}
	
private:
	ProcessorCommons osp_commons;
	Simulator my_local_sim;
	Solution* global_threads_solution;
	vector<int> starting_bets;

	void solution_find_max_ROI_rec(int stake_number, double cumulative_stake, int lastBetAdded) {
		if (stake_number == osp_commons.total_rolls) {
			double dynamic_ROI = my_local_sim.getSimulationROI(osp_commons.dynamic_solution, 200000); //200,000 sims
			if (dynamic_ROI > (osp_commons.best_ROI)) {
				osp_commons.best_ROI = dynamic_ROI;
				osp_commons.best_stakes = osp_commons.dynamic_solution;
			}
			return;
		}
		for (int i = lastBetAdded; i < (int)osp_commons.possible_bets.size(); ++i) {
			osp_commons.dynamic_solution[stake_number] = osp_commons.possible_bets[i];
			bool profitable = osp_commons.checkIfProfitableNoBreakEven(osp_commons.dynamic_solution, stake_number, cumulative_stake + osp_commons.possible_bets[i]);
			if (profitable) {
				solution_find_max_ROI_rec(stake_number + 1, cumulative_stake + osp_commons.possible_bets[i], i);
			}
		}
	}
};

class OptimalSimulatorProcessor {
public:
	OptimalSimulatorProcessor(ProcessorCommons& osp_commons_in)
		: osp_commons(osp_commons_in) {}

	void find_max_ROI_solution_with_simulator() {
		osp_commons.total_rolls = 1;
		buildTasksVector();
		global_threads_solution.updated = true;
		while (global_threads_solution.updated) {
			global_threads_solution.updated = false;
			cout << "Currently computing strategies for " << osp_commons.total_rolls << " rolls.";
			findSolutionWithThreads();
			if (global_threads_solution.updated) {
				cout << " Solution found with " << global_threads_solution.get_latest_ROI() << "% ROI." << endl;
			}
			else {
				cout << endl;
			}
			++osp_commons.total_rolls;
		}
		osp_commons.all_solutions = global_threads_solution.get_all_solutions(); //Copy solutions from threads over to osp_commons
	}

private:
	ProcessorCommons& osp_commons;
	Solution global_threads_solution;
	std::vector<std::thread> threads;
	vector<SimStakeFinder> tasks_vector;

	void findSolutionWithThreads() {
		refreshStakeFinders();
		for (int q = 0; q < (int)tasks_vector.size(); ++q) {
			threads[q] = std::thread(tasks_vector[q]);
		}
		for (int i = 0; i < (int)threads.size(); ++i) {
			threads[i].join();
		}
	}

	void refreshStakeFinders() { //This will be the function that has to partition the work among each stakeFinder object
		//1. Make the starting_bets vectors for each task vector
		vector<vector<int>> starting_bet_vectors(osp_commons.num_threads);
		int vec_to_push = 0;
		for (int i = 0; i < (int)osp_commons.possible_bets.size(); ++i) {
			starting_bet_vectors[vec_to_push].push_back(i);
			++vec_to_push;
			if (vec_to_push == osp_commons.num_threads) {
				vec_to_push = 0;
			}
		}
		//2.Update the task objects in the task vector
		for (int j = 0; j < ((int)tasks_vector.size()); ++j) {
			tasks_vector[j].set_parameters_for_processing(starting_bet_vectors[j], osp_commons.total_rolls);
		}
	}

	void buildTasksVector() {
		tasks_vector.resize(osp_commons.num_threads, SimStakeFinder(osp_commons, &global_threads_solution));
		threads.resize(osp_commons.num_threads);
	}

};

/*
7/11 Update: Run every permuation through the sim and return what is best.
Everything would have to be profitable (not break even)
Should run with 100,000, then run with 1,000,000 if in a range, then maybe even run 5,000,000 if within a closer range.
The simulator also needs to account for surplus wins.
*/

#endif
