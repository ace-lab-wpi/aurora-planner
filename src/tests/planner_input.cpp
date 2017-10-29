#include <lcm/lcm-cpp.hpp>
#include "lcmtypes/acel_lcm_msgs.hpp"

int main(int argc, char ** argv)
{
    lcm::LCM lcm;
    if(!lcm.good())
        return 1;
    rts3a::routePlannerInputs_t input_data;
    input_data.groundSpeed = 1;
    input_data.minimumTurnRadius_meters = 15;

    lcm.publish("PLANNER_INPUT", &input_data);

    return 0;
}
