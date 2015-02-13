
#ifndef _GAME_INTERFACE_H
#define _GAME_INTERFACE_H

#include "GameState.h"

#include <string>

class AutoSweep;
class SweepAction;
class MineField;

/*!
 * \class GameInterface
 * \brief Defines the interface between the Game and the Solver
 */
class GameInterface
{
public:
    // game controller methods
    virtual void startNewGame(int rows, int cols, int num_mines) = 0;
    virtual bool loadGame(const std::string& filename) = 0;

    virtual void executeAction(const SweepAction& action) = 0;

    // game state investigation methods
    virtual const MineField* getMineField() = 0;
    virtual GameState getGameState() const = 0;
    virtual int getNumMines() const = 0;

    // debug methods
    virtual void verifyState(AutoSweep *as) { }
};

#endif // _GAME_INTERFACE_H
