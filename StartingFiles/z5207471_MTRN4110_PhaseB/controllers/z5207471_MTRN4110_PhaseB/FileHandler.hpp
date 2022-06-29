// File:                 FileHandler.hpp
// Date:                 10/06/2022
// Description:          Class definition for the FileHandler class designed to handle file input/output for MTRN4110 Assignment Phase A
// Author:               Lachlan Scott (z5207471)
// Development Platform: MacOS 12.4 Monterey, M1 Pro processor

#include <memory>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <array>
#include <MazeMap.hpp>

struct MotionPlan
{
    std::string line;
    int initRow;
    int initColumn;
    Direction initHeading;
    std::vector<Movement> steps;
};

class FileHandler
{
public:
    FileHandler();
    std::unique_ptr<MotionPlan> ReadMotionPlan(std::string filePath);
    std::unique_ptr<MazeMap> ReadMap(std::string filePath);
    void WriteExecutionHeader(std::string filePath);
    void WritePlanState(std::string filePath, int step, int row, int column, Direction heading, std::array<bool, 3> wallVisibility);
    void WriteLine(std::string filePath, std::string line, bool shouldAppend);
    void WriteLines(std::string filePath, std::vector<std::string> lines, bool shouldAppend);
private:
    static constexpr int PLAN_INIT_COND_LENGTH {3};
    std::regex planRegex;
    std::regex mapSeparatorRowRegex;
    std::regex mapCellRowRegex;
};
