
#ifndef _EDGE_FINDER_H_
#define _EDGE_FINDER_H_

#include "MineField.h"

class EdgeFinder
{

public:
    EdgeFinder(const MineField& mf) : _minefield(mf)
    {
        _edge_grid.allocCells(mf.getRows(), mf.getCols());
    }

    void findEdges(std::vector<MineField::LocationList>& edges)
    {
        int tag = 1;
        while (true)
        {
            Location start = findStart();

            if (!start.valid()) break;

            edges.push_back(MineField::LocationList());

            findEdgesRecursive(edges.back(), start, tag);

        }
    }

private:
    void findEdgesRecursive(MineField::LocationList& edge, Location location, int tag)
    {
        printf("findEdgesRecursive(%d,%d)\n", location.col, location.row);
        _edge_grid.getCell(location) = tag;
        edge.push_back(location);
        //        sleep(1);


        for (Location neighbour_location : _minefield.getNeighbours(location.row, location.col))
        {
            if (cellIsEdge(neighbour_location.row, neighbour_location.col))
            {
                findEdgesRecursive(edge, neighbour_location, tag);
            }
        }
    }


    bool cellIsEdge(int i, int j) const
    {
        if (_minefield.getCell(i, j).getState() != Cell::UNKNOWN ||
            _edge_grid.getCell(i, j) != 0) return false;

        for (Location loc : _minefield.getNeighbours(i, j))
        {
            const Cell& n = _minefield.getCell(loc);
            int tag = _edge_grid.getCell(loc);
            //            printf("tag %d,%d = %d\n", j, i, tag);

            if (n.getState() == Cell::KNOWN && n.getValue() > 0 && tag == 0) return true;
        }
        return false;
    }

    Location findStart()
    {
        for (int i = 0; i < _minefield.getRows(); i++)
        {
            for (int j = 0; j < _minefield.getCols(); j++)
            {
                if (cellIsEdge(i, j)) return Location(i, j);
            }
        }
        return Location(-1, -1);
    }

    Grid<int>  _edge_grid;

    const MineField& _minefield;


};


#endif
