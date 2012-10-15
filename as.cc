
#include "SweepMain.h"
#include "AutoSweep.h"
#include "SimulatedGame.h"

#include "Neighbours.h"

// experiment to determine probabilities of opening a zero in corner/edge/center cells
void test_zero_probabilities()
{
    int corner_total = 0;
    int edge_total = 0;
    int center_total = 0;


    int corner_zeros = 0;
    int edge_zeros = 0;
    int center_zeros = 0;


    for (int i = 0; i < 100000; i++)
    {
        MineField mf(10, 10, 10);

        for (int i = 0; i < mf.getRows(); i++)
        {
            for (int j = 0; j < mf.getCols(); j++)
            {
                const Cell& cell = mf.getCell(i, j);

                bool top = i == 0;
                bool left = j == 0;

                bool right = j == (mf.getCols() - 1);
                bool bottom = i == (mf.getRows() - 1);

                bool is_zero = (cell.getValue() == 0);

                if ((top && left) || (top && right) ||
                    (bottom && left) || (bottom && right))
                {
                    corner_total++;
                    if (is_zero) corner_zeros++;
                }
                else if (top || left || bottom || right)
                {
                    edge_total++;
                    if (is_zero) edge_zeros++;
                }
                else
                {
                    center_total++;
                    if (is_zero) center_zeros++;
                }
            }
        }
    }

    printf("corner zeros = %.1f%%\n", 100.0 * corner_zeros / corner_total);
    printf("edge zeros   = %.1f%%\n", 100.0 * edge_zeros / edge_total);
    printf("center zeros = %.1f%%\n", 100.0 * center_zeros / center_total);

}


int main()
{
    srand(4);

#if 0
    Neighbours n;

    n.addLocation(Location(1, 1));
    n.addLocation(Location(1, 2));
    n.addLocation(Location(1, 3));

    Neighbours n2 = n;

    for (Location loc : n2)
    {
        printf("%d,%d\n", loc.row, loc.col);
    }
    return 0;
#endif

    AutoSweep      auto_sweep;
    SimulatedGame  game;

    SweepMain sm(&game, &auto_sweep);

    return sm.run(10000);
}
