
#include "QtGameView.h"

//#include "AutoSweep.h"

void QtGameView::executeAction(const SweepAction& action)
{
    
    if (_main_window.getMineField()->getCell(action.getRow(), action.getCol()).getState() !=
        Cell::UNKNOWN)
    {
        return;
    }
    
    pauseGui(-1);

    switch (action.getAction())
    {
    case SweepAction::FLAG_CELL:
        flagCell(action);
        break;
    case SweepAction::CLICK_CELL:
        clickCell(action);
        break;
    default:
        assert(0);
    }
    
}



void QtGameView::verifyState(AutoSweep *as)
{
    printf("VERIFY STATE!!\n");
#if 0
    if (as->getEdges().size() != 0)
    {
        _main_window.clearTags();

        int tag = 1;
        for (MineField::LocationList locations : as->getEdges())
        {
            _main_window.tagCells(locations, tag);
            tag++;
        }
    }
#endif
    _main_window.updateGUI();
    pauseGui(-1);
    
}
