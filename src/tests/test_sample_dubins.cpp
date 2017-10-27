#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <bitset>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <random>
#include <fstream>
#include <string>

#include "trajectory/dubins_steer.h"
#include "trans_sys/spot_hoa_interpreter.h"
#include "sampling/ltl_sampling_dubins.h"
#include "stopwatch/stopwatch.h"
#include "config_reader/config_reader.h"

#include <lcm/lcm-cpp.hpp>
#include "lcmtypes/acel_lcm_msgs.hpp"

using namespace acel;

int main()
{
    // Random seed
    srand(time(NULL));
    // LCM for communication
    lcm::LCM lcm;
    // Timer
    stopwatch::StopWatch stopwatch;
    // Read the configuration file
    // ConfigReader config_reader("../../config/test_sample_dubins.ini");
    ConfigReader config_reader("../../config/test.ini");
    std::cout << config_reader << std::endl;
    /*** Set up the seaching object ***/
    LTL_SamplingDubins ltl_sampling_dubins;

    /*** Set the size of the workspace ***/
    double work_space_size_x = config_reader.GetReal("work_space_size_x", 0);
    double work_space_size_y = config_reader.GetReal("work_space_size_y", 0);
    ltl_sampling_dubins.init_workspace(work_space_size_x, work_space_size_y);
    // Publish workspace size for visualization
    sampling::workspace_size_data space_data;
    space_data.size_x = work_space_size_x;
    space_data.size_y = work_space_size_y;
    lcm.publish("WORKSPACE", &space_data);

    /*** Set all parameters ***/
    // EPSILON is the forward step size when sampling searching
    double EPSILON = config_reader.GetReal("EPSILON", 0);
    // RADIUS is the radius of checking aera when sampling searching
    double RADIUS = config_reader.GetReal("RADIUS", 0);
    // radius_L is the left minimum turning radius
    double radius_L = config_reader.GetReal("radius_L", 0);
    // radius_R is the right minimum turning radius
    double radius_R = config_reader.GetReal("radius_R", 0);
    // Set the groud speed of the aircraft
    double ground_speed = 1;

    ltl_sampling_dubins.init_parameter(EPSILON, RADIUS, radius_L, radius_R, ground_speed);

    /*** Read formula ***/
    // "(<> p0) && (<> p1) && (<> p2)" means visit p0, p1 and p2 region of interests
    // std::string ltl_formula = "(<> p0) && (<> p1) && (<> p2)";
    std::string ltl_formula = config_reader.Get("ltl_formula", "");
    // "<> (p0 && <> (p1 && (<> p2)))" means visit p0, p1 and p2 region of interests and by this order
    // std::string ltl_formula = "<> (p2 && <> (p1 && (<> p0)))";
    // Wrap all region of interests (ROI) as input for reading formula
    std::vector<std::string> buchi_regions;
    buchi_regions.push_back("p0");
    buchi_regions.push_back("p1");
    buchi_regions.push_back("p2");
    // indep_set store the ROI that independent to each other, in this case means p0, p1 and p2 have no intersections
    std::vector<int> indep_set = {0, 1, 2};
    ltl_sampling_dubins.read_formula(ltl_formula, buchi_regions, indep_set);

    /*** Set the initial state of the UAV ***/
    std::vector<double> init_state = {50, 10, M_PI/2};
    ltl_sampling_dubins.set_init_state(init_state);
    
    /*** Set region of interests ***/
    // All ROI and obstacles are rectangle for now
    // Three parameters are x position, y position and the name of ROI (0 means p0)
    std::pair <double, double> position_x (20, 35);
    std::pair <double, double> position_y (30, 45);
    ltl_sampling_dubins.set_interest_region(position_x, position_y, 0);
    // For visualization
    sampling::region_data r_data;
    r_data.position_x[0] =  position_x.first;
    r_data.position_x[1] =  position_x.second;
    r_data.position_y[0] =  position_y.first;
    r_data.position_y[1] =  position_y.second;
    lcm.publish("REGION", &r_data);

    position_x = std::make_pair(55, 95);
    position_y = std::make_pair(55, 95);
    ltl_sampling_dubins.set_interest_region(position_x, position_y, 1);
    // For visualization
    r_data.position_x[0] =  position_x.first;
    r_data.position_x[1] =  position_x.second;
    r_data.position_y[0] =  position_y.first;
    r_data.position_y[1] =  position_y.second;
    lcm.publish("REGION", &r_data);

    position_x = std::make_pair(10, 20);
    position_y = std::make_pair(80, 90);
    ltl_sampling_dubins.set_interest_region(position_x, position_y, 2);
    // For visualization
    r_data.position_x[0] =  position_x.first;
    r_data.position_x[1] =  position_x.second;
    r_data.position_y[0] =  position_y.first;
    r_data.position_y[1] =  position_y.second;
    lcm.publish("REGION", &r_data);

    /*** Set obstacles ***/
    position_x = std::make_pair(35, 62);
    position_y = std::make_pair(35, 40);
    ltl_sampling_dubins.set_obstacle(position_x, position_y);
    // For visualization
    r_data.position_x[0] =  position_x.first;
    r_data.position_x[1] =  position_x.second;
    r_data.position_y[0] =  position_y.first;
    r_data.position_y[1] =  position_y.second;
    lcm.publish("OBSTACLE", &r_data);

    position_x = std::make_pair(15, 40);
    position_y = std::make_pair(65, 70);
    ltl_sampling_dubins.set_obstacle(position_x, position_y);
    // For visualization
    r_data.position_x[0] =  position_x.first;
    r_data.position_x[1] =  position_x.second;
    r_data.position_y[0] =  position_y.first;
    r_data.position_y[1] =  position_y.second;
    lcm.publish("OBSTACLE", &r_data);
    
    /*** Set the number of iterations ***/
    // Solution towards to optimal when iterations -> infinite
    int iterations = 1500;

    /*** Start sampling searching ***/
    stopwatch.tic();
    ltl_sampling_dubins.start_sampling(iterations);
    std::cout << "Time used for searching: " << stopwatch.toc() << std::endl;

    /*** Get result ***/
    // std::vector<std::vector<double>> path = ltl_sampling_dubins.get_path();
    std::vector<WayPoint> way_points = ltl_sampling_dubins.get_waypoints();

    // Publish the trajectory and visualize the result
    sampling::path_data path_data_;
    path_data_.num_state = way_points.size();
    path_data_.state_x.resize(path_data_.num_state);
    path_data_.state_y.resize(path_data_.num_state);
    for (int i = 0; i < way_points.size(); i++) {
        path_data_.state_x[i] = way_points[i].x;
        path_data_.state_y[i] = way_points[i].y;
        // std::cout << "x: " << way_points[i].x << ", y: " << way_points[i].y << ", time: "<< way_points[i].t << std::endl;
    }
    lcm.publish("PATH", &path_data_);
    sampling::sample_draw draw;
    draw.if_draw = true;
    lcm.publish("DRAW_SAMPLE", &draw);

    // Publish the way points data as planner output
    rts3a::routePlannerOutputs_t planner_output;
    planner_output.numWaypoints = way_points.size();
    planner_output.waypoints.resize(way_points.size());
    for (int i = 0; i < way_points.size(); i++){
        rts3a::waypoint_t way_point_;
        way_point_.x = way_points[i].x;
        way_point_.y = way_points[i].y;
        way_point_.z = 0;
        way_point_.t = way_points[i].t;
        
        planner_output.waypoints[i] = way_point_;
    }
    lcm.publish("PLANNER_OUTPUT", &planner_output);
    
    return 0;
}