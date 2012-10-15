
#ifndef _EDGE_SOLVER_H_
#define _EDGE_SOLVER_H_

#include "MineField.h"


class EdgeSolver
{
    struct CheckPoint
    {
        CheckPoint(Location& l, int hidden_bombs) :
            location(l),
            num_hidden_bombs(hidden_bombs)
        { }
        Location         location;
        int              num_hidden_bombs;
        std::vector<int> edge_points;
    };

public:
    EdgeSolver()
    { }

    void solveEdge(const MineField* mf, const MineField::LocationList& edge)
    {
        _minefield = mf;

        _check_points.clear();
        _valid_configs.clear();

        printf("Edge Size: %zd\n", edge.size());

        findCheckPoints(edge);

        printf("CheckPoints:\n");
        for (const CheckPoint & cp : _check_points)
        {
            printf(" %d,%d, %d : ", cp.location.row, cp.location.col, cp.num_hidden_bombs);
            for (int index : cp.edge_points)
            {
                printf(" %d", index);
            }
            printf("\n");
        }

        for (uint64_t config = 0; config < (1ULL << edge.size()); config++)
        {
            //            printf("testing %d\n", config);
            if (testConfiguration(_check_points, config))
            {
                printf("possible config 0x%x\n", (unsigned)config);
                _valid_configs.push_back(config);
            }
        }
    }

    const std::vector<Location> getEdgeConfig(const MineField::LocationList& edge, unsigned config_index) const
    {
        printf("getEdgeConfig(%d)\n\n", config_index);
        std::vector<Location> config_locs;
        if (config_index >= _valid_configs.size()) return config_locs;

        uint64_t config = _valid_configs[config_index];

        printf("getEdgeConfig: config = 0x%x\n", unsigned(config));

        int index = 0;
        while (config != 0)
        {
            if (config & 1)
            {
                config_locs.push_back(edge[index]);
            }
            config >>= 1;
            index++;
        }

        return config_locs;
    }

private:

    template<typename T>
    typename std::vector<T>::iterator searchVector(typename std::vector<T>& vec, Location& needle)
    {
        for (typename std::vector<T>::iterator it = vec.begin(); it != vec.end(); ++it)
        {
            if (it->location == needle) return it;
        }
        return vec.end();
    }

    bool testConfiguration(std::vector<CheckPoint>& checkpoints, uint64_t config)
    {
        for (const CheckPoint & cp : checkpoints)
        {
            int num_bombs = 0;

            for (int edge_index : cp.edge_points)
            {
                if (config & (1ULL << edge_index)) num_bombs++;
            }

            if (num_bombs != cp.num_hidden_bombs)
                return false;
        }
        return true;
    }

    void findCheckPoints(const MineField::LocationList& edge)
    {
        int edge_point_index = 0;

        for (Location edge_point : edge)
        {
            MineField::LocationList neighbours = _minefield->getNeighbours(edge_point.row, edge_point.col);
            for (Location loc : neighbours)
            {
                const Cell& nb = _minefield->getCell(loc);

                if (nb.getState() == Cell::KNOWN && nb.getValue() > 0)
                {
                    auto it = searchVector(_check_points, loc);

                    if (it == _check_points.end())
                    {
                        _check_points.push_back(CheckPoint(loc, nb.getValue()));

                        subtractFlaggedNeighbours(_check_points.back());

                        it = _check_points.end() - 1;
                    }

                    it->edge_points.push_back(edge_point_index);
                }
            }
            edge_point_index++;
        }
    }


    void subtractFlaggedNeighbours(CheckPoint& cp)
    {
        MineField::LocationList neighbours = _minefield->getNeighbours(cp.location.row, cp.location.col);
        for (Location loc : neighbours)
        {
            if (_minefield->getCell(loc).getState() == Cell::FLAGGED)
            {
                cp.num_hidden_bombs--;
            }
        }
    }



    bool isPossibleSolution(MineField::LocationList& edge, uint64_t mine_map)
    {
        for (unsigned i = 0; i < edge.size(); i++)
        {

        }
        return false;
    }

    std::vector<CheckPoint> _check_points;

    const MineField* _minefield;

    std::vector<uint64_t> _valid_configs;

};


#endif // _EDGE_SOLVER_H_
