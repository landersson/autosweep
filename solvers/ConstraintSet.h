
#ifndef _CONSTRAINT_SET_H
#define _CONSTRAINT_SET_H

#include "SweepAction.h"
#include "Constraint.h"

#include "log.h"
#include "utils.h"

#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

#include <assert.h>

//--- ConstrainSet ------------------------------------------------------------
//
// This class describes a set of interdependent constraint equations, i.e:
// 
//    x1 + x3 + x5 + ... + x6 = 3
//    x4 + x6 + x7 + ... + x9 = 2
//    ...
//
//    In this application, x's are assumed to be boolean, i.e 0 or 1.
//-----------------------------------------------------------------------------   
class ConstraintSet
{
public:
    struct Solution
    {
        Solution(const std::vector<bool> & xvec) : xs(xvec)
        {
            sum = std::count(xs.begin(), xs.end(), true);
        }

        // number of mines, i.e sum of x's = 1
        int sum;

        std::vector<bool> xs;
    };


private:
    //--- Variables -----------------------------------------------------------
    std::vector<Constraint>           _constraints;


    // combined set of x variables (througout all constraints) in this set
    std::vector<int>                  _members;

    // for each variable x<i>, 'var_cst_map[i]' refers to a vector of
    // indices (into the 'constraints' vector), listing which
    // Constraints are dependent on x<i>.
    std::map<int, std::vector<int> >  var_cst_map;
    
    std::map<int, int>                original_indices;      
    
    // backtracking statistics
    uint64_t nodes_visited;


public:
    
    // solutions satisfying all constraints in this set
    std::vector<Solution>             solutions;

    
    std::map<int, std::vector<const Solution *> > solutions_with_num_bombs;


    //--- Methods -------------------------------------------------------------

public:

    // This method will localise the variable indices 
    void localizeIndices()
    {
        std::map<int, int> new_indices;      
        
        for (unsigned i = 0; i < _members.size(); i++)
        {
            original_indices[i] = _members[i];
            new_indices[_members[i]] = i;        
        }
        for (unsigned i = 0; i < _members.size(); i++)
        {
            _members[i] = new_indices[_members[i]];
        }
        for (Constraint &cst : _constraints)
        {
            cst.remapIndices(new_indices);
        }
    }

    ConstraintSet() : nodes_visited(0) 
    { }

    ConstraintSet(const Constraint& c) : nodes_visited(0)
    {
        addConstraint(c);
    }

    // Add a new constraint to this set
    void addConstraint(const Constraint& c)
    {
        for (int xi : c.xs)
        {
            if (!containsX(xi))
            {
                _members.push_back(xi);
            }
        }
        _constraints.push_back(c);
    }

    // returns true if x<i> is part of this constraint set
    bool containsX(int i) const
    {
        return std::find(_members.begin(), _members.end(), i) != _members.end();
    }

    // returns true if any x in given Constraint 'c' is a member of this set.
    bool overlaps(const Constraint& c)
    {
        for (int i : c.xs)
        {
            if (containsX(i)) return true;
        }
        return false;
    }

    
    void setupVarToCstMapping()
    {
        for (int xi : _members)
        {
            for (unsigned c_index = 0; c_index < _constraints.size(); c_index++)
            {
                if (_constraints[c_index].containsX(xi))
                {
                    var_cst_map[xi].push_back(c_index);
                }
            }
        }
    }

    void printCells() const
    {
        for (auto x : var_cst_map)
        {
            std::string s = "      x";
            s += utils::int2str(x.first);
            Log::print(Log::VERBOSE, "%3s : ", s.c_str());
            
            for (int i : x.second)
            {
                Log::print(Log::VERBOSE, "%d ", i);
            }
            Log::print(Log::VERBOSE, "\n");           
        }
    }

    // find all possible solutions to current constraints.
    void findSolutions()
    {
        solutions.clear();
        std::vector<bool> state;

        nodes_visited = 0;

        backtrack(state);

        if (!verifySolutions())
        {
            Log::print(Log::ERROR, "ERROR, Invalid solutions found!!\n");
            exit(1);
        }

        Log::print(Log::VERBOSE, "Backtracking %d high tree done, found %d solutions\n", 
                   _members.size(), solutions.size());
        
        double total_nodes = std::pow(2.0, (double)_members.size() + 1);

        double percentage_visited = 100.0 * nodes_visited / total_nodes;
            
        Log::print(Log::VERBOSE, "   Nodes visited = %d/%.0f (%.2f%%)\n",
                   nodes_visited, total_nodes, percentage_visited);

        for (const Solution & solution : solutions)
        {
            solutions_with_num_bombs[solution.sum].push_back(&solution);
        }
                
        for (auto x : solutions_with_num_bombs)
        {
            Log::print(Log::VERBOSE, "   %d bombs => %d solutions\n",
                       x.first, x.second.size());
        }
    }

