
#include "MineFieldWidget.h"
#include "MineFieldButton.h"

#include "IconFactory.h"
#include "MineField.h"
#include "QtGameLogic.h"

#include <QMainWindow>

#include <QBoxLayout>

#include <stdexcept>

MineFieldWidget::MineFieldWidget() :
    QWidget(0),
    _gl(0),
    _layout(0),
    _frame(0),
    _show_bombs(false)
{
    _config_index = 0;

    createGameLogic();

    setMouseTracking(true);
    this->newGame(4, 4, 1);    //start a new game by default

}

MineFieldWidget::~MineFieldWidget()
{
    _gl->deleteLater();
}



void MineFieldWidget::newGameSlot()
{
    int row = 12;
    int col = 12;
    int num = 2;

    this->createGameLogic();
    this->newGame(row, col, num);

}

bool MineFieldWidget::loadMineField(const char* filename)
{
    FILE* fp = fopen(filename, "r");

    if (fp == 0)
    {
        fprintf(stderr, "MineFieldWidget: error opening file '%s'\n", filename);
        return false;
    }

    std::vector<std::string> rows;

    char buffer[512];
    while (0 != fgets(buffer, sizeof(buffer), fp))
    {
        rows.push_back(buffer);
        printf("%s", buffer);
    }

    fclose(fp);

    if (!_gl->loadGame(rows))
    {
        fprintf(stderr, "MineFieldWidget: error loading minefield from file '%s'\n", filename);
        return false;
    }
    int row = _gl->getRows();
    int col = _gl->getCols();
    int num = _gl->getNumMines();

    printf("%dx%dx%d\n", row, col, num);

    this->updateGUI(true);

    //    this->createNormalGameLogic();

    return true;
}

void MineFieldWidget::toggleBombsSlot()
{
    _show_bombs = !_show_bombs;
    updateGUISlot(true);
    printf("Show Bombs = %d\n", _show_bombs);
}



void MineFieldWidget::createGameLogic()
{
    if (_gl)delete _gl;

    _gl = new QtGameLogic();

    connect(_gl, SIGNAL(winSignal()), this, SLOT(winSlot()));
    connect(_gl, SIGNAL(loseSignal()), this, SLOT(loseSlot()));
}


void MineFieldWidget::initializeWidgets()
{
    //    if (this->central) delete this->central;
    //    this->central=new QWidget(this);
    //    this->setCentralWidget(this->central);

}


void MineFieldWidget::createMineField(int row, int col)
{
    printf("MineFieldWidget::createMineField(%d, %d)\n", row, col);

    for (auto bv : buttons)
        for (auto b : bv)
            delete b;

    buttons.clear();

    delete _frame;
    delete _layout;

    _frame = new QFrame(this);
    _frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    _frame->setGeometry(QRect(0, 0, 20 * col, 20 * row));

    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(QMargins(2, 2, 2, 2));

    _layout->addWidget(_frame);

    setLayout(_layout);

    buttons.resize(row);

    for (int i = 0; i < row; i++)
    {
        buttons[i].resize(col);
    }

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            MineFieldButton* b = new MineFieldButton(this->_frame, i, j);
            b->setGeometry(QRect(j * 20, i * 20, 21, 21));
            b->setIcon(IconFactory::getInstance()->getIcon(Cell::UNKNOWN, 0)); //initialize icon
            buttons[i][j] = b;
        }
    }

}


void MineFieldWidget::mouseMoveEvent(QMouseEvent* e)
{
    int gx = e->x() / 20;
    int gy = e->y() / 20;

    printf("%d,%d", gx, gy);
}

void MineFieldWidget::updateGUI(bool rebuildMineField)
{
    int row = _gl->getRows(), col = _gl->getCols();

    printf("MineFieldWidget::updateGUI(%d, %d)\n", row, col);

    if (rebuildMineField)
    {
        _frame->setFixedSize(col * 20 + 4, row * 20 + 4);
    }

    //    printf("Frame Size = %dx%d\n", _frame->size().width(), _frame->size().height());
    //    printf("This  Size = %dx%d\n", size().width(), size().height());

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            MineFieldButton* b = buttons[i][j];
            b->setTag(0);
        }
    }

    //    printf("num edges = %zd\n", _auto_sweep->autoSweep().getEdges().size());

#if 0
    int tag = 1;
    for (const MineField::LocationList & locations : _auto_sweep->autoSweep().getEdges())
    {
        for (Location loc : locations)
        {
            //            printf("Edge @ %d,%d\n", loc.row, loc.col);
            MineFieldButton* b = buttons[loc.row][loc.col];
            b->setTag(tag);
        }
        tag++;
    }

    std::vector<Location> config = _auto_sweep->autoSweep().getEdgeConfig(_config_index);

    //    printf("Edge Config %d = %zd\n", _condig_index, config.size());

    if (config.size() == 0)
    {
        _config_index = 0;
    }

    for (Location loc : config)
    {
        printf("Bomb at %d,%d\n", loc.row, loc.col);
        MineFieldButton* b = buttons[loc.row][loc.col];
        b->setTag(-1);
    }
