// File:                 EPuck.cpp
// Date:                 24/06/2022
// Description:          Class implementation for the MazeMap class used to solve mazes in MTRN4110 Assignment Phase B
// Author:               Lachlan Scott (z5207471)
// Development Platform: MacOS 12.4 Monterey, M1 Pro processor

#include <MazeMap.hpp>

MazeMap::MazeMap(std::vector<std::string> mapString)
{
    textLines = mapString;

    for (int i = 0; i < NUM_MAZE_ROWS; i++)
    {
        for (int j = 0; j < NUM_MAZE_COLS; j++)
        {
            bool isStartCell = false;
            char centerChar = textLines[2 * i + 1][4 * j + 2];

            switch (centerChar)
            {
                case '^':
                    isStartCell = true;
                    startDirection = Direction::North;
                    break;
                case 'v':
                    isStartCell = true;
                    startDirection = Direction::South;
                    break;
                case '>':
                    isStartCell = true;
                    startDirection = Direction::East;
                    break;
                case '<':
                    isStartCell = true;
                    startDirection = Direction::West;
                    break;
                case 'x':
                    goalRow = i;
                    goalColumn = j;
            }

            if (isStartCell)
            {
                startRow = i;
                startColumn = j;
            }

            // A cell border is open if it contains a space character at its center
            cells[i][j] =
                {
                    .isNorthOpen = (textLines[2 * i][4 * j + 2] == ' '),
                    .isSouthOpen = (textLines[2 * i + 2][4 * j + 2] == ' '),
                    .isEastOpen = (textLines[2 * i + 1][4 * j + 4] == ' '),
                    .isWestOpen = (textLines[2 * i + 1][4 * j] == ' '),
                    .weight = -1
                };
        }
    }

    FloodFill();
}

std::vector<std::string> MazeMap::GetStringLines()
{
    return textLines;
}

void MazeMap::FloodFill()
{
    // Goal cell is assigned zero weight
    cells[goalRow][goalColumn].weight = 0;

    std::vector<MazeCoordinates> queue {};
    queue.push_back(MazeCoordinates{ .row = goalRow, .column = goalColumn });

    // Iterate through all reachable cells and assign each a weight equal to the number of 
    // orthogonal movements required to reach the goal cell
    while (queue.size() > 0)
    {
        MazeCoordinates currCoords = queue.front();
        queue.erase(queue.begin());

        MazeCell currCell = cells[currCoords.row][currCoords.column];

        int minAdjacentWeight = std::numeric_limits<int>::max();

        if (currCell.isNorthOpen)
        {
            int northRow = currCoords.row - 1;
            int northCellWeight = cells[northRow][currCoords.column].weight;

            if (northCellWeight == -1)
            {
                queue.push_back(MazeCoordinates{ .row = northRow, .column = currCoords.column });
            }
            else
            {
                minAdjacentWeight = std::min(northCellWeight, minAdjacentWeight);
            }
        }

        if (currCell.isSouthOpen)
        {
            int southRow = currCoords.row + 1;
            int southCellWeight = cells[southRow][currCoords.column].weight;

            if (southCellWeight == -1)
            {
                queue.push_back(MazeCoordinates{ .row = southRow, .column = currCoords.column });
            }
            else
            {
                minAdjacentWeight = std::min(southCellWeight, minAdjacentWeight);
            }
        }

        if (currCell.isEastOpen)
        {
            int eastColumn = currCoords.column + 1;
            int eastCellWeight = cells[currCoords.row][eastColumn].weight;

            if (eastCellWeight == -1)
            {
                queue.push_back(MazeCoordinates{ .row = currCoords.row, .column = eastColumn });
            }
            else
            {
                minAdjacentWeight = std::min(eastCellWeight, minAdjacentWeight);
            }
        }

        if (currCell.isWestOpen)
        {
            int westColumn = currCoords.column - 1;
            int westCellWeight = cells[currCoords.row][westColumn].weight;

            if (westCellWeight == -1)
            {
                queue.push_back(MazeCoordinates{ .row = currCoords.row, .column = westColumn });
            }
            else
            {
                minAdjacentWeight = std::min(westCellWeight, minAdjacentWeight);
            }
        }

        if (currCell.weight == -1)
        {
            cells[currCoords.row][currCoords.column].weight = minAdjacentWeight + 1;
        }
    }
}

