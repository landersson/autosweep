#ifndef _CONSTRAINT_H
#define _CONSTRAINT_H

#include <vector>
#include <algorithm>
#include <map>

//--- Constraint --------------------------------------------------------------
//
// This class describes a relation of the form x1 + x2 + ... + x<n> = C
// In this application, x's are assumed to be a boolean variables. 
//
//-----------------------------------------------------------------------------
struct Constraint
{
    //--- Variables -----------------------------------------------------------


    // the original minefield location of this Constraint
    Location loc;

    // lhs variable indices: i.e: x0+x1+x3+x4 makes xs={1,3,4}
    std::vector<int> xs;

    // rhs constant value
    int C;


    //--- Methods -------------------------------------------------------------


    Constraint(const Location& l, int rhs) : loc(l), C(rhs) { }

    void addX(int i)
    {
        xs.push_back(i);
    }

    // returns true if x<i> is part of this constraint
    bool containsX(int i) const
    {
        return std::find(xs.begin(), xs.end(), i) != xs.end();
    }

    void fixIndices(std::map<int, int>& new_indices)
    {
        for (unsigned i = 0; i < xs.size(); i++)
        {
            xs[i] = new_indices[xs[i]];
        }
        sort(xs.begin(), xs.end());
    }

    // given current partial variable state { x0...x<n-1> } in 'vars'
    // and state of x<n> in 'last', return true if the given state of
    // 'last' can be safely rejected.
    bool rejectable(bool last, const std::vector<bool> & vars)
    {
#if 0
        // debug spam
        printf("Constraints::rejectable(x%d=%d, vars={", 
               (unsigned)vars.size(), last);
        for (unsigned i = 0; i < vars.size(); i++)
            printf("x%d=%d, ", i, vars[i]);
        printf("}\n");
#endif

        // bit i in 'mask' represents xs[i] and is used to keep track of
        // which x's have already been eliminated from this constraint.

        // initially, set all mass bits representing xs to one 
        // i.e (not eliminated)

        unsigned mask = (1 << xs.size()) - 1;
        
        int c = C;
        
        // "subtract" known variables from this Constraint.
        for (unsigned i = 0; i < xs.size(); i++)
        {
            unsigned x_index = xs[i];

            if (x_index < vars.size())
            {
                // x_index is part of the known set of variables

                // subtract is's value from the RHS constant 
                if (vars[x_index] == true) 
                {
                    c--;
                }

                // remove it from the LHS bitmask of vars
                clearBit(mask, i);
            }
        }
        
        int num_bits_set = countBitsSet(mask);

        if (num_bits_set == 1)
        {
            // If only one x variable remains, it must be x<n>, i.e 'last',
            // and this constraint is fully resolvable. If the value
            // if 'last' doesn't match the remaining rhs constant 'c',
            // this configuration can be safely rejected.

            int last_value = last ? 1 : 0;

            if (c != last_value)
            {
                return true;
            }
        }
        return false;
    }

    // returns true if the lhs x variable configuration given in 'vars'
    // is compatible with this constraint
    bool isValidSolution(const std::vector<bool> &vars) const
    {
        int c = 0;
        for (unsigned i = 0; i < xs.size(); i++)
        {
            if (vars[xs[i]]) c++;
        }
        
        return c == C;
    }
    
    bool empty() const
    {
        return xs.empty();
    }

private:
    void clearBit(unsigned &x, int bit_to_clear) const
    {
        x &= ~(1u << bit_to_clear);
    }

    // returns number of bits set in the given integer 'n'
    int countBitsSet(unsigned n) const
    {
        int count = 0 ;
        while (n)  
        {
            count++ ;
            n &= (n - 1) ;
        }
        return count;
    }
};

#endif // _CONSTRAINT_H