    // classic backtracking algorithm...
    void backtrack(std::vector<bool>& state)
    {
        if (state.size() == _members.size())
        {
            // found a solution!
#if 0
            Log::print(Log::DEBUG, "backtrack() found solution: ");
            
            int i = 0;
            int n = 0;
            for (bool b : state) 
            {
                Log::print(Log::DEBUG, "x%d=%d ", i++, (int)b);
                if (b) n++;
            }
            Log::print(Log::DEBUG, " n=%d\n", n);
#endif
            solutions.push_back(Solution(state));
            
            return;
        }

        if (!rejectable(false, state))
        {
            state.push_back(false);
            backtrack(state);
            state.pop_back();
        }

        if (!rejectable(true, state))
        {
            state.push_back(true);
            backtrack(state);
            state.pop_back();
        }        
    }

    // returns true if, given the the state of x0 - x<n-1> in 'current_xs'
    // the state of x<n> in 'next_x' can be rejected as a possible solution
    // to the current set of constraints
    bool rejectable(bool next_x, const std::vector<bool> & current_xs)
    {
        nodes_visited++;

        int x_n = current_xs.size();

        for (int cst_index : var_cst_map[x_n])
        {
//            printf("CONSTRANT SET %d\n", cst_index);
            if (_constraints[cst_index].rejectable(next_x, current_xs))
                return true;
        }

        return false;
    }

    // verify that all solutions found are actually correct
    bool verifySolutions()
    {
        for(const Solution& solution : solutions)
        {
            for (const Constraint& cst : _constraints)
            {
                if (!cst.isValidSolution(solution.xs)) return false;
            }
        }
        return true;
    }

    const std::vector<Location> getMemberLocations(const GridSize & grid) const
    {
        std::vector<Location> locations;        

        for (const auto & index : original_indices)
        {
            locations.push_back(grid.indexToLocation(index.second));
        }
        return locations;
    }


    Location getMemberLocation(int member_index, const GridSize & grid) const
    {
        const auto it = original_indices.find(member_index);
        
        if (it == original_indices.end()) assert(0);

        return grid.indexToLocation(it->second);
    }

    void createSafeClicks(std::vector<SweepAction>& actions, int cols)
    {
        int mine_count[_members.size()];

        for (unsigned i = 0; i < _members.size(); i++)
        {
            mine_count[i] = 0;
        }

        for(const Solution& solution : solutions)
        {
            for (unsigned i = 0; i < solution.xs.size(); i++)
            {
                mine_count[i] += solution.xs[i];
            }
        }
        
        for (unsigned i = 0; i < _members.size(); i++)
        {
            int index = original_indices[_members[i]];
            int row = index / cols;
            int col = index - row * cols;

            if (mine_count[i] == 0)
            {
                Log::print(Log::DEBUG, "   safe clicking %d\n", index);
                actions.push_back(SweepAction(SweepAction::CLICK_CELL, 
                                              Location(row, col),
                                              SweepAction::CERTAIN));
            }
            else if (mine_count[i] == (int)solutions.size())
            {
                Log::print(Log::INFO, "   safe flagging %d (%d)\n", index, _members[i]);
#if 1
                actions.push_back(SweepAction(SweepAction::FLAG_CELL, 
                                              Location(row, col),
                                              SweepAction::CERTAIN));
#endif
            }
        }
#if 0
        if (0 && actions.empty())
        {
            int min_index = std::min_element(mine_count, mine_count + _members.size()) - mine_count;

            int index = original_indices[_members[min_index]];
            int row = index / cols;
            int col = index - row * cols;

            Log::print(Log::VERBOSE, "   clicking %d\n", index);
            actions.push_back(SweepAction(SweepAction::CLICK_CELL, 
                                          Location(row, col)));

        }
#endif
    }

    // merge constraints from given set into this set
    void mergeConstraintSet(const ConstraintSet& cset);

    // returns true if any of the variables in this set exists in given set 'other'
    bool overlaps(const ConstraintSet& other);

    const std::vector<int>& getMembers() const
    {
        return _members;
    }
    int numMembers() const 
    { 
        return _members.size(); 
    }

};


inline void ConstraintSet::mergeConstraintSet(const ConstraintSet& cset)
{
    for (const Constraint& c : cset._constraints)
    {
        this->addConstraint(c);
    }
}


inline bool ConstraintSet::overlaps(const ConstraintSet& other)
{
    for (int i : _members)
    {
        if (other.containsX(i)) return true;
    }
    return false;
}


#endif // _CONSTRAINT_SET_H

