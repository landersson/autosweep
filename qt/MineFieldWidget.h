#ifndef _MINE_FIELD_WIDGET_H
#define _MINE_FIELD_WIDGET_H

#include "QtGameLogic.h"
#include "Cell.h"

#include <QtGui>

#include <vector>

class MineFieldButton;
class QAutoSweep;

class MineFieldWidget : public QWidget
{
    Q_OBJECT
public:
    MineFieldWidget();
    ~MineFieldWidget();

    void updateGUI(bool rebuildMineField = false);

    void newGame(int row, int col, int num);
    bool loadGame(const std::string& filename);

    void dig(int i, int j);
    void mark(int i, int j);
    void unmark(int i, int j);
    void explore(int i, int j);
    void toggle(int i, int j);
    void sinkWidgetsIfUnknown(int i, int j);

    void createMineField();
    void createGameLogic();
    void createCompeteGameLogic();
    void initializeWidgets();

    int cellToPixelX(int cell_x) const
    {
        return this->pos().x() + cell_x * 20 + 12;
    }

    int cellToPixelY(int cell_y) const
    {
        return this->pos().y() + cell_y * 20 + 32;
    }

    const MineField* getMineField() const
    {
        return _gl->getMineField();
    }

    const GameLogic* getGameLogic() const
    {
        return _gl;
    }


    QWidget* childAt(int x, int y)
    {
        return QWidget::childAt(cellToPixelX(x) - this->pos().x(),
                                cellToPixelY(y) - 20 - this->pos().y());
    }

    GameState getState() const
    {
        return _gl->getState();
    }

    // debugging visualization support
    void tagCells(const MineField::LocationList& locations, int tag);
    void clearTags();

private:
    std::vector<std::vector<MineFieldButton*> > buttons;
    std::vector<std::vector<Cell> > boardCopy;

    QtGameLogic* _gl;

    QBoxLayout* _layout;
    QFrame*     _frame;

    bool        _show_bombs;

    int         _config_index;

    void mouseMoveEvent(QMouseEvent* e);

public slots:
    void newGameSlot();
    void winSlot();
    void loseSlot();
    void updateGUISlot(bool flag);

    void toggleBombsSlot();
};

#endif // _MINE_FIELD_WIDGET_H
