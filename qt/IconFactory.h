#ifndef _ICON_FACTORY_H
#define _ICON_FACTORY_H

class Cell;

#include <QIcon>

//singleton pattern, so each icon is also unique
class IconFactory
{
private:
    static IconFactory* ins;
    IconFactory();
    QIcon* UNKNOWN, *UNKNOWN2, *MARKED, *MINE, *NUM[9];
public:
    static IconFactory* getInstance();
    const QIcon& getIcon(Cell& c) const;
    const QIcon& getIcon(int state, int value, bool solved = false) const;
};

#endif // _ICON_FACTORY_H
