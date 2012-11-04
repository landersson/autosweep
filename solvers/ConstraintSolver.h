#ifndef _CONSTRAINT_SOLVER_H
#define _CONSTRAINT_SOLVER_H

#include "ConstraintSet.h"

class ConstraintSolver
{
    int      num_csets;
    int      cset_size;
    int      max_cset_size;
    uint64_t nodes_visited;
    uint64_t total_nodes;
    
    std::vector<Constraint>    _constraints;
    std::vector<ConstraintSet> _csets;

public:

    ConstraintSolver(const MineField* mf = 0) : _mf(mf) 
    { 
        // reset statistics
        num_csets = 0;
        cset_size = 0;
        nodes_visited = 0;
        total_nodes = 0;
        max_cset_size = 0;
    }

    void createConstraints()
    {
        for (int i = 0; i < _mf->getRows(); i++)
        {
            for (int j = 0; j < _mf->getCols(); j++)
            {
                const Cell& c = _mf->getCell(i, j);

                if (c.getState() != Cell::KNOWN) continue;

                Neighbours neighbours = _mf->getNeighbours(i, j);
                
                if (8 == countState(neighbours, Cell::KNOWN)) continue;
                
                Constraint new_constraint(Location(i, j), c.getValue());

                for (Location loc : neighbours)
                {
                    const Cell& n = _mf->getCell(loc);
             
                    if (n.getState() == Cell::UNKNOWN)
                    {
                        int cell_index = _mf->locationToIndex(loc.row, loc.col);

                        new_constraint.addX(cell_index);
                    }
                    else if (n.getState() == Cell::FLAGGED) 
                    {
                        new_constraint.C--;
                    }
                }

                if (!new_constraint.xs.empty())
                {
                    std::sort(new_constraint.xs.begin(), new_constraint.xs.end());

                    _constraints.push_back(new_constraint);
                }
            }
        }
    }

    bool solve(const MineField *mf, std::vector<SweepAction>& actions)
    {
        _mf = mf;

        _constraints.clear();

        createConstraints();

        if (_constraints.empty()) return false;


//        printConstraints(_constraints);

        setupActions(actions);

        if (!actions.empty()) return true;

        int iter = 0;
        while (simplifyConstraints())
        {
            Log::print(Log::DEBUG, "*** Simplify ITER %d\n", iter++);
        }

        deleteEmptyConstraints();
        
        setupActions(actions);
        if (!actions.empty()) return true;
        
        Log::print(Log::VERBOSE, "Simplified:\n");
        printConstraints(_constraints);


        separateConstraints();

        int set_index = 0;
        Log::print(Log::VERBOSE, "\nConstraint Sets: %d\n", _csets.size());
        for (ConstraintSet& cset : _csets)
        {
            cset.localizeIndices();
            Log::print(Log::VERBOSE, "--- Set %d -------------------------\n", set_index++);
            printConstraints(cset.constraints);

            Log::print(Log::VERBOSE, "Variable -> Constraints Mapping:\n", 
                       (int)cset.members.size());
            cset.setupVarToCstMapping();
            cset.printCells();
            Log::print(Log::VERBOSE, "\n");
        }
        Log::print(Log::VERBOSE, "------------------------------------\n");

        unsigned total_num_solutions = 1;

        for (ConstraintSet& cset : _csets)
        {
            cset.findSolutions();

            cset.createSafeClicks(actions, _mf->getCols());
            
            num_csets++;
            cset_size += cset.members.size();
            nodes_visited += cset.nodes_visited;
            total_nodes += (2 << cset.members.size());

            if (cset.members.size() > (unsigned)max_cset_size)
            {
                max_cset_size = cset.members.size();
            }

            total_num_solutions *= cset.members.size();

        }
        if (actions.empty())
        {
            Log::print(Log::VERBOSE, "Num solution permutations: %d\n", total_num_solutions);
        }
        

        return true;
    }

    void printConstraints(const std::vector<Constraint> &constraints)
    {
        int c = 0;
        for (const Constraint& cst : constraints)
        {            
            Log::print(Log::VERBOSE, " %2d (%02d,%02d) :  ", c, cst.loc.row, cst.loc.col);

            for (int index : cst.xs)
            {
                Log::print(Log::VERBOSE, "x%d ", index);
            }
            Log::print(Log::VERBOSE, " = %2d\n", cst.C);
            c++;
        }
    }

    // simplyfy constraints by merging sets that are subsets of eachother
    bool simplifyConstraints()
    {
        int simplified = false;

        for (unsigned i = 0; i < _constraints.size() - 1; i++)
        {
            for (unsigned j = i + 1; j < _constraints.size(); j++)
            {
                bool eliminated = false;
                if (_constraints[i].xs.size() < _constraints[j].xs.size()) 
                {
                    eliminated = eliminate(_constraints[i], _constraints[j]);
                }
                else
                {
                    eliminated = eliminate(_constraints[j], _constraints[i]);
                }

                if (eliminated)
                {
//                    Log::print(Log::DEBUG, "   simplifyConstraints() eliminated %d in %d\n", i, j);
//                    printConstraints(_constraints);
                    simplified = true;
                }
            }         
        }
        return simplified;
    }


