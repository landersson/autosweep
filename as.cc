
#include "SweepMain.h"
#include "Config.h"
#include "SweepConfig.h"
#include "SimulatedGame.h"
#include "Neighbours.h"

#include "solvers/ConstraintSweep.h"

#include "utils.h"
#include "log.h"

#include <boost/math/special_functions/binomial.hpp>

void test_rand_int();
void test_zero_probabilities();
void test_Pn(int argc, char *argv[]);

const std::vector<Option> options = 
{
    { "rows",      Option::Type::INTEGER, "16",    
      "number of rows in minefield" },
    { "cols",      Option::Type::INTEGER, "30",    
      "number of columns in minefield" },
    { "mines",     Option::Type::INTEGER, "99",    
      "number of mines in minefield" },
    { "games",     Option::Type::INTEGER, "1",     
      "number of games to play" },
    { "seed",      Option::Type::INTEGER, "99",    
      "initial random number generator seed" },
    { "safe-open", Option::Type::BOOLEAN, "false", 
      "if true, first cell will never contain a bomb" },
    { "log-level", Option::Type::INTEGER, "3",     
      "logging verbosity" },
    { "solver",    Option::Type::STRING, "constraint-sweep",
      "name of the solver algorithm to load and use" }
};

int main(int argc, char* argv[])
{   
    //test_zero_probabilities();
    //test_rand_int();
//    test_Pn(argc, argv);
//    return 0;
#if 0
    unsigned max = 300;
    
    for (unsigned i = 0; i <= max; i++)
    {       
        double x = utils::binomial(max, i);
        double y = boost::math::binomial_coefficient<double>(max, i);
        double diff;
        
        diff = x - y;

        printf("%f - %f = %f\n", x, y, diff);
    }
    return 0;
#endif
    Configuration config;

    config.parse(argc, argv, options);

    config.printHelp(options);
//    exit(1);
    SweepConfig swconfig;

    if (!swconfig.initialize(config)) return 1;

//    printf("level = %d\n", config.getInteger("log-level"));
    
    Log::setLevel(config.getInteger("log-level"));
    Log::setOutputStream(stdout);

    srand(config.getInteger("seed"));

    AutoSweep* auto_sweep = AutoSweep::loadSolver(config.getString("solver"));

    if (auto_sweep == nullptr) return 1;

    SimulatedGame game;

    SweepMain sm(&game, auto_sweep);

    return sm.run(swconfig);
}

// Given two sets A and B with An and Bn empty slots in each,
// distribute N balls randomly over both sets (probability of ball
// in slot = N / (An + Bn) ). Now, what is the probability P(n),
// where n is in the range [0,...,min(An, N)], that set A has n
// balls in it?
double calcP(int n, int N, int An, int Bn)
{
    // P(n) = binomial(An, n) * binomial(Bn, N-n) / binomial(An + Bn, N)

    return boost::math::binomial_coefficient<double>(An, n) *
        boost::math::binomial_coefficient<double>(Bn, N - n)
        / boost::math::binomial_coefficient<double>(An + Bn, N);
}



void test_Pn(int argc, char* argv[])
{
    // 1, 99, 5, 472

    

//    double sum2 = 0;
    for (unsigned i = 1; i <= 2; i++)
    {
        double x = calcP(i, 4, 3, 6+3);

        printf("%d = %f\n", i, x);
    }
//    printf("sum = %f\n", sum2);

    return;

    const int total_loops = 9000000;

    const int total_cells = atoi(argv[1]);
    const int total_known = atoi(argv[2]);
    const int total_bombs = atoi(argv[3]);

    assert(total_cells > total_bombs);

    // bomb frequence histogram
    int bomb_freq[total_bombs + 1];
    memset(bomb_freq, 0, sizeof(bomb_freq));

    for (int loops = 0; loops < total_loops; loops++)
    {
        bool cells[total_cells];
        memset(cells, 0, sizeof(cells));

        // generate randomized list of indices 0..total_cells - 1
        int indices[total_cells];
        for (int i = 0; i < total_cells; i++)
        {
            indices[i] = i;
        }
        std::random_shuffle(indices, indices + total_cells);
        
        // flag bombs beloging to "known cells"
        for (int i = 0; i < total_bombs; i++)
        {
//            printf("i=%d\n", indices[i]);
            cells[indices[i]] = true;
        }

        // count number of bombs in "known cells"
        int num_bombs = 0;
        for (int i = 0; i < total_known; i++)
        {
            if (cells[i]) num_bombs++;
        }
//        printf("n = %d\n", num_bombs);
        // update bomb frequency histogram
        bomb_freq[num_bombs]++;
    }

    int max_bombs = std::min(total_bombs, total_known);

    double sum = 0;

    for (int i = 0; i <= max_bombs; i++)
    {
        // statistical probability
        double probability = 1.0 * bomb_freq[i] / total_loops;

        // some formula to calculate this directly?
#if 0
        double probability2 = binomialCoefficient(total_known, i) *
            binomialCoefficient(total_cells - total_known, total_bombs - i)
            / binomialCoefficient(total_cells, total_bombs);

        double probability3 = boost::math::binomial_coefficient<double>(total_known, i) * boost::math::binomial_coefficient<double>(total_cells - total_known, total_bombs - i) / boost::math::binomial_coefficient<double>(total_cells, total_bombs);
#endif        
//        printf("%d %.6f  %.6f  %.6f\n", i, probability, probability2, probability3);
        sum += probability;
    }
    printf("Ptotal = %.4f\n", sum);
}


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

// used to verify random number generation
void test_rand_int()
{
    const int size = 10;
    int histogram[size];

    for (int i = 0; i < size; i++) histogram[i] = 0;

    for (int i = 0; i < 100000000; i++)
    {
        int x = utils::rand_int(0, size - 1);

        histogram[x]++;
    }

    printf("Random distribution 0-%d:\n", size);
    for (int i = 0; i < size; i++) 
    {
        printf("%d: %d\n", i, histogram[i]);
    }
}
