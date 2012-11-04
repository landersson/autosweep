
#include "SweepMain.h"
#include "QtGameView.h"

#include "solvers/ConstraintSweep.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    SweepConfig config;

    if (!config.initialize(argc, argv)) return 1;

    QApplication app(argc, argv);

    srand(config.getInteger("seed"));

    AutoSweep* auto_sweep = AutoSweep::loadSolver(config.getString("solver"));
    if (auto_sweep == nullptr) return 1;

    QtGameView game;

    SweepMain sm(&game, auto_sweep);

    return sm.run(config);
}
