
#include "SweepMain.h"
#include "AutoSweep.h"
#include "SimulatedGame.h"

#include "Neighbours.h"

#include "utils.h"

// experiment to determine probabilities of opening a zero in corner/edge/center cells
void test_zero_probabilities()
{
    int corner_total = 0;
    int edge_total = 0;
    int center_total = 0;


    int corner_zeros = 0;
    int edge_zeros = 0;
    int center_zeros = 0;


    for (int g = 0; g < 100000; g++)
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

    printf("Corner/edge/center empty cell probabilities:\n");
    printf("corner zeros = %.1f%%\n", 100.0 * corner_zeros / corner_total);
    printf("edge zeros   = %.1f%%\n", 100.0 * edge_zeros / edge_total);
    printf("center zeros = %.1f%%\n", 100.0 * center_zeros / center_total);

}

void test_rand_int()
{
    const int size = 10;
    int histogram[size];

    for (int i = 0; i < size; i++) histogram[i] = 0;

    for (int i = 0; i < 100000000; i++)
    {
        int x = rand_int(0, size - 1);

        histogram[x]++;
    }

    printf("Random distribution 0-%d:\n", size);
    for (int i = 0; i < size; i++) 
    {
        printf("%d: %d\n", i, histogram[i]);
    }
}


int main()
{
    srand(5);
   
    test_zero_probabilities();
    test_rand_int();

    AutoSweep      auto_sweep;
    SimulatedGame  game;

    SweepMain sm(&game, &auto_sweep);

    return sm.run(10000);
}
