#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <array>
#include <vector>

static constexpr int NUM_MAZE_COLS {9};                // Number of grid columns in the maze
static constexpr int NUM_MAZE_ROWS {5};                // Number of grid rows in the maze

enum class Direction : char
{
    North = 'N',
    South = 'S',
    East = 'E',
    West = 'W'
};

enum class Movement : char
{
    Forward = 'F',
    Left = 'L',
    Right = 'R'
};

class MazeMap
{
public:
    MazeMap(std::vector<std::string> mapString);
    std::vector<std::string> GetStringLines();
    void FindShortestPaths();
    void FindOptimalPath();
    void PrintMazeInfo();
private:
    struct MazeCell
    {
        bool isNorthOpen;
        bool isSouthOpen;
        bool isEastOpen;
        bool isWestOpen;
        int weight;
    };
    int startRow;
    int startColumn;
    int goalRow;
    int goalColumn;
    Direction initDirection;
    std::vector<std::string> textLines;
    std::array<std::array<MazeCell, NUM_MAZE_COLS>, NUM_MAZE_ROWS> cells;
};
