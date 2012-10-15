
#ifndef _AUTO_SWEEP_H
#define _AUTO_SWEEP_H

#include "MineField.h"
#include "SweepAction.h"
#include "EdgeSolver.h"


#include <vector>
#include <cassert>

class NeighbourStats
{
    int _num_flagged;
    int _num_unknown;
};

class AutoSweep
{
public:
    AutoSweep() { }
    bool sweep(const MineField* mf);

    const std::vector<SweepAction>& getSweepActions()
    {
        return _actions;
    }

    const std::vector<MineField::LocationList>& getEdges() const
    {
        std::vector<MineField::LocationList> edges2;

        return _edges;
    }

    const std::vector<Location> getEdgeConfig(int index) const
    {
        if (_edges.size() == 0) return std::vector<Location>();
        return _edge_solver.getEdgeConfig(_edges[0], index);
    }

private:
    int flagBombs(MineField* mf);

    void updateModel(const MineField* mf);
    void selectFirstUnknown(const MineField* mf);

    int countNeighboursWithState(const MineField* mf, int i, int j, int state);
    int flagObviousMines(const MineField* mf);
    int findSafeCells(const MineField* mf);

    void findEdges(const MineField* mf);
    void solveEdges(const MineField* mf);


    bool rejectMineAt(const MineField* mf, int i, int j);

    std::vector<SweepAction> _actions;

    std::vector<MineField::LocationList> _edges;

    EdgeSolver _edge_solver;


    MineField _model;

};


#endif // _AUTO_SWEEP_H
