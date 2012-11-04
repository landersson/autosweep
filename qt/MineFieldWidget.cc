
#include "MineFieldWidget.h"
#include "MineFieldButton.h"

#include "IconFactory.h"
#include "MineField.h"
#include "QtGameLogic.h"

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


void MineFieldWidget::toggleBombsSlot()
{
    _show_bombs = !_show_bombs;
    updateGUISlot(true);
    printf("Show Bombs = %d\n", _show_bombs);
}



void MineFieldWidget::createGameLogic()
{
    if (_gl) delete _gl;

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


void MineFieldWidget::createMineField()
{
    int row = getMineField()->getRows();
    int col = getMineField()->getCols();


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

void MineFieldWidget::tagCells(const MineField::LocationList& locations, int tag)
{
    for (const Location & loc : locations)
    {
        printf("Edge @ %d,%d\n", loc.row, loc.col);
        MineFieldButton* b = buttons[loc.row][loc.col];
        b->setTag(tag);
    }
}

void MineFieldWidget::clearTags()
{
    for (int i = 0; i < _gl->getRows(); i++)
    {
        for (int j = 0; j < _gl->getCols(); j++)
        {
            MineFieldButton* b = buttons[i][j];
            b->setTag(0);
        }
    }
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


    //    printf("num edges = %zd\n", _auto_sweep->autoSweep().getEdges().size());

#if 0
    int tag = 1;
    for (const Neighbours & locations : _auto_sweep->autoSweep().getEdges())
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
    // initialize static variables
    MineFieldButton::state = 0;
    MineFieldButton::gui = this;

    printf("MineFieldWidget::newGame(%d, %d, %d)\n", row, col, num);

    this->_gl->newGame(row, col, num);
    this->createMineField();
    this->updateGUI(true);
}

bool MineFieldWidget::loadGame(const std::string& filename)
{
    // initialize static variables
    MineFieldButton::state = 0;
    MineFieldButton::gui = this;

    printf("MineFieldWidget::loadGame(%s)\n", filename.c_str());

    if (!this->_gl->loadGame(filename)) return false;

    this->createMineField();
    this->updateGUI(true);

    return true;
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


void MineFieldWidget::updateGUISlot(bool flag)
{
    updateGUI(flag);
}
