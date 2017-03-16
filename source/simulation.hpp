#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "param_set.hpp"
#include "model.hpp"
#include "cell_param.hpp"
#include "reaction.hpp"
#include "concentration_level.hpp"
#include "baby_cl.hpp"
#include <vector>
#include <array>
using namespace std;

/* simulation contains simulation data, partially taken from input_params and partially derived from other information
	notes:
 There should be only one instance of simulation at any time.
	todo:
 */


typedef cell_param<NUM_REACTIONS> Rates;
typedef cell_param<NUM_DELAY_REACTIONS> Delays;
typedef cell_param<NUM_CRITICAL_SPECIES> CritValues;

class simulation{
    
 public:
  // Times and timing
  RATETYPE _step_size; // The step size in minutes
  int time_total; // The number of minutes to run for
  int steps_total; // The number of time steps to simulate (total time / step size)
  int steps_split; // The number of time steps it takes for cells to split
  int steps_til_growth; // The number of time steps to wait before allowing cells to grow into the anterior PSM
  bool no_growth; // Whether or not the simulation should rerun with growth

  // Granularities
  int big_gran; // The granularity in time steps with which to analyze and store data
  int small_gran; // The granularit in time steps with which to simulate data

  // Cutoff values
  double max_con_thresh; // The maximum threshold concentrations can reach before the simulation is prematurely ended
  int max_delay_size; // The maximum number of time steps any delay in the current parameter set takes plus 1 (so that baby_cl and each mutant know how many minutes to store)
  int max_delays[NUM_SPECIES];  // The maximum number of time steps that each specie might be accessed in the past

  // Sizes
  int _width_total; // The width in cells of the PSM
  int width_initial; // The width in cells of the PSM before anterior growth
  int width_current; // The width in cells of the PSM at the current time step
  int height; // The height in cells of the PSM
  int cells; // The number of cells in the simulation
  int _cells_total; // The total number of cells of the PSM (total width * total height)

  // Neighbors and boundaries
  //array2D<int> neighbors; // An array of neighbor indices for each cell position used in 2D simulations (2-cell and 1D calculate these on the fly)
  int active_start; // The start of the active portion of the PSM
  int active_end; // The end of the active portion of the PSM

  // PSM section and section-specific times
  int section; // Posterior or anterior (sec_post or sec_ant)
  int time_start; // The start time (in time steps) of the current simulation
  int time_end; // The end time (in time steps) of the current simulation
  int time_baby; // Time 0 for baby_cl at the end of a simulation

  // Mutants and condition scores
  int num_active_mutants; // The number of mutants to simulate for each parameter set
  double max_scores[NUM_SECTIONS]; // The maximum score possible for all mutants for each testing section
  double max_score_all; // The maximum score possible for all mutants for all testing sections

  const param_set& _parameter_set;
  const model& _model;
  Rates _rates;
  Delays _delays;
  CritValues _critValues;
  Concentration_level _cl;
  baby_cl _baby_cl;
  //Context<double> _contexts;
  int _baby_j[NUM_SPECIES];
  //int* _delay_size;
  //int* _time_prev;
  int _j;
  std::vector<std::array<int, 6> > _neighbors;
  //double* _sets;
  int _NEIGHBORS_2D;
  //int* _relatedReactions[NUM_SPECIES];

    
    simulation(const model& m, const param_set& ps, int cells_total, int width_total, RATETYPE step_size) : _parameter_set(ps), _model(m), _rates(*this, cells_total), _delays(*this, cells_total), _critValues(*this, cells_total),_cl(*this), _baby_cl(*this), _cells_total(cells_total),_width_total(width_total),_neighbors(_cells_total), _step_size(step_size){
    //,_baby_j(NUM_REACTIONS), _time_prev(NUM_REACTIONS), _contexts(cells), _rates()
      _j =0 ;
      for (int i = 0; i < NUM_SPECIES; i++) {
        _baby_j[i] = 0;
      }
      _NEIGHBORS_2D = 6;
    
  }
  void test_sim();
  void execute();
  void baby_to_cl(baby_cl& baby_cl, Concentration_level& cl, int time, int* baby_times);
  void copy_records(Concentration_level& cl, int* time, int* time_prev);
  bool any_less_than_0(baby_cl& baby_cl, int* times);
  bool concentrations_too_high (baby_cl& baby_cl, int* time, double max_con_thresh);
  void calculate_delay_indices(baby_cl& baby_cl, int* baby_time, int time, int cell_index, Rates& rs, int old_cells_mrna[], int old_cells_protein[]);
  void initialize();
  void calc_neighbor_2d();
  void set_test_data();
 private:
  void calc_max_delays();
};
#endif

