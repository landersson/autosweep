
#include "QtGameLogic.h"

void QtGameLogic::gameWon()
{
    emit this->winSignal();
}

void QtGameLogic::gameLost()
{
    emit this->loseSignal();
}
