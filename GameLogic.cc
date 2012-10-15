
#include "GameLogic.h"
#include "utils.h"

#include <cassert>
#include <stdexcept>


GameLogic::GameLogic(): _mine_field(0), _state(GameState::STOP)
{

}

GameLogic::~GameLogic()
{
    delete _mine_field;
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

#if 0
void GameLogic::newGame(int row, int col, const Board::LocationList& lst)
{
    if (row < Board::MIN_ROW_NUM || col < Board::MIN_COLUMN_NUM
        || row > Board::MAX_ROW_NUM || col > Board::MAX_COLUMN_NUM)
        throw std::invalid_argument("invalid size of board");
    if (_num_mines < 0 || _num_mines > row * col)
        throw std::invalid_argument("invalid number of mines");
    if (_mine_field) delete _mine_field;
    this->_num_mines = lst.size();
    _mine_field = new Board(row, col, lst);
    setState(GameState::RUN);
}
#endif

bool GameLogic::loadGame(const std::vector<std::string>& rows)
{
    delete _mine_field;

    _mine_field = new MineField(2, 2, 1);
    if (!_mine_field->fromStrings(rows)) return false;

    _num_mines = 0;

    for (int i = 0; i < _mine_field->getRows(); i++)
        for (int j = 0; j < _mine_field->getCols(); j++)
            if (this->getCell(i, j).getValue() == Cell::MINE) _num_mines++;

    printf("GameLogic::loadGame() loaded %dx%dx%d game\n",
           getRows(), getCols(), _num_mines);

    setState(GameState::RUN);
    return true;
}

void GameLogic::newGame(int row, int col, int num)
{
#if 0
    if (row < 1 || col < Board::MIN_COLUMN_NUM
        || row > Board::MAX_ROW_NUM || col > Board::MAX_COLUMN_NUM)
        throw std::invalid_argument("invalid size of board");
    if (num < 0 || num > row * col)
        throw std::invalid_argument("invalid number of mines");
#endif
    if (_mine_field) delete _mine_field;
    this->_num_mines = num;
    _mine_field = new MineField(row, col, num);
    setState(GameState::RUN);
}

int GameLogic::getNumMines()
{
    return _num_mines;
}

bool GameLogic::validState(GameState v)
{
    return v == GameState::RUN || v == GameState::STOP ||
           v == GameState::WIN || v == GameState::LOSE;
}

void GameLogic::setState(GameState new_state)
{
    if (this->validState(new_state))
    {
        if (_state != new_state)
        {
            _state = new_state;
        }
    }
    else
        throw std::invalid_argument("unknown state for GameLogic: " + int2str((int)new_state));
}


std::string GameLogic::toString()
{
    if (this->getState() == GameState::RUN)
        return _mine_field->toString();
    else
        return "";
}

void GameLogic::floodFillDig(int i, int j)
{
    //    printf("GameLogic::floodFillDig(%d,%d)\n", i, j);

    if (getState() != GameState::RUN)
    {
        fprintf(stderr, "Game is not running");
        return;
    }
    else if (!_mine_field->isValidCoordinate(i, j))
    {
        fprintf(stderr, "Invalid arguments");
        return;
    }
    MineField::LocationList lst = _mine_field->getNeighbours(i, j);

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

int GameLogic::getRows()
{
    return _mine_field->getRows();
}

int GameLogic::getCols()
{
    return _mine_field->getCols();
}

MineField::LocationList GameLogic::getNeighbours(int i, int j)
{
    return this->_mine_field->getNeighbours(i, j);
}


int GameLogic::getMarkedNum()
{
    //  count one by one...
    //  maybe not efficient,
    //  but I don't want to bring trouble to later maintainence
    //  because keeping track of a counting variable in multiple functions is difficult
    if (getState() != GameState::RUN)
        return -1;
    int res = 0;
    for (int i = 0; i < this->getRows(); i++)
    {
        for (int j = 0; j < this->getCols(); j++)
        {
            if (getCell(i, j).getState() == Cell::FLAGGED)
            {
                res++;
            }
        }
    }
    return res;
}

void GameLogic::dig(int i, int j)
{
    //    printf("GameLogic::dig(%d,%d)\n", i, j);

    //  need to check win & lose after calling this function

    if (getState() != GameState::RUN)
        throw std::logic_error("Game is not running");
    else if (!_mine_field->isValidCoordinate(i, j))
        throw std::invalid_argument("Invalid arguments");

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

    bool bomb_hit = c.getValue() == Cell::MINE;

    if (this->checkWin())
    {
        this->setState(GameState::WIN);
        gameWon();
    }
    else if (bomb_hit || this->checkLose())
    {
        this->revealAll();
        this->setState(GameState::LOSE);
        gameLost();
    }
}

void GameLogic::mark(int i, int j)
{
    //    printf("GameLogic::mark(%d,%d)\n", i, j);

    //  need to check win after calling this function
    if (getState() != GameState::RUN)
        throw std::logic_error("Game is not running");
    else if (!_mine_field->isValidCoordinate(i, j))
        throw std::invalid_argument("Invalid arguments");

    Cell& c = _mine_field->getCell(i, j);
    if (c.getState() != Cell::UNKNOWN)
        fprintf(stderr, "Can only mark UNKNOWN cells\n");
    else
        c.setState(Cell::FLAGGED);

    if (this->checkWin())
    {
        this->setState(GameState::WIN);
        gameWon();;
    }
}

void GameLogic::unmark(int i, int j)
{
    if (getState() != GameState::RUN)
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

