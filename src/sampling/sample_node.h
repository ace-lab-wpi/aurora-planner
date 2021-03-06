// #ifndef SRC_SAMPLE_NODE_H_
// #define SRC_SAMPLE_NODE_H_
#include <map>
#include <vector>
#include <utility>
// #include "trajectory/dubins_steer.h"
#include "trajectory/dubins_path.h"
#include "sampling/region.h"


class SampleNode {
public:
    SampleNode(){};
    SampleNode(uint64_t id, std::vector<double> states);
    ~SampleNode(){};

private:
    std::vector<double> state_;
    uint64_t id_;
    int ba_state_;
    double cost_;
    uint64_t parent_id_;
    int parent_ba_;
    std::vector<std::pair<int, uint64_t>> children_;
    std::vector<std::vector<double>> traj_point_wise_;
    DubinsPath::PathData traj_data_;

public:
    std::vector<double> get_state();
    void set_state(std::vector<double> state);

    uint64_t get_id();
    void set_id(uint64_t id);

    int get_ba();
    void set_ba(int ba_id);

    double get_cost();
    void set_cost(double cost);

    uint64_t get_parent_id();
    void set_parent_id(uint64_t parent_id);

    int get_parent_ba();
    void set_parent_ba(int parent_ba);
    
    std::vector<std::vector<double>> get_traj();
    void set_traj(std::vector<std::vector<double>> traj);

    void set_traj_data(DubinsPath::PathData traj_data);
    DubinsPath::PathData get_traj_data();

    std::vector<std::pair<int, uint64_t>>& get_children_id();
    void set_children_id(std::vector<std::pair<int, uint64_t>> children);
    void add_children_id(std::pair<int, uint64_t> one_children);

};


class SubSampleSpace {
public:
    SubSampleSpace(){};
    ~SubSampleSpace(){};
private:
    std::vector<SampleNode> sample_nodes_;
    int ba_state_;
    double step_ = 0.1;
    std::map<int, SampleNode> sample_node_id_map_; 
    double get_dist(std::vector<double> states_1, std::vector<double> states_2);
    double get_dist_dubins(std::vector<double> states_1, std::vector<double> states_2, double min_radius);
public:
    void insert_sample(SampleNode new_sample);
    SampleNode& get_sample(uint64_t id);
    std::vector<SampleNode>& get_all_samples();
    SampleNode& get_min_cost_sample();
    int num_samples();
    int get_ba_state();
    SampleNode& get_parent(std::vector<double> state);
    SampleNode& get_parent_dubins(std::vector<double> state, double min_radius);
    SampleNode& rechoose_parent(SampleNode parent_sample, std::vector<double> state, std::vector<Region> obstacles, double RADIUS);
    SampleNode& rechoose_parent_dubins(SampleNode parent_sample, std::vector<double> state, DubinsPath::PathData& dubins_steer_data, std::vector<Region> obstacles, double work_space_size_x, double work_space_size_y, double RADIUS, double min_radius, double path_step, double collision_check_rate);

};
