
#ifndef _SIMULATED_GAME_H
#define _SIMULATED_GAME_H

#include "GameInterface.h"
#include "GameLogic.h"

#include "log.h"

#include <cassert>

class SimulatedGame : public GameInterface
{
public:        
    virtual void startNewGame(int rows, int cols, int num_mines)
    {
        _gl.newGame(rows, cols, num_mines);
    }

    virtual bool loadGame(const std::string& filename)
    {
        return _gl.loadGame(filename);
    }

    virtual void executeAction(const SweepAction& action)
    {
        if (_gl.getCell(action.getLocation()).getState() != Cell::UNKNOWN) return;

        switch (action.getAction())
        {
        case SweepAction::FLAG_CELL:
            _gl.mark(action.getRow(), action.getCol());
            break;
        case SweepAction::CLICK_CELL:
            verifyClick(action);
            _gl.dig(action.getRow(), action.getCol());
            break;
        default:
            assert(0);
        }
    }

    virtual const MineField* getMineField()
    {
        return _gl.getMineField();
    }

    virtual GameState getGameState() const
    {
        return _gl.getState();
    }

    virtual int getNumMines() const
    {
        return _gl.getNumMines();
    }



protected:
    void verifyClick(const SweepAction& action)
    {
        if ((_gl.getCell(action.getLocation()).getValue() == Cell::MINE) &&
            action.isCertain())
        {
            Log::print(Log::ERROR, "FATAL SOLVER ERROR: certain click at %d,%d is a mine!\n",
                       action.getRow(), action.getCol());
            exit(1);
        }
    }

    GameLogic _gl;

};

#endif // _SIMULATED_GAME_H
