
#ifndef _GAME_INTERFACE_H
#define _GAME_INTERFACE_H

#include "GameState.h"

class SweepAction;
class MineField;

class GameInterface
{
public:
    // controller methods
    virtual void startNewGame(int rows, int cols, int num_mines) = 0;
    virtual void executeAction(const SweepAction& action) = 0;

    // interpreter methods
    virtual const MineField* getMineField() = 0;
    virtual GameState getGameState() const = 0;

};

#endif // _GAME_INTERFACE_H
