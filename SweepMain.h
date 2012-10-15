
#ifndef _SWEEP_MAIN_H
#define _SWEEP_MAIN_H

#include "GameInterface.h"
#include "AutoSweep.h"

#include <map>

class SweepMain
{
public:
    SweepMain(GameInterface* game, AutoSweep* as) :
        _game(game),
        _auto_sweeper(as)
    { }

    int run(int num_games_to_play)
    {
        int games_played = 0;
        int games_won = 0;

        std::map<int, int> lost_at_move;

        while (keepRunning() && games_played < num_games_to_play)
        {
            _game->startNewGame(16, 16, 52);

            int moves_in_this_game = 0;

            while (_game->getGameState() == GameState::RUN)
            {
                _auto_sweeper->sweep(_game->getMineField());

                //                printf("actions = %zd\n", _auto_sweeper->getSweepActions().size());

                for (const SweepAction & action : _auto_sweeper->getSweepActions())
                {
                    _game->executeAction(action);

                    moves_in_this_game++;
                }
            }


            if (_game->getGameState() == GameState::WIN)
            {
                games_won++;
            }
            else
            {
                lost_at_move[moves_in_this_game]++;
            }

            games_played++;

            if ((games_played % 10) == 0)
            {
                //                printGamesWon(games_won, games_played);
            }
        }

        float games_lost = games_played - games_won;

        for (auto x : lost_at_move)
        {
            printf("%3d : %3.2f\n", x.first, 100.0f * x.second / games_lost);
        }

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
        printf("won %d/%d games: %.3f%%\n", games_won, games_played,
               100.0 * games_won / games_played);
    }


    int             _games_played;
    int             _games_won;


    GameInterface*  _game;
    AutoSweep*      _auto_sweeper;

};


#endif // _SWEEP_MAIN_H
