
#include "ConstraintSweep.h"
#include "utils.h"

bool ConstraintSweep::sweep(const MineField* mf)
{
    _actions.clear();

    updateModel(mf);

    if (!_actions.empty()) return true;

    _cst_solver.solve(&_model, _actions);

    if (_actions.empty())
    {
        Log::print(Log::VERBOSE, "No actions created, taking random guess\n");
        selectFirstUnknown(mf);
    }

    return true;
}

void ConstraintSweep::updateModel(const MineField* mf)
{
    if (_model.empty())
    {
        _model.allocCells(mf->getRows(), mf->getCols());
        _model.resetCells();
        Log::print(Log::VERBOSE, "ConstraintSweep::updateModel() initializing model size = %d,%d\n",
                   _model.getRows(), _model.getCols());
    }
    for (int i = 0; i < mf->getRows(); i++)
    {
        for (int j = 0; j < mf->getCols(); j++)
        {
            Cell& m = _model.getCell(i, j);
            const Cell& r = mf->getCell(i, j);
            m.setValue(r.getValue());
            m.setState(r.getState());
        }
    }
}

void ConstraintSweep::selectFirstUnknown(const MineField* mf)
{
    // try to click a corner
    if (clickIfUnknown(mf, mf->getRows() - 1, mf->getCols() - 1)) return;
    if (clickIfUnknown(mf, 0, mf->getCols() - 1)) return;
    if (clickIfUnknown(mf, mf->getRows() - 1, 0)) return;
    if (clickIfUnknown(mf, 0, 0)) return;

    
    // try to click an edge
    std::vector<Location> edges;

    for (int j = 1; j < mf->getCols() - 1; j++)
    {
        pushIfUnknown(mf, edges, 0, j);
        pushIfUnknown(mf, edges, mf->getRows() - 1, j);
    } 
    for (int i = 1; i < mf->getRows() - 1; i++)
    {
        pushIfUnknown(mf, edges, i, 0);
        pushIfUnknown(mf, edges, i, mf->getCols() - 1);
    }

    if (!edges.empty())
    {
        unsigned index = rand_int(0, edges.size() - 1);
        assert(index < edges.size());
        _actions.push_back(SweepAction(SweepAction::CLICK_CELL, edges[index]));
        return;        
    }

    // select random internal cell
    while (true)
    {
        int i = rand_int(0, mf->getRows() - 1);
        int j = rand_int(0, mf->getCols() - 1);

        //        printf("%d, %d\n", i, j);

        const Cell& cell = mf->getCell(i, j);

        if ((cell.getState() == Cell::UNKNOWN))
        {
            _actions.push_back(SweepAction(SweepAction::CLICK_CELL, 
                                           Location(i, j)));
            return;
        }
    }
}


// if the state of Cell i,j is unknown, add an action to click it
bool ConstraintSweep::clickIfUnknown(const MineField* mf, int i, int j)
{
    if (mf->getCell(i, j).getState() == Cell::UNKNOWN)
    {
        _actions.push_back(SweepAction(SweepAction::CLICK_CELL, 
                                       Location(i, j)));
        return true;
    }
    return false;
}

// if the state of cell i,j is unknown, push it's location on 'locations'
void ConstraintSweep::pushIfUnknown(const MineField* mf, 
                                    std::vector<Location>& locations, 
                                    int i, int j)
{
    if (mf->getCell(i, j).getState() == Cell::UNKNOWN)
    {
        locations.push_back(Location(i, j));
    }
}


extern "C" void* initialize(const char *options)
{
    return new ConstraintSweep;
}
