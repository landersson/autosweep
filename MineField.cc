
#include "MineField.h"

#include "utils.h"

#include <iostream>
#include <sstream>
#include <iomanip>

MineField::MineField(int rows, int cols, int num_mines) : Grid<Cell>(rows, cols)
{
    this->randomlyFillMines(num_mines);
    this->setupValues();
}

void MineField::resetCells()
{
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            getCell(i, j).setState(Cell::UNKNOWN);
            getCell(i, j).setValue(0);
        }
    }
}


// set up values for each cell to be consistent with mine alignment
void MineField::setupValues()
{
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            if (this->getCell(i, j).getValue() != Cell::MINE) continue;

            Neighbours lst = this->getNeighbours(i, j);

            for (size_t k = 0; k < lst.size(); k++)
            {
                Location& p = lst[k];
                Cell& c = getCell(p);

                if (c.getValue() != Cell::MINE)
                    c.setValue(c.getValue() + 1);
            }
        }
    }
}

// randomly fill in <num> mines into board
void MineField::randomlyFillMines(int num)
{
    int i, j;
    while (num--)
    {
        do
        {
            i = rand_int(0, _rows - 1);
            j = rand_int(0, _cols - 1);
        }
        while (getCell(i, j).getValue() == Cell::MINE);

        getCell(i, j).setValue(Cell::MINE);
    }
}


std::string MineField::toString() const
{
    std::stringstream res;
    res << _rows;
    res << 'x';
    res << _cols;
    res << '\n';

    res << "  ";

    for (int i = 0; i < _cols; i++)
        res << std::setw(4) << std::right << i;

    res << std::setw(0) << "\n";

    for (int i = 0; i < _rows; i++)
    {
        res << std::setw(4) << std::left << i << std::setw(0);

        for (int j = 0; j < _cols; j++)
            res << getCell(i, j).toString();
        res << "\n";
    }
    return res.str();
}


bool MineField::loadFromFile(const std::string& filename)
{
    FILE* fp = fopen(filename.c_str(), "r");

    if (fp == 0)
    {
        fprintf(stderr, "MineField: error opening file '%s'\n", filename.c_str());
        return false;
    }

    std::vector<std::string> rows;

    char buffer[512];
    while (0 != fgets(buffer, sizeof(buffer), fp))
    {
        rows.push_back(buffer);
        printf("%s", buffer);
    }

    fclose(fp);

    if (!fromStrings(rows))
    {
        fprintf(stderr, "MineField: error loading minefield from file '%s'\n", filename.c_str());
        return false;
    }

//    printf("%dx%dx%d\n", _rows, _cols, _num);

    return true;
}



bool MineField::fromStrings(const std::vector<std::string>& strings)
{
    if (strings.size() < 3) return false;

    int rows, cols;
    if (2 != sscanf(strings[0].c_str(), "%dx%d", &rows, &cols)) return false;

    _rows = rows;
    _cols = cols;

    if (strings.size() != (unsigned)(rows + 2)) return false;

    allocCells(rows, cols);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int cell_pos = 4 + j * 4;
            getCell(i, j).fromString(strings[i + 2].substr(cell_pos, 4));
        }
    }

    printf("MineField::fromStrings() size = %dx%d\n", rows, cols);

    return true;
}


void MineField::fail(const char* message) const
{
    std::cerr << message;
    exit(1);
}