#endif

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            MineFieldButton* b = buttons[i][j];
            Cell& c = _gl->getCell(i, j);

            int s = c.getState();
            int v = c.getValue();

            if (_show_bombs && v == Cell::MINE)
            {
                s = Cell::KNOWN;
            }

            //            if(boardCopy[i][j] !=c)
            {
                b->setIcon(IconFactory::getInstance()->getIcon(s, v));
                //                boardCopy[i][j] = c;
            }
        }
    }
    //    updateStatusBar();
}

void MineFieldWidget::newGame(int row, int col, int num)
{
    /**initialize static variables**/
    MineFieldButton::state = 0;
    MineFieldButton::gui = this;
    /******************************/
    printf("newGame(%d, %d, %d)\n", row, col, num);

    this->_gl->newGame(row, col, num);
    this->createMineField(row, col);
    this->updateGUI(true);
}

void MineFieldWidget::winSlot()
{
    this->updateGUI();

    QMessageBox::information(this, "You Win!",
                             tr("<h3>Congratulations! You win!</h3>"));
}

void MineFieldWidget::loseSlot()
{
    this->updateGUI();
    QMessageBox::information(this, "You Lose",
                             tr("<h3>Sorry, You lose.</h3>"
                                "<p>better luck next time</p>"));
}


void MineFieldWidget::dig(int i, int j)
{
    this->_gl->dig(i, j);
    this->updateGUI();
}

#if 1
void MineFieldWidget::mark(int i, int j)
{
    this->_gl->mark(i, j);
    this->updateGUI();
}


void MineFieldWidget::unmark(int i, int j)
{
    this->_gl->unmark(i, j);
    this->updateGUI();
}
#endif

void MineFieldWidget::toggle(int i, int j)
{
    int s = this->_gl->getCell(i, j).getState();
    if (s == Cell::FLAGGED)
        this->_gl->unmark(i, j);
    else if (s == Cell::UNKNOWN)
        this->_gl->mark(i, j);
    this->updateGUI();
}

void MineFieldWidget::sinkWidgetsIfUnknown(int i, int j)
{
    printf("msWidget::sinkWidgetsIfUnknown(%d,%d)\n", i, j);
    if (_gl->getCell(i, j).getState() == Cell::UNKNOWN)
    {
        printf("MineFieldWidget::sinking %d, %d\n", i, j);
        buttons[i][j]->setIcon(IconFactory::getInstance()->getIcon(Cell::KNOWN, 0));
    }
}


#if 0
void MineFieldWidget::explore(int i, int j)
{
    printf("MineFieldWidget::explore(%d, %d)\n", i, j);

    this->_gl->explore(i, j);
    this->updateGUI();
}

void MineFieldWidget::raiseNeighbourWidgets(int i, int j)
{
    printf("MineFieldWidget::raiseNeighboursIfUnknowns(%d, %d)\n", i, j);

    MineField::LocationList lst = this->_gl->getNeighbours(i, j);
    for (size_t k = 0; k < lst.size(); k++)
    {
        MineFieldButton* b = this->buttons[lst[k].row][lst[k].col];
        if (this->_gl->getCell(lst[k].row, lst[k].col).getState() == Cell::UNKNOWN)
        {
            b->setIcon(IconFactory::getInstance()->getIcon(Cell::UNKNOWN, 0));
        }
    }
}


void MineFieldWidget::raiseWidgetsIfUnknown(int i, int j)
{
    printf("MineFieldWidget::raiseWidgetsIfUnknowns(%d, %d)\n", i, j);

    if (_gl->getCell(i, j).getState() == Cell::UNKNOWN)
        buttons[i][j]->setIcon(IconFactory::getInstance()->getIcon(Cell::UNKNOWN, 0));
}

void MineFieldWidget::sinkNeighbourWidgets(int i, int j)
{
    printf("MineFieldWidget::sinkNeighbourWidgets(%d, %d)\n", i, j);

    MineField::LocationList lst = this->_gl->getNeighbours(i, j);
    for (size_t k = 0; k < lst.size(); k++)
    {
        MineFieldButton* b = this->buttons[lst[k].row][lst[k].col];
        if (this->_gl->getCell(lst[k].row, lst[k].col).getState() == Cell::UNKNOWN)
        {
            b->setIcon(IconFactory::getInstance()->getIcon(Cell::KNOWN, 0));
        }
    }
}
#endif
void MineFieldWidget::updateGUISlot(bool flag)
{
    updateGUI(flag);
}
