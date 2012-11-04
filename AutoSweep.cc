
#include "AutoSweep.h"
#include "SharedObject.h"

#include "log.h"

void AutoSweep::updateModel(const MineField* mf)
{
    if (_model.empty())
    {
        _model.allocCells(mf->getRows(), mf->getCols());
        _model.resetCells();
        Log::print(Log::VERBOSE, "ConstraintSweep::updateModel() initializing model size = %d,%d\n",
                   _model.getRows(), _model.getCols());
    }
    for (int i = 0; i < mf->getRows(); i++)
    {
        for (int j = 0; j < mf->getCols(); j++)
        {
            Cell& m = _model.getCell(i, j);
            const Cell& r = mf->getCell(i, j);
            m.setValue(r.getValue());
            m.setState(r.getState());
        }
    }
}

AutoSweep* AutoSweep::loadSolver(const std::string& name)
{
    SharedObject solver_so;

    std::string full_name = "lib";

    full_name += name;
    full_name += ".";
    full_name += SharedObject::sharedObjectExtension();

    if (!solver_so.open(full_name.c_str()))
    {
        Log::print(Log::ERROR, "Error opening solver shared object '%s' : %s\n",
                   full_name.c_str(), solver_so.lastError().c_str());
        return 0;
    }
    
    AutoSweep *solver = solver_so.createObject<AutoSweep>("initialize", "");
    if (solver == NULL)
    {
        Log::print(Log::ERROR, "Error initializing solver '%s' : %s\n",
                   full_name.c_str(), solver_so.lastError().c_str());
        return 0;
    }

//    lcc::Log::verbose("bootstrap: loaded scanner dso '%s'", scanner_dso_path.c_str());

    return solver;
}
