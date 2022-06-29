// File:                 FileHandler.hpp
// Date:                 10/06/2022
// Description:          Class implementation for the FileHandler class designed to handle file input/output for MTRN4110 Assignment Phase A
// Author:               Lachlan Scott (z5207471)
// Development Platform: MacOS 12.4 Monterey, M1 Pro processor

#include <FileHandler.hpp>

FileHandler::FileHandler()
{
    // Construct the motion plan regex
    std::stringstream planPatternString;
    planPatternString << "[0-" << (NUM_MAZE_ROWS - 1) << "][0-" << (NUM_MAZE_COLS - 1) << "][NSEW][FLR]*";

    planRegex = std::regex(planPatternString.str());

    // Construct regex for a separator row of a map input
    std::stringstream separatorPatternString;
    separatorPatternString << "( (-{3}| {3})){" << NUM_MAZE_COLS << "} ";
    mapSeparatorRowRegex = std::regex(separatorPatternString.str());

    // Construct regex for a cell row of a map input
    std::stringstream cellPatternString;
    cellPatternString << "\\|( [x^v<> ] [| ]){" << NUM_MAZE_COLS - 1 << "} [x^v<> ] \\|";
    mapCellRowRegex = std::regex(cellPatternString.str());
}

std::unique_ptr<MotionPlan> FileHandler::ReadMotionPlan(std::string filePath)
{
    std::ifstream fileStream(filePath, std::ios::in);

    std::string line;

    std::unique_ptr<MotionPlan> plan = nullptr;

    if (fileStream)
    {
        bool lineReadSuccess = (bool)std::getline(fileStream, line);
        fileStream.close();

        if (!lineReadSuccess)
        {
            throw std::runtime_error("FileHandler::ReadMotionPlan: File does not contain a single line.");
        }
    }
    else
    {
        throw std::runtime_error("FileHandler::ReadMotionPlan: Failed to open instruction file.");
    }

    // Ensure that plan matches the regex pattern
    if (std::regex_match(line, planRegex))
    {
        plan = std::make_unique<MotionPlan>();

        plan->line = line;

        // Extract the initial conditions of the instruction set from the line
        std::string initConds = line.substr(0, PLAN_INIT_COND_LENGTH);
        line.erase(0, PLAN_INIT_COND_LENGTH);

        plan->initRow = initConds[0] - '0';
        plan->initColumn = initConds[1] - '0';
        plan->initHeading = static_cast<Direction>(initConds[2]);

        for (char& c : line)
        {
            plan->steps.push_back(static_cast<Movement>(c));
        }
    }
    else
    {
        throw std::runtime_error("FileHandler::ReadMotionPlan: Instructions were not in correct format.");
    }

    return plan;
}

