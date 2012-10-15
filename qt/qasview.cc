
#include "SweepMain.h"
#include "AutoSweep.h"
#include "QtGameView.h"

#include <QApplication>

int main(int argc, char** argv)
{
    srand(4);

    QApplication app(argc, argv);

    AutoSweep      auto_sweep;
    QtGameView    game;

    SweepMain sm(&game, &auto_sweep);

    return sm.run(1000);
}
