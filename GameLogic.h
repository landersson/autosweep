
#ifndef _GAME_LOGIC_H
#define _GAME_LOGIC_H

#include "MineField.h"
#include "GameState.h"

class GameLogic
{
protected:
    MineField*  _mine_field;
    GameState   _state;
    int         _num_mines;        // mine number in the board

    void floodFillDig(int i, int j);
    void revealAll();
    void setState(GameState new_state);

public:
    GameLogic();
    ~GameLogic();

    void newGame(int row, int col, int num);
    bool loadGame(const std::vector<std::string>& rows);

    int getRows() ;
    int getCols() ;
    int getNumMines() ;
    int getMarkedNum() ;

    GameState getState() const;

    std::string toString();
    Neighbours getNeighbours(int i, int j);

    Cell& getCell(int i, int j);

    MineField* getMineField();

    void dig(int i, int j);
    void mark(int i, int j);
    void unmark(int i, int j);

    bool checkWin();
    bool checkLose();

    virtual void gameWon() { }
    virtual void gameLost() { }
};


//--- inline method declarations ----------------------------------------------

inline MineField* GameLogic::getMineField()
{
    return _mine_field;
}


inline int GameLogic::getRows()
{
    return _mine_field->getRows();
}

inline int GameLogic::getCols()
{
    return _mine_field->getCols();
}


#endif // GAMELOGIC_H
