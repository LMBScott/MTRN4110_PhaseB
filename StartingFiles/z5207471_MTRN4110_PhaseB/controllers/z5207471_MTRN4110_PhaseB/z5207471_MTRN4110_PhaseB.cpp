// File:                 z5207471_MTRN4110_PhaseA.cpp
// Date:                 24/06/2022
// Description:          Main file for the execution of tasks required for Phase B of the MTRN4110 assignment project
// Author:               Lachlan Scott (z5207471)
// Development Platform: MacOS 12.4 Monterey, M1 Pro processor

#include <EPuck.hpp>

const std::string MOTION_PLAN_FILE_NAME = "../../MotionPlan.txt";
const std::string MOTION_EXECUTION_FILE_NAME = "../../MotionExecution.csv";
const std::string MAP_INPUT_FILE_NAME = "../../Map.txt";
const std::string MAP_OUTPUT_FILE_NAME = "../../Output.txt";
const std::string PATH_PLAN_OUTPUT_FILE_NAME = "../../PathPlan.txt";

int main(int argc, char **argv)
{
    EPuck ePuck {};

    ePuck.SetOutputFile(MAP_OUTPUT_FILE_NAME);
    ePuck.SetPathPlanOutputFile(PATH_PLAN_OUTPUT_FILE_NAME);
    ePuck.ReadMap(MAP_INPUT_FILE_NAME);
    ePuck.SolveMap();

    return 0;
}
