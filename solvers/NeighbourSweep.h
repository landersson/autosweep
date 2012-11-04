// This solver finds certain moves by only looking at a Cell's immediate neighbours.

#ifndef _NEIGHBOUR_SWEEP_H
#define _NEIGHBOUR_SWEEP_H

#include "MineField.h"
#include "AutoSweep.h"

class NeighbourSweep : public AutoSweep
{
public:
    // this method gets called every time a new game is started
    virtual void startNewGame(int total_num_mines = -1) 
    { }
    
    // this gets called to generate new sweep actions as long as we are still alive
    virtual bool sweep(const MineField* mf);

private:
    int flagObviousMines(const MineField* mf);
    int findSafeCells(const MineField* mf);
    int countNeighboursWithState(const MineField* mf, int i, int j, int state);
    bool rejectMineAt(const MineField* mf, int i, int j);

};

#endif // _NEIGHBOUR_SWEEP_H
