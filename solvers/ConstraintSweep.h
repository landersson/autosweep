
#ifndef _CONSTRAINT_SWEEP_H
#define _CONSTRAINT_SWEEP_H

#include "MineField.h"
#include "AutoSweep.h"
#include "ConstraintSolver.h"

#include <vector>

class ConstraintSweep : public AutoSweep
{
public:
    // this method gets called every time a new game is started
    virtual void startNewGame(int total_num_mines = -1) 
    { 
        _mines_left = total_num_mines;
    }
    
    // this gets called to generate new sweep actions as long as we are still alive
    virtual bool sweep(const MineField* mf);

    
private:
    void updateModel(const MineField* mf);

    bool clickIfUnknown(const MineField* mf, int i, int j);
    void pushIfUnknown(const MineField* mf, 
                       std::vector<Location> &locations, 
                       int i, int j);
    void selectFirstUnknown(const MineField* mf);

    void removeDuplicateActions();

    // the total number of mines left in unknown cells 
    int _mines_left;

    ConstraintSolver _cst_solver;

};

#endif // _AUTO_SWEEP_H