std::unique_ptr<MazeMap> FileHandler::ReadMap(std::string filePath)
{
    std::ifstream fileStream(filePath, std::ios::in);

    std::vector<std::string> mapLines {};

    std::string line;

    std::unique_ptr<MazeMap> map = nullptr;

    if (fileStream)
    {
        int numLines = 0;
        int numStartPoints = 0;
        int numGoalPoints = 0;
        int expectedLineLength = 4 * NUM_MAZE_COLS + 1; // 3 characters per column, plus separators

        while ((bool)std::getline(fileStream, line))
        {
            numLines++;

            // Remove new line character at end of each line
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

            // Check that each line contains the expected number of characters
            int lineLength = line.length();
            if (lineLength != expectedLineLength)
            {
                std::stringstream errorMsg;
                errorMsg << "FileHandler::ReadMap: Map file line "
                         << numLines
                         << " contains "
                         << line.length()
                         << " characters, expected "
                         << expectedLineLength
                         << " for a maze with "
                         << NUM_MAZE_COLS
                         << " columns.";
                throw std::runtime_error(errorMsg.str());
            }

            // Check that line matches the appropriate regex pattern
            // Even rows are cell rows
            if (numLines % 2 == 0)
            {
                if (!std::regex_match(line, mapCellRowRegex))
                {
                    throw std::runtime_error("FileHandler::ReadMap: Map file line " + std::to_string(numLines) + " contains invalid character(s) for a cell row.");
                }

                // Check row for goal or starting points
                for (int i = 0; i < lineLength; i++)
                {
                    switch (line[i])
                    {
                        case 'x':
                            numGoalPoints++;
                            break;
                        case '^':
                        case 'v':
                        case '<':
                        case '>':
                            numStartPoints++;
                            break;
                    }
                }

                if (numGoalPoints > 1)
                {
                    throw std::runtime_error("FileHandler::ReadMap: Map contains more than one goal point (\'x\')");
                }

                if (numStartPoints > 1)
                {
                    throw std::runtime_error("FileHandler::ReadMap: Map contains more than one starting point (\'^\', 'v', '<' or '>')");
                }
            }
            else if (!std::regex_match(line, mapSeparatorRowRegex))
            {
                throw std::runtime_error("FileHandler::ReadMap: Map file line " + std::to_string(numLines) + " contains invalid character(s) for a separator row.");
            }

            mapLines.push_back(line);
        }

        fileStream.close();

        // Check that the expected number of lines was read in
        int numExpectedLines = 2 * NUM_MAZE_ROWS + 1; // 1 line per row, plus separators
        if (numLines != numExpectedLines)
        {
            std::stringstream errorMsg;
            errorMsg << "FileHandler::ReadMap: Map file contains "
                     << numLines
                     << " lines, expected "
                     << numExpectedLines
                     << " for a maze with "
                     << NUM_MAZE_ROWS
                     << " rows.";
            throw std::runtime_error(errorMsg.str());
        }

        // Check that map contains at least one goal point (already checked for more than one)
        if (numGoalPoints < 1)
        {
            throw std::runtime_error("FileHandler::ReadMap: Map does not contain a goal point (\'x\')");
        }

        // Check that map contains at least one starting point (already checked for more than one)
        if (numStartPoints < 1)
        {
            throw std::runtime_error("FileHandler::ReadMap: Map does not contain a starting point (\'^\', 'v', '<' or '>')");
        }

        map = std::make_unique<MazeMap>(MazeMap(mapLines));
    }
    else
    {
        throw std::runtime_error("FileHandler::ReadMap: Failed to open map file.");
    }

    return map;
}

void FileHandler::WriteExecutionHeader(std::string filePath)
{
    std::ofstream fout {filePath, std::ios::out};

    if (fout.is_open()) 
    {
        fout << "Step,Row,Column,Heading,Left Wall,Front Wall,Right Wall" << std::endl;
        fout.close();
    } 
    else 
    {
        // Throw an error message if file failed to open
        throw std::runtime_error("Writing header line to " + filePath + " failed!");
    }
}

void FileHandler::WritePlanState(std::string filePath, int step, int row, int column, Direction heading, std::array<bool, 3> wallVisibility)
{
    std::ofstream fout {filePath, std::ios::out | std::ios::app};  // Open file in append mode

    if (fout.is_open()) 
    {
        fout << step << ','
             << row << ','
             << column << ','
             << (char)heading << ','
             << (wallVisibility[0] ? 'Y' : 'N') << ','
             << (wallVisibility[1] ? 'Y' : 'N') << ','
             << (wallVisibility[2] ? 'Y' : 'N') << std::endl;
        fout.close();
    } 
    else 
    {
        // Throw an error message if file failed to open
        throw std::runtime_error("Writing new line to " + filePath + " failed!");
    }
}

void FileHandler::WriteLine(std::string filePath, std::string line, bool shouldAppend)
{
    std::ios::openmode openMode = std::ios::out;

    if (shouldAppend)
    {
        openMode |= std::ios::app;
    }

    std::ofstream fout {filePath, openMode};

    if (fout.is_open()) 
    {
        fout << line;

        fout.close();
    } 
    else 
    {
        // Throw an error message if file failed to open
        throw std::runtime_error("Writing new line to " + filePath + " failed!");
    }
}

void FileHandler::WriteLines(std::string filePath, std::vector<std::string> lines, bool shouldAppend)
{
    std::ios::openmode openMode = std::ios::out;

    if (shouldAppend)
    {
        openMode |= std::ios::app;
    }

    std::ofstream fout {filePath, openMode};

    if (fout.is_open()) 
    {
        for (std::string line : lines)
        {
            fout << line;
        }

        fout.close();
    } 
    else 
    {
        // Throw an error message if file failed to open
        throw std::runtime_error("Writing new line to " + filePath + " failed!");
    }
}


