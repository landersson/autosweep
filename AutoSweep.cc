
#include "AutoSweep.h"
#include "EdgeFinder.h"

#include "utils.h"

#include <iostream>

bool AutoSweep::sweep(const MineField* mf)
{
    //    std::cout << "Reality:\n";
    //    std::cout << mf->toString() << std::endl;

    _actions.clear();

    updateModel(mf);

    flagObviousMines(mf);
    findSafeCells(mf);

    if (_actions.empty())
    {
        selectFirstUnknown(mf);
#if 0
        _edges.clear();
        printf("Finding edges...\n");
        findEdges(mf);
        solveEdges(mf);
#endif

    }
    //    printf("AutoSweep::sweep()\n");

    return true;
}

void AutoSweep::updateModel(const MineField* mf)
{
    if (_model.empty())
    {
        _model.allocCells(mf->getRows(), mf->getCols());
        _model.resetCells();
        printf("AutoSweep::updateModel() initializing model size = %d,%d\n",
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
    //    std::cout << "Model:\n";
    //    std::cout << mf->toString() << std::endl;
}


void AutoSweep::selectFirstUnknown(const MineField* mf)
{
#if 1
    while (true)
    {
        int i = rand_int(0, mf->getRows() - 1);
        int j = rand_int(0, mf->getCols() - 1);

        //        printf("%d, %d\n", i, j);

        const Cell& cell = mf->getCell(i, j);

        if ((cell.getState() == Cell::UNKNOWN))
        {
            _actions.push_back(SweepAction(SweepAction::CLICK_CELL, i, j));
            return;
        }
    }
#else

    for (int i = 0; i < mf->getRows(); i++)
    {
        for (int j = 0; j < mf->getCols(); j++)
        {
            const Cell& cell = mf->getCell(i, j);

            if ((cell.getState() == Cell::UNKNOWN))
            {
                _actions.push_back(SweepAction(SweepAction::CLICK_CELL, i, j));
                return;
            }
        }
    }
#endif
}

int AutoSweep::flagObviousMines(const MineField* mf)
{
    for (int i = 0; i < mf->getRows(); i++)
    {
        for (int j = 0; j < mf->getCols(); j++)
        {
            const Cell& cell = mf->getCell(i, j);

            // an an unflagged cell with at least one mine nearby
            if ((cell.getState() == Cell::KNOWN) &&
                cell.getValue() > 0)
            {
                Neighbours neighbours = mf->getNeighbours(i, j);

                unsigned num_unknown_neighbours = 0;
                unsigned num_flagged_neighbours = 0;

                // count number of flagged mines and number of unknown
                // cells in neighbourhood.
                for (Location loc : neighbours)
                {
                    const Cell& n = mf->getCell(loc);

                    if (n.getState() == Cell::UNKNOWN)
                    {
                        num_unknown_neighbours++;
                    }
                    if (n.getState() == Cell::FLAGGED)
                    {
                        num_flagged_neighbours++;
                    }
                }

                int unknown_and_flagged = num_flagged_neighbours + num_unknown_neighbours;

                if (num_unknown_neighbours > 0 &&
                    (cell.getValue() == unknown_and_flagged))
                {
                    for (Location loc : neighbours)
                    {
                        const Cell& n = mf->getCell(loc);

                        if (n.getState() == Cell::UNKNOWN &&
                            _model.getCell(loc).getState() == Cell::UNKNOWN)
                        {
                            _model.getCell(loc).setState(Cell::FLAGGED);

                            _actions.push_back(SweepAction(SweepAction::FLAG_CELL,
                                                           loc.row, loc.col));
                            //                            n.setValue(Cell::MINE);
                            //                            printf("Mine at %d,%d\n", loc.col, loc.row);
                        }
                    }
                }
#if 0
                printf("%d,%d: c=%d, u=%d, b=%d\n", j, i, 11,
                       num_unknown_neighbours,
                       num_flagged_neighbours);
#endif
            }
        }
    }
    return 1;
}


int AutoSweep::findSafeCells(const MineField* mf)
{
    for (int i = 0; i < mf->getRows(); i++)
    {
        for (int j = 0; j < mf->getCols(); j++)
        {
            const Cell& cell = _model.getCell(i, j);

            // an an unflagged cell with at least one mine nearby
            if ((cell.getState() == Cell::UNKNOWN))
            {
                if (rejectMineAt(mf, i, j))
                {
                    //                    printf("%d,%d rejected\n", j, i);

                    _actions.push_back(SweepAction(SweepAction::CLICK_CELL, i, j));
                }
            }
        }
    }
    return 1;
}


int AutoSweep::countNeighboursWithState(const MineField* mf, int i, int j, int state)
{
    Neighbours neighbours = mf->getNeighbours(i, j);

    int num_flagged = 0;

    for (Location loc : neighbours)
    {
        if (mf->getCell(loc).getState() == state)
        {
            num_flagged++;
        }
    }

    return num_flagged;
}



void AutoSweep::findEdges(const MineField* mf)
{
    _edges.clear();

    EdgeFinder ef(*mf);
    ef.findEdges(_edges);

    //    _edges.clear();
    //    _edges.resize(1);

    //    _edges[0].push_back(Location(1, 2));
}


void AutoSweep::solveEdges(const MineField* mf)
{
    for (const MineField::LocationList& edge : _edges)
    {
        _edge_solver.solveEdge(mf, edge);
    }
}


bool AutoSweep::rejectMineAt(const MineField* mf, int i, int j)
{
    Neighbours neighbours = mf->getNeighbours(i, j);

    //    printf("reject %d,%d\n", j, i);

    for (Location loc : neighbours)
    {
        const Cell& n = mf->getCell(loc);

        if (n.isNextToBombs())
        {
            int num_flagged = countNeighboursWithState(&_model, loc.row, loc.col, Cell::FLAGGED);

#if 0
            printf("   REJECT %d,%d: nb %d,%d: flagged=%d, value=%d\n",
                   j, i,
                   loc.col, loc.row,
                   num_flagged, n.getValue());
#endif
            if (num_flagged > n.getValue())
            {
                fprintf(stderr, "ERROR, too many cells flagged next to %d,%d\n",
                        loc.col, loc.row);
            }
            else if (num_flagged == n.getValue())
            {
                return true;
            }
        }
    }
    return false;
}


