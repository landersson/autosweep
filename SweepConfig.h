
#ifndef _SWEEP_CONFIG_H
#define _SWEEP_CONFIG_H

#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#include <stdexcept>


// very simple command line and options file parsing 
class Config
{
public:
    bool initialize(int argc, const char* const argv[])
    {
        for (int i = 1; i < argc; i++)
        {
            if (!parseOption(argv[i])) return false;
        }
        
        if (_options.find("config") != _options.end())
        {
            if (!loadOptions(_options["config"])) return false;
        }
        return true;
    }

    bool isDefined(const std::string& opt_name) const
    {
        return (_options.find(opt_name) != _options.end());        
    }

    const std::string& getString(const std::string& opt_name) const
    {
        std::map<std::string, std::string>::const_iterator it = _options.find(opt_name);

        if (it == _options.end())
        {
            fprintf(stderr, "Config: ERROR, unknown option '%s'\n", opt_name.c_str());
            exit(1);
        };

        return it->second;
    }

    int getInteger(const std::string& opt_name) const
    {                
        std::string value = getString(opt_name);

        try 
        {
            return std::atoi(value.c_str());
        }
        catch(std::invalid_argument)
        {
            fprintf(stderr, "Config: Error, invalid integer value '%s'\n",
                    value.c_str());
            exit(1);
        }
    }

    int getInteger(const std::string& opt_name, int default_value) const
    {
        if (!isDefined(opt_name)) return default_value;

        return getInteger(opt_name);
    }
        

private:

    bool loadOptions(const std::string& filename)
    {
        fprintf(stderr, "SweepConfig: loading options from file '%s'\n", filename.c_str());

        std::ifstream cfgfile;

        cfgfile.open(filename.c_str());

        if (cfgfile.fail())
        {
            fprintf(stderr, "SweepConfig:: ERROR, can't load config from file '%s'\n", 
                    filename.c_str());
            return false;
        }
        
        while(cfgfile.good())
        {
            std::string option;
            cfgfile >> option;
            
            if (!option.empty()) 
            {
                if (!parseOption(option, false)) 
                {
                    fprintf(stderr, "SweepConfig: Error reading options from file '%s'\n",
                            filename.c_str());
                    return false;
                }
            }
        }
        
        cfgfile.close();
        return true;
    }

    bool parseOption(const std::string &option, bool overwrite = true)
    {
        // options must be at least as long as 'a=b'
        if (option.length() < 3)
        {
            printErrorMessage(option);
            return false;
        }
        
        std::string::size_type equal_char_pos = option.find('=');
        
        // no equal char found, return error
        if (equal_char_pos == std::string::npos)
        {
            printErrorMessage(option);
            return false;
        }
        
        std::string opt_name = option.substr(0, equal_char_pos); 
        std::string opt_value = option.substr(equal_char_pos + 1); 
        
        if (opt_name.size() < 1 || opt_value.size() < 1)
        {
            printErrorMessage(option);
            return false;
        }
        
        bool option_exists_already = (_options.find(opt_name) != _options.end());

        if (!(option_exists_already && !overwrite))
        {
            _options[opt_name] = opt_value;
        }
        return true;
    }

   
    void printErrorMessage(const std::string &option)
    {
        fprintf(stderr, "Config: ERROR: invalid option '%s'\n", option.c_str());
        fprintf(stderr, "Config:        options must be specified as name=value\n");
    }
private:
    std::map<std::string, std::string> _options;

};


// minesweeper tailored Config subclass
class SweepConfig : public Config
{
public:
    // initialize default sweep configuration
    SweepConfig() :
        _rows(16),
        _cols(16),
        _num_mines(52),
        _num_games(1)
    { 
        // default sweeper configuration: Microsoft Minesweeper Expert mode
        const char* default_options[] = {
            "rows=16",
            "cols=30",
            "mines=99",
            "games=1",
            "seed=1",
            "safe-open=0",
            "log-level=3",
            "solver=constraint-sweep"
        };

        int num_default_opts = sizeof(default_options) / sizeof(char *);

        Config::initialize(num_default_opts, default_options);
    }

    

    bool initialize(int argc, const char* const argv[])
    {
        if (!Config::initialize(argc, argv)) return false;

        _rows = getInteger("rows", _rows);
        _cols = getInteger("cols", _cols);
        _num_mines = getInteger("mines", _num_mines);
        _num_games = getInteger("games", _num_games);

        return true;
    }
    
    int numRows() const  { return _rows; }
    int numCols() const  { return _cols; }
    int numMines() const { return _num_mines; }
    int numGames() const { return _num_games; }

    bool safeOpen() const { return getInteger("safe-open") != 0; } 

private:
    
    int _rows;
    int _cols;
    int _num_mines;
    int _num_games;

};

#endif // _SWEEP_CONFIG_H
