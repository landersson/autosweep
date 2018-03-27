Simple MineSweeper Solver with GUI

Build Instructions:

  Dependencies: cmake, 
                C++ compiler with decent C++11 support (i.e g++ >= 4.6) 
                qt4-dev for gui tools

  mkdir build
  cd build
  cmake ..
  make


Command Line Options (and default values):
   rows=16  
   cols=30
   mines=99
   games=1
   seed=1
   safe-open=0
   log-level=3
   solver=constraint-sweep

Example Usage:

  * Simulate and time 1000 expert mode games using the constraint eq solver:

       time ./cas games=1000 solver=constraint-sweep          

  * Simulate and time 10000 intermediate mode games using neighbour-sweep

       time ./cas games=10000 rows=16 cols=16 mines=40 solver=neighbour-sweep


Source Code Terminology:

   Cell    =   A MineSweeper field element
   Corner  =   One of the for Cells at the corner of the minefield
   Edge    =   The edge of the minefield, excluding Corners
   LHS     =   Left Hand Side
   RHS     =   Right Hand Side
   <TODO>
