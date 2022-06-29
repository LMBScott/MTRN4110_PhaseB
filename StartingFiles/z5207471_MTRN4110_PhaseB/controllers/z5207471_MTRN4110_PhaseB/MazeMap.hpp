#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <array>
#include <vector>
#include <limits>

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
    void PathFind();
    void PrintMap();
    void PrintWeightMap();
    std::vector<std::string> GetShortestPathOutput();
    std::vector<std::string> GetOptimalPathOutput();
    std::string GetOptimalPathPlan();
private:
    struct MazeCoordinates
    {
        int row;
        int column;
    };
    struct MazeStep
    {
        int row;
        int column;
        int weight;
        Direction direction;
    };
    struct MazePath
    {
        std::string planString;
        std::vector<MazeCoordinates> stepCoordinates;
    };
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
    Direction startDirection;
    std::vector<std::string> textLines;
    std::vector<MazePath> shortestPaths;
    MazePath optimalPath;
    std::array<std::array<MazeCell, NUM_MAZE_COLS>, NUM_MAZE_ROWS> cells;
    void FloodFill();
    void IteratePathFind(int shortestPathIndex, Direction currentDirection);
    std::vector<std::string> GetPathMap(MazePath path);
};
