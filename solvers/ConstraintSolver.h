#ifndef _CONSTRAINT_SOLVER_H
#define _CONSTRAINT_SOLVER_H

#include "ConstraintSet.h"

#include <algorithm>

class ConstraintSolver
{
    int      num_csets;
    int      cset_size;
    int      max_cset_size;
    uint64_t nodes_visited;
    uint64_t total_nodes;
    
    std::vector<Constraint>    _constraints;
    std::vector<ConstraintSet> _csets;

    const MineField* _mf;
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

    // This method creates a set of constraints given the current state of the mine field.
    // Constraints are created for cells with a known number of neighbouring mines that have
    // more than one unknown neighbour cell. I.e, if cell C is known to have 2 neighbouring 
    // mines and 4 unkown neighbours N1-N2, we can create the constraint:
    //
    //    N1 + N2 + N2 + N2 = 2, 
    //
    // where N<n> = 1 if cell n contains a mine, or zero otherwise

    void createConstraints()
    {
        for (auto git = _mf->begin(); git != _mf->end(); ++git)
        {            
            const Cell& c = *git; 
            
            if (c.getState() != Cell::KNOWN) continue;
            
            Neighbours neighbours = _mf->getNeighbours(git.getLocation().row,
                                                       git.getLocation().col);
            
            if (8 == countState(neighbours, Cell::KNOWN)) continue;
            
            Constraint new_constraint(git.getLocation(), c.getValue());
            
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

    // This method simplifies the list of constraints by merging constraints that 
    // are subsets of each other. For example, the constraints,
    //
    //    C1: x1 + x4 + x5 + x6 = 3
    //    C2: x4 + x6 = 2
    //
    // will be merged and simplified into:
    //
    //    C1: x1 + x5 = 1

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


    // This method separate constraints into independent sets. Example:
    //
    //  Constraints:
    //        C1: x1 + x2      = 2
    //        C2: x2 + x4 + x5 = 1
    //        C3: x6 + x7      = 3
    //
    // Constraint 1 and 2 are clearly interdependant because they share a common
    // variable c2, while constraint 3 is independent. This set of contraints would
    // be split into two sets with the first containing C1 and C2, and the second C3.
    //
    // The main reason for doing this is speed. The set of final solutions should be 
    // the same regardless of if we solve all constraints as a single, large set of
    // contraints or solve each independent subset individually. However, the number 
    // of possible solutions that needs to be tested will be significantly less if
    // we separate constraints into independent sets.

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

            // find out which existing sets Constraint 'c' overlaps
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

        // verify that no set overlaps another set
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

    bool solve(const MineField *mf, std::vector<SweepAction>& actions)
    {
        _mf = mf;

        _constraints.clear();
        _csets.clear();

        createConstraints();

        if (_constraints.empty()) return false;

//        printConstraints(_constraints);

        if (findSafeActions(actions)) return true;

        int iter = 0;
        while (simplifyConstraints())
        {
            Log::print(Log::DEBUG, "*** Simplify ITER %d\n", iter++);
        }

        deleteEmptyConstraints();

        if (findSafeActions(actions)) return true;

        Log::print(Log::VERBOSE, "Simplified:\n");
        printConstraints(_constraints);

        separateConstraints();

        int set_index = 0;
        Log::print(Log::VERBOSE, "\nConstraint Sets: %d\n", _csets.size());
        for (ConstraintSet& cset : _csets)
        {
            cset.localizeIndices();
            Log::print(Log::VERBOSE, "--- Set %d -------------------------\n", set_index++);
//            printConstraints(cset.constraints);

            Log::print(Log::DEBUG, "   Variable -> Constraints Mapping:\n", 
                       (int)cset.getMembers().size());
            cset.setupVarToCstMapping();
//            cset.printCells();
            Log::print(Log::DEBUG, "\n");
        }
        Log::print(Log::VERBOSE, "------------------------------------\n");

        unsigned total_num_solutions = 1;

        for (ConstraintSet& cset : _csets)
        {
            cset.findSolutions();

            cset.createSafeClicks(actions, _mf->getCols());
            
            num_csets++;
            cset_size += cset.getMembers().size();
//            nodes_visited += cset.nodes_visited;
            total_nodes += (2 << cset.getMembers().size());

            if (cset.getMembers().size() > (unsigned)max_cset_size)
            {
                max_cset_size = cset.getMembers().size();
            }

            total_num_solutions *= cset.solutions.size();
        }

        if (actions.empty())
        {
            Log::print(Log::VERBOSE, "Num solution permutations: %d\n", total_num_solutions);
        }
        

        return true;
    }

    // Check if any of the constraints in the list are simple enough to
    // say for sure that certain cells does/doesn't contain bombs
    bool findSafeActions(std::vector<SweepAction> & actions)
    {
        for (const Constraint& c : _constraints)
        {
            if (c.C == 0)
            {
                // all boolean variables must be zero
                for (int index : c.xs)
                {
                    Log::print(Log::DEBUG, "   clicking %d\n", index);
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
                    Log::print(Log::DEBUG, "   flagging %d\n", index);
                    actions.push_back(SweepAction(SweepAction::FLAG_CELL, 
                                                  _mf->indexToLocation(index),
                                                  SweepAction::CERTAIN));
                }
            }
        }
        return !actions.empty();
    }

    const std::vector<ConstraintSet> & getConstraintSets() const
    {
        return _csets;
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

    // return the number of cells in the Neighbours list that are in the given state.
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

};


#endif // _CONSTRAINT_SOLVER_H
