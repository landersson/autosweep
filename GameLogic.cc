
#include "GameLogic.h"
#include "log.h"
#include "utils.h"

#include <cassert>
#include <stdexcept>


GameLogic::GameLogic(): _mine_field(0), _state(GameState::STOPPED), 
                        _num_mines(0), _first_move(false)
{ 
}

GameLogic::~GameLogic()
{
    delete _mine_field;
}

bool GameLogic::loadGame(const std::string& filename)
{
    delete _mine_field;

    _mine_field = new MineField(2, 2, 1);
    if (!_mine_field->loadFromFile(filename)) return false;

    _num_mines = 0;

    for (int i = 0; i < _mine_field->getRows(); i++)
        for (int j = 0; j < _mine_field->getCols(); j++)
            if (this->getCell(i, j).getValue() == Cell::MINE) _num_mines++;

    printf("GameLogic::loadGame() loaded %dx%dx%d game\n",
           getRows(), getCols(), _num_mines);

    setState(GameState::RUNNING);
    return true;
}

void GameLogic::newGame(int row, int col, int num)
{
    if (_mine_field) delete _mine_field;
    this->_num_mines = num;
    _mine_field = new MineField(row, col, num);
    _first_move = true;
    setState(GameState::RUNNING);
}

Cell& GameLogic::getCell(const Location& loc)
{
    if (!_mine_field) throw std::runtime_error("Board has not been constructed yet");
    return _mine_field->getCell(loc);
}

Cell& GameLogic::getCell(int i, int j)
{
    if (!_mine_field) throw std::runtime_error("Board has not been constructed yet");
    return _mine_field->getCell(i, j);
}

GameState GameLogic::getState() const
{
    return _state;
}

void GameLogic::setState(GameState new_state)
{    
    _state = new_state;
}

int GameLogic::getNumMines() const
{
    return _num_mines;
}

std::string GameLogic::toString()
{
    if (this->getState() == GameState::RUNNING)
        return _mine_field->toString();
    else
        return "";
}

void GameLogic::floodFillDig(int i, int j)
{
    //    printf("GameLogic::floodFillDig(%d,%d)\n", i, j);

    if (getState() != GameState::RUNNING)
    {
        fprintf(stderr, "Game is not running");
        return;
    }
    else if (!_mine_field->isValidCoordinate(i, j))
    {
        fprintf(stderr, "Invalid arguments");
        return;
    }
    Neighbours lst = _mine_field->getNeighbours(i, j);

    _mine_field->getCell(i, j).setState(Cell::KNOWN);

    for (size_t i = 0; i < lst.size(); i++)
    {
        Location& p = lst[i];
        Cell& c = _mine_field->getCell(p.row, p.col);
        if (c.getState() != Cell::UNKNOWN) continue;
        if (c.getValue() == 0)
            floodFillDig(p.row, p.col);
        else
            dig(p.row, p.col);
    }
}

void GameLogic::revealAll()
{
    for (int i = 0; i < _mine_field->getRows(); i++)
        for (int j = 0; j < _mine_field->getCols(); j++)
            this->getCell(i, j).setState(Cell::KNOWN);
}

Neighbours GameLogic::getNeighbours(int i, int j)
{
    return this->_mine_field->getNeighbours(i, j);
}


void GameLogic::dig(int i, int j)
{
    Log::print(Log::DEBUG, "GameLogic::dig(%d,%d)\n", i, j);

    //  need to check win & lose after calling this function

    if (getState() != GameState::RUNNING)
        throw std::logic_error("Game is not running");
    else if (!_mine_field->isValidCoordinate(i, j))
        throw std::invalid_argument("Invalid arguments");

    bool bomb_hit = false;

    while (true)
    {
        bomb_hit = _mine_field->getCell(i, j).getValue() == Cell::MINE;

        if (bomb_hit)
        {
            if (_first_move)
            {
                _mine_field->randomlyFillMines(_num_mines);
                continue;
            }

            Log::print(Log::VERBOSE, "GameLogic::dig(%d,%d) BOOOM!!!\n", i, j);
        }
        break;
    }
    
    Cell& c = _mine_field->getCell(i, j);

    if (c.getState() != Cell::UNKNOWN)
        fprintf(stderr, "Can only dig UNKNOWN cells\n");
    else
    {
        if (c.getValue() == 0)
            this->floodFillDig(i, j);
        else
            c.setState(Cell::KNOWN);
    }
    
    if (this->checkWin())
    {
        this->setState(GameState::WON);
        gameWon();
    }
    else if (bomb_hit) // || this->checkLose())
    {
        this->revealAll();
        this->setState(GameState::LOST);
        gameLost();
    }

    _first_move = false;
}

void GameLogic::mark(int i, int j)
{
//    printf("GameLogic::mark(%d,%d)\n", i, j);

    //  need to check win after calling this function
    if (getState() != GameState::RUNNING)
        throw std::logic_error("Game is not running");
    else if (!_mine_field->isValidCoordinate(i, j))
        throw std::invalid_argument("Invalid arguments");

    Cell& c = _mine_field->getCell(i, j);
    if (c.getState() != Cell::UNKNOWN)
    {
        fprintf(stderr, "Can only mark UNKNOWN cells\n");
        exit(1);
    }
    else
    {
        if (c.getValue() != Cell::MINE ||
            c.getState() == Cell::FLAGGED)
        {
            fprintf(stderr, "AUTO SWEEP MEGA ERROR 0x433211: Attempted to flag a cell with no mine!\n");
            exit(1);
        }
        c.setState(Cell::FLAGGED);
    }

    if (this->checkWin())
    {
        this->setState(GameState::WON);
        gameWon();;
    }
}

void GameLogic::unmark(int i, int j)
{
    if (getState() != GameState::RUNNING)
        throw std::logic_error("Game is not running");
    else if (!_mine_field->isValidCoordinate(i, j))
        throw std::invalid_argument("Invalid arguments");

    Cell& c = _mine_field->getCell(i, j);
    if (c.getState() != Cell::FLAGGED)
        fprintf(stderr, "Can only unmark FLAGGED cells\n");
    else
        c.setState(Cell::UNKNOWN);

}

bool GameLogic::checkWin()
{
    //  all mine are FLAGGED, the rest are KNOWN
    if (!_mine_field) return false;
    for (int i = 0; i < _mine_field->getRows(); i++)
    {
        for (int j = 0; j < _mine_field->getCols(); j++)
        {
            const Cell& c = _mine_field->getCell(i, j);
            if (c.getValue() == Cell::MINE)
            {
                if (c.getState() != Cell::FLAGGED)
                    return false;
            }
            else
            {
                if (c.getState() != Cell::KNOWN)
                    return false;
            }
        }
    }
    return true;
}

bool GameLogic::checkLose()
{
    //  some MINE is KNOWN
    if (!_mine_field) return false;
    for (int i = 0; i < _mine_field->getRows(); i++)
    {
        for (int j = 0; j < _mine_field->getCols(); j++)
        {
            const Cell& c = _mine_field->getCell(i, j);
            if (c.getValue() == Cell::MINE
                && c.getState() == Cell::KNOWN)
            {
                //                printf("Pang! at %d,%d\n", i, j);
                return true;
            }
        }
    }
    return false;
}