    // separate simplified constraints into independent sets
    void separateConstraints()
    {
        _csets.clear();
#if 0
        // do not try to separate into independent sets
        _csets.push_back(ConstraintSet());

        for (const Constraint& c : _constraints)
        {
            _csets[0].addConstraint(c);
        }

        return;
#endif
        for (const Constraint& c : _constraints)
        {
            std::vector<int> found_in_sets;

            // find out which existint sets Constraint 'c' overlaps
            for (unsigned i = 0; i <  _csets.size(); i++)
            {
                if (_csets[i].overlaps(c))
                {
                    found_in_sets.push_back(i);
                }
            }

            if (found_in_sets.empty())
            {
                // 'c' doesn't overlap any existing constraint, add new set
                _csets.push_back(ConstraintSet(c));
            }
            else
            {
                // 'c' overlaps set 'found_in_sets[0]'... merge c into this set.
                _csets[found_in_sets[0]].addConstraint(c);

                // if 'c' is part of any other sets, merge these into found_in_sets[0] too 
                for (unsigned i = 1; i < found_in_sets.size(); i++)
                {
                    _csets[found_in_sets[0]].mergeConstraintSet(_csets[found_in_sets[i]]);
                }

                // erase merged sets
                found_in_sets.erase(found_in_sets.begin());

                while (found_in_sets.size() > 0)
                {
                    _csets.erase(_csets.begin() + found_in_sets.back());
                    found_in_sets.pop_back();
                }                
            }
        }

        // verify that no set overlaps another
        for (unsigned i = 0; i < _csets.size() - 1; i++)
        {
            for (unsigned j = i + 1; j < _csets.size(); j++)
            {
                if (_csets[i].overlaps(_csets[j]))
                {
                    Log::print(Log::ERROR, "FATAL ERROR: cset %d overlaps cset %d\n", i, j);
                    exit(1);
                }                        
                //printf("%d vs %d\n", i, j);
            }
        }
    }

    void setupActions(std::vector<SweepAction>& actions)
    {
        for (const Constraint& c : _constraints)
        {
            if (c.C == 0)
            {
                // all boolean variables must be zero
                for (int index : c.xs)
                {
                    Log::print(Log::VERBOSE, "   clicking %d\n", index);
                    actions.push_back(SweepAction(SweepAction::CLICK_CELL, 
                                                  _mf->indexToLocation(index),
                                                  SweepAction::CERTAIN));
                }
            }
            else if (c.C == (int)c.xs.size())
            {
                // all boolean variables must be true
                for (int index : c.xs)
                {
                    Log::print(Log::VERBOSE, "   flagging %d\n", index);
                    actions.push_back(SweepAction(SweepAction::FLAG_CELL, 
                                                  _mf->indexToLocation(index),
                                                  SweepAction::CERTAIN));
                }
            }
        }
    }

    void printCombinedStats()
    {        
        printf("cset stats:\n");
        printf("   Max size: %d vars\n", max_cset_size); 
        printf("   Average size: %.1f vars\n", 1.0 * cset_size / num_csets); 
        printf("   Average visit ratio: %.5f%%\n", 100.0 * nodes_visited / total_nodes); 
    }



private:

    void deleteEmptyConstraints()
    {
        std::vector<Constraint> constraints2 = _constraints;
        _constraints.clear();
        
        for (const Constraint& c : constraints2)
        {
            if (!c.xs.empty()) _constraints.push_back(c);
        }
    }


    // Try to eliminate c1 in c2... If c1 is a subset of c2, subtract
    // c1 from c2 and return true. If not, return false.

    bool eliminate(const Constraint &c1, Constraint& c2)
    {
        if (c1.empty()) return false;        

        for (int xi : c1.xs)
        {
            if (!c2.containsX(xi)) return false;
        }
        
        std::vector<int> old_x2 = c2.xs;
        c2.xs.clear();
        
        for (unsigned i = 0; i < old_x2.size(); i++)
        {
            if (!c1.containsX(old_x2[i])) c2.xs.push_back(old_x2[i]);
        }
            
        c2.C -= c1.C;
        
        return true;
    }


    int countState(const Neighbours& cells, int state)
    {
        int count = 0;
        for (Location loc : cells)
        {
            const Cell& n = _mf->getCell(loc);
            
            if (n.getState() == state)
            {
                count++;
            }
        }
        return count;
    }

    const MineField* _mf;
};


#endif // _CONSTRAINT_SOLVER_H
