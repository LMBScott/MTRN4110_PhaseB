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
                    initDirection = Direction::North;
                    break;
                case 'v':
                    isStartCell = true;
                    initDirection = Direction::South;
                    break;
                case '<':
                    isStartCell = true;
                    initDirection = Direction::East;
                    break;
                case '>':
                    isStartCell = true;
                    initDirection = Direction::West;
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
}

std::vector<std::string> MazeMap::GetStringLines()
{
    return textLines;
}

void MazeMap::PrintMazeInfo()
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
                switch (initDirection)
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