void MazeMap::PrintMap()
{
    for (int i = 0; i < NUM_MAZE_ROWS; i++)
    {
        std::stringstream row;
        std::stringstream rowNorthSide;
        std::stringstream rowSouthSide;

        for (int j = 0; j < NUM_MAZE_COLS; j++)
        {
            MazeCell cell = cells[i][j];

            std::string northSide = cell.isNorthOpen ? "   " : "---";
            std::string southSide = cell.isSouthOpen ? "   " : "---";
            char westSide = cell.isWestOpen ? ' ' : '|';
            char center = ' ';

            if (i == startRow && j == startColumn)
            {
                switch (startDirection)
                {
                    case Direction::North:
                        center = '^';
                        break;
                    case Direction::South:
                        center = 'v';
                        break;
                    case Direction::East:
                        center = '>';
                        break;
                    case Direction::West:
                        center = '<';
                        break;
                }
            }
            else if (i == goalRow && j == goalColumn)
            {
                center = 'x';
            }

            rowNorthSide << ' ' << northSide;
            row << westSide << ' ' << center << ' '; 
            rowSouthSide << ' ' << southSide;

            if (j == NUM_MAZE_COLS - 1)
            {
                // Print the cell's East side if it's the last cell in the row
                char eastSide = cell.isEastOpen ? ' ' : '|';
                row << eastSide;
            }
        }

        if (i == 0)
        {
            std::cout << rowNorthSide.str() << ' ' << std::endl;
        }

        std::cout << row.str() << std::endl;
        std::cout << rowSouthSide.str() << ' ' << std::endl; // Add an extra space at end of row's South side
    }
}

void MazeMap::PathFind()
{
    shortestPaths.clear();

    std::string planString = std::to_string(startRow) + std::to_string(startColumn) + (char)startDirection;
    std::vector<MazeCoordinates> stepCoordinates = { MazeCoordinates{ .row = startRow, .column = startColumn } };

    shortestPaths.push_back(MazePath{ .planString = planString, .stepCoordinates = stepCoordinates });
    IteratePathFind(0, startDirection);

    // Find optimal shortest path, which is the path with the fewest plan steps
    int minStepCount = std::numeric_limits<int>::max();
    int optimalPathIndex = 0;

    for (int i = 0; i < shortestPaths.size(); i++)
    {
        if (shortestPaths[i].planString.length() < minStepCount)
        {
            minStepCount = shortestPaths[i].planString.length();
            optimalPathIndex = i;
        }
    }

    optimalPath = shortestPaths[optimalPathIndex];
}

