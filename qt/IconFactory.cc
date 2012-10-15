
#include "IconFactory.h"
#include "utils.h"

IconFactory* IconFactory::ins = 0;
IconFactory::IconFactory()
{
    UNKNOWN = new QIcon(":/images/icons/UNKNOWN.png");
    MARKED = new QIcon(":/images/icons/MARKED.png");
    MINE = new QIcon(":/images/icons/MINE.png");
    for (int i = 0; i < 9; i++)
        NUM[i] = new QIcon(std::string(":/images/icons/" + int2str(i) + ".png").c_str());
}
IconFactory* IconFactory::getInstance()
{
    if (!ins)ins = new IconFactory();
    return ins;
}
const QIcon& IconFactory::getIcon(Cell& c) const
{
    int s = c.getState(), v = c.getValue();
    return this->getIcon(s, v);
}
const QIcon& IconFactory::getIcon(int s, int v, bool solved) const
{

    if (s == Cell::UNKNOWN)
    {
        if (solved) return *UNKNOWN2;
        return *UNKNOWN;
    }
    else if (s == Cell::FLAGGED)
        return *MARKED;
    else if (s == Cell::KNOWN)
    {
        if (v == Cell::MINE)
            return *MINE;
        else
            return *NUM[v];
    }
    fprintf(stderr, "Unknown state/value of cell\n");
    return *UNKNOWN;        //to silent warnings
}
