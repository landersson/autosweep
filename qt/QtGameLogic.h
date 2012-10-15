#ifndef _QGAME_LOGIC_H
#define _QGAME_LOGIC_H

#include "GameLogic.h"
#include <QObject>

class QtGameLogic: public QObject, public GameLogic
{
    Q_OBJECT
public:
    virtual void gameWon();
    virtual void gameLost();
signals:
    void winSignal();
    void loseSignal();
};

#endif // QGAMELOGIC_H
