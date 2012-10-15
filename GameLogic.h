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
    bool validState(GameState state);

    bool markedNeighboursNumberOK(int i, int j);
    void digAllUnknownNeighbours(int i, int j);

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

    std::string cheat();
    std::string toString();
    MineField::LocationList getNeighbours(int i, int j);

    Cell& getCell(int i, int j);

    MineField* getMineField()
    {
        return _mine_field;
    }

    void dig(int i, int j);
    void mark(int i, int j);
    void unmark(int i, int j);
    void explore(int i, int j);

    bool checkWin();
    bool checkLose();

    virtual void gameWon() { }
    virtual void gameLost() { }
};

#endif // GAMELOGIC_H
