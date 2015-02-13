
#ifndef _SWEEP_MAIN_H
#define _SWEEP_MAIN_H

#include "GameInterface.h"
#include "SweepConfig.h"

#include "AutoSweep.h"

#include "log.h"

#include <map>

class SweepMain
{
public:
    SweepMain(GameInterface* game, AutoSweep* as) :
        _game(game),
        _auto_sweeper(as)
    { }

    bool verifyConfig(SweepConfig& config)
    {
        if (config.numRows() < 3 ||
            config.numCols() < 3) return false;

        // allow max 50% bombs
        int num_cells = config.numRows() * config.numCols();

        if (config.numMines() >= num_cells / 2) return false;

        if (config.numGames() < 0) return false;

        return true;
    }
    
    int run(SweepConfig& config)
    {
        if (!verifyConfig(config))
        {
            fprintf(stderr, "SweepMain: invalid configuration\n");
            exit(1);
        }
    
        Log::print(Log::INFO, "Running %d games, size=%dx%d, mines=%d\n", 
                   config.numGames(), config.numRows(), config.numCols(), config.numMines());
           

        int games_played = 0;
        int games_won = 0;

        std::map<int, int> lost_at_move;


        while (keepRunning() && games_played < config.numGames())
        {
#if 0
            if (config.isDefined("load"))
            {
                _game->loadGame(config.getString("load"));
            }
            else
            {
                _game->startNewGame(config.numRows(), config.numCols(), config.numMines());
            }
#else
            _game->startNewGame(config.numRows(), config.numCols(), config.numMines());
#endif

            _auto_sweeper->startNewGame(config.numMines());
            

            int moves_in_this_game = 0;

            while (_game->getGameState() == GameState::RUNNING)
            {
                _auto_sweeper->sweep(_game->getMineField());

                _game->verifyState(_auto_sweeper);

//                printf("actions = %zd\n", _auto_sweeper->getSweepActions().size());

                for (const SweepAction & action : _auto_sweeper->getSweepActions())
                {
                    _game->executeAction(action);

                    moves_in_this_game++;
                }            
            }

            if (_game->getGameState() == GameState::WON)
            {
                games_won++;
            }
            else
            {
                lost_at_move[moves_in_this_game]++;
            }

            games_played++;

            if ((games_played % 1) == 0)
            {
                //              printGamesWon(games_won, games_played);
            }
        }

#if 0
        float games_lost = games_played - games_won;

        for (auto x : lost_at_move)
        {
            printf("%3d : %3.2f\n", x.first, 100.0f * x.second / games_lost);
        }
#endif

        printGamesWon(games_won, games_played);

        return 0;
    }

    virtual bool keepRunning()
    {
        return true;
    }

protected:
    void printGamesWon(int games_won, int games_played)
    {
        Log::print(Log::INFO, "Done: won %d/%d games: %.3f%%\n", games_won, games_played,
                   100.0 * games_won / games_played);
    }
    
    int             _games_played;
    int             _games_won;

    GameInterface*  _game;
    AutoSweep*      _auto_sweeper;

};


#endif // _SWEEP_MAIN_H
