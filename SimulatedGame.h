
#include "GameInterface.h"
#include "GameLogic.h"

#include <cassert>

class SimulatedGame : public GameInterface
{
public:

    virtual void startNewGame(int rows, int cols, int num_mines)
    {
        //        printf("SimulatedGame::startNewGame(%d, %d, %d)\n",
        //               rows, cols, num_mines);

        _gl.newGame(rows, cols, num_mines);
    }

    virtual void executeAction(const SweepAction& action)
    {
        if (_gl.getCell(action.getRow(), action.getCol()).getState() != Cell::UNKNOWN) return;

        switch (action.getAction())
        {
        case SweepAction::FLAG_CELL:
            _gl.mark(action.getRow(), action.getCol());
            break;
        case SweepAction::CLICK_CELL:
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

protected:
    GameLogic _gl;

};