void MazeMap::IteratePathFind(int shortestPathIndex, Direction currentDirection)
{
    MazePath currPath = shortestPaths[shortestPathIndex];
    MazeCoordinates currCoords = currPath.stepCoordinates.back();

    if (currCoords.row == goalRow && currCoords.column == goalColumn)
    {
        return;
    }

    MazeCell currCell = cells[currCoords.row][currCoords.column];

    bool hasFoundPath = false;

    if (currCell.isNorthOpen)
    {
        int northRow = currCoords.row - 1;
        int northCellWeight = cells[northRow][currCoords.column].weight;

        if (northCellWeight < currCell.weight)
        {
            hasFoundPath = true;

            switch (currentDirection)
            {
                case Direction::South:
                    shortestPaths[shortestPathIndex].planString += (char)Movement::Left;
                    shortestPaths[shortestPathIndex].planString += (char)Movement::Left;
                    break;
                case Direction::East:
                    shortestPaths[shortestPathIndex].planString += (char)Movement::Left;
                    break;
                case Direction::West:
                    shortestPaths[shortestPathIndex].planString += (char)Movement::Right;
                    break;
                case Direction::North:
                default:
                    break;
            }

            shortestPaths[shortestPathIndex].planString += (char)Movement::Forward;

            shortestPaths[shortestPathIndex].stepCoordinates.push_back(MazeCoordinates
                { 
                    .row = northRow, 
                    .column = currCoords.column
                });

            IteratePathFind(shortestPathIndex, Direction::North);
        }
    }

    if (currCell.isSouthOpen)
    {
        int southRow = currCoords.row + 1;
        int southCellWeight = cells[southRow][currCoords.column].weight;

        if (southCellWeight < currCell.weight)
        {
            int nextPathIndex = shortestPathIndex;

            if (hasFoundPath)
            {
                // Already found a path from this cell, create a new shortest path for this branch
                nextPathIndex = shortestPaths.size();
                MazePath newPath = currPath;
                shortestPaths.push_back(newPath);
            }

            hasFoundPath = true;

            switch (currentDirection)
            {
                case Direction::North:
                    shortestPaths[nextPathIndex].planString += (char)Movement::Left;
                    shortestPaths[nextPathIndex].planString += (char)Movement::Left;
                    break;
                case Direction::East:
                    shortestPaths[nextPathIndex].planString += (char)Movement::Right;
                    break;
                case Direction::West:
                    shortestPaths[nextPathIndex].planString += (char)Movement::Left;
                    break;
                case Direction::South:
                default:
                    break;
            }

            shortestPaths[nextPathIndex].planString += (char)Movement::Forward;

            shortestPaths[nextPathIndex].stepCoordinates.push_back(MazeCoordinates
                { 
                    .row = southRow, 
                    .column = currCoords.column
                });

            IteratePathFind(nextPathIndex, Direction::South);
        }
    }

    if (currCell.isEastOpen)
    {
        int eastColumn = currCoords.column + 1;
        int eastCellWeight = cells[currCoords.row][eastColumn].weight;

        if (eastCellWeight < currCell.weight)
        {
            int nextPathIndex = shortestPathIndex;

            if (hasFoundPath)
            {
                // Already found a path from this cell, create a new shortest path for this branch
                nextPathIndex = shortestPaths.size();
                MazePath newPath = currPath;
                shortestPaths.push_back(newPath);
            }

            hasFoundPath = true;

            switch (currentDirection)
            {
                case Direction::North:
                    shortestPaths[nextPathIndex].planString += (char)Movement::Right;
                    break;
                case Direction::South:
                    shortestPaths[nextPathIndex].planString += (char)Movement::Left;
                    break;
                case Direction::West:
                    shortestPaths[nextPathIndex].planString += (char)Movement::Left;
                    shortestPaths[nextPathIndex].planString += (char)Movement::Left;
                    break;
                case Direction::East:
                default:
                    break;
            }

            shortestPaths[nextPathIndex].planString += (char)Movement::Forward;

            shortestPaths[nextPathIndex].stepCoordinates.push_back(MazeCoordinates
                { 
                    .row = currCoords.row, 
                    .column = eastColumn
                });

            IteratePathFind(nextPathIndex, Direction::East);
        }
    }

    if (currCell.isWestOpen)
    {
        int westColumn = currCoords.column - 1;
        int westCellWeight = cells[currCoords.row][westColumn].weight;

        if (westCellWeight < currCell.weight)
        {
            int nextPathIndex = shortestPathIndex;

            if (hasFoundPath)
            {
                // Already found a path from this cell, create a new shortest path for this branch
                nextPathIndex = shortestPaths.size();
                MazePath newPath = currPath;
                shortestPaths.push_back(newPath);
            }

            switch (currentDirection)
            {
                case Direction::North:
                    shortestPaths[nextPathIndex].planString += (char)Movement::Left;
                    break;
                case Direction::South:
                    shortestPaths[nextPathIndex].planString += (char)Movement::Right;
                    break;
                case Direction::East:
                    shortestPaths[nextPathIndex].planString += (char)Movement::Left;
                    shortestPaths[nextPathIndex].planString += (char)Movement::Left;
                    break;
                case Direction::West:
                default:
                    break;
            }

            shortestPaths[nextPathIndex].planString += (char)Movement::Forward;

            shortestPaths[nextPathIndex].stepCoordinates.push_back(MazeCoordinates
                { 
                    .row = currCoords.row, 
                    .column = westColumn
                });

            IteratePathFind(nextPathIndex, Direction::West);
        }
    }
}

