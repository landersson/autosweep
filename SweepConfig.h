
#ifndef SWEEP_CONFIG_H_
#define SWEEP_CONFIG_H_

#include "utils.h"
#include "Config.h"

#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#include <stdexcept>


// minesweeper tailored Config subclass
class SweepConfig
{
public:
    // initialize default sweep configuration
    SweepConfig() :
        _rows(16),
        _cols(16),
        _num_mines(52),
        _num_games(1),
        _safe_open(false)
    { }

    bool initialize(const Configuration & config)
    {
        _rows = config.getInteger("rows", _rows);
        _cols = config.getInteger("cols", _cols);
        _num_mines = config.getInteger("mines", _num_mines);
        _num_games = config.getInteger("games", _num_games);
        _safe_open = config.getBoolean("safe-open", _safe_open);
        return true;
    }
    
    int numRows() const  { return _rows; }
    int numCols() const  { return _cols; }
    int numMines() const { return _num_mines; }
    int numGames() const { return _num_games; }

    bool safeOpen() const { return _safe_open; }

private:
    int _rows;
    int _cols;
    int _num_mines;
    int _num_games;
    bool _safe_open;
};

#endif // SWEEP_CONFIG_H_