void MazeMap::PrintWeightMap()
{
    for (int i = 0; i < NUM_MAZE_ROWS; i++)
    {
        std::stringstream row;
        std::stringstream rowNorthSide;
        std::stringstream rowSouthSide;

        for (int j = 0; j < NUM_MAZE_COLS; j++)
        {
            MazeCell cell = cells[i][j];
            int cellWeight = cell.weight;

            std::string northSide = cell.isNorthOpen ? "   " : "---";
            std::string southSide = cell.isSouthOpen ? "   " : "---";
            char westSide = cell.isWestOpen ? ' ' : '|';
            std::string center = "   ";

            // If cell has a weight, print it in the center of the cell, right-aligned
            if (cellWeight != -1)
            {
                // Three digit weight
                if (cellWeight > 99)
                {
                    center = std::to_string(cellWeight);
                }
                // Two digit weight
                else if (cellWeight > 9)
                {
                    center = ' ' + std::to_string(cellWeight);
                }
                // One digit weight
                else
                {
                    center = ' ' + std::to_string(cellWeight) + ' ';
                }
            }

            rowNorthSide << ' ' << northSide;
            row << westSide << center; 
            rowSouthSide << ' ' << southSide;

            if (j == NUM_MAZE_COLS - 1)
            {
                // Print the cell's East side if it's the last cell in the row
                char eastSide = cell.isEastOpen ? ' ' : '|';
                row << eastSide;
            }
        }

        if (i == 0)
        {
            std::cout << rowNorthSide.str() << ' ' << std::endl;
        }

        std::cout << row.str() << std::endl;
        std::cout << rowSouthSide.str() << ' ' << std::endl; // Add an extra space at end of row's South side
    }
}

std::vector<std::string> MazeMap::GetPathMap(MazeMap::MazePath path)
{
    std::vector<std::string> output;

    for (int i = 0; i < NUM_MAZE_ROWS; i++)
    {
        std::stringstream row;
        std::stringstream rowNorthSide;
        std::stringstream rowSouthSide;

        for (int j = 0; j < NUM_MAZE_COLS; j++)
        {
            MazeCell cell = cells[i][j];
            int cellWeight = -1;

            // Print cell's weight (distance to goal) if it is part of the path
            for (int k = 0; k < path.stepCoordinates.size(); k++)
            {
                if (path.stepCoordinates[k].row == i && path.stepCoordinates[k].column == j)
                {
                    cellWeight = cells[i][j].weight;
                }
            }

            std::string northSide = cell.isNorthOpen ? "   " : "---";
            std::string southSide = cell.isSouthOpen ? "   " : "---";
            char westSide = cell.isWestOpen ? ' ' : '|';
            std::string center = "   ";

            if (i == startRow && j == startColumn)
            {
                switch (startDirection)
                {
                    case Direction::North:
                        center = " ^ ";
                        break;
                    case Direction::South:
                        center = " v ";
                        break;
                    case Direction::East:
                        center = " > ";
                        break;
                    case Direction::West:
                        center = " < ";
                        break;
                }
            }
            // If cell has a weight, print it in the center of the cell, right-aligned
            else if (cellWeight != -1)
            {
                // Three digit weight
                if (cellWeight > 99)
                {
                    center = std::to_string(cellWeight);
                }
                // Two digit weight
                else if (cellWeight > 9)
                {
                    center = ' ' + std::to_string(cellWeight);
                }
                // One digit weight
                else
                {
                    center = ' ' + std::to_string(cellWeight) + ' ';
                }
            }

            rowNorthSide << ' ' << northSide;
            row << westSide << center; 
            rowSouthSide << ' ' << southSide;

            if (j == NUM_MAZE_COLS - 1)
            {
                // Print the cell's East side if it's the last cell in the row
                char eastSide = cell.isEastOpen ? ' ' : '|';
                row << eastSide;
            }
        }

        if (i == 0)
        {
            rowNorthSide << ' ' << std::endl;
            output.push_back(rowNorthSide.str());
        }

        row << std::endl;
        rowSouthSide << ' ' << std::endl; // Add an extra space at end of row's South side

        output.push_back(row.str());
        output.push_back(rowSouthSide.str());
    }

    return output;
}

std::vector<std::string> MazeMap::GetShortestPathOutput()
{
    std::vector<std::string> output;

    for (int i = 0; i < shortestPaths.size(); i++)
    {
        output.push_back("Path - " + std::to_string(i + 1) + ":\n");
        std::vector<std::string> pathMap = GetPathMap(shortestPaths[i]);
        output.insert(output.end(), pathMap.begin(), pathMap.end());
    }

    output.push_back(std::to_string(shortestPaths.size()) + " shortest paths found!\n");

    return output;
}

std::vector<std::string> MazeMap::GetOptimalPathOutput()
{
    std::vector<std::string> output = GetPathMap(optimalPath);

    output.push_back("Shortest path with least turns found!\n");
    output.push_back("Path Plan (" + std::to_string(optimalPath.planString.size() - 3) + " steps): " + optimalPath.planString + '\n');

    return output;
}

std::string MazeMap::GetOptimalPathPlan()
{
    return optimalPath.planString;
}
