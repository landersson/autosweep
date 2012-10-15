
#ifndef _Q_GUI_CONTROLLER_H_
#define _Q_GUI_CONTROLLER_H_

#include "MineFieldWidget.h"
#include "GameInterface.h"

#include <QCursor>
#include <QMouseEvent>
#include <QMainWindow>

#include <cassert>

class QtGameView : public QWidget, public GameInterface
{
public:
    enum class MouseButton
    {
        LEFT,
        RIGHT
    };


    QtGameView() :
        _origin_x(0),
        _origin_y(0)
    {
        this->setWindowIcon(QIcon(":/images/img/logo.png"));

        QVBoxLayout* top_layout = new QVBoxLayout(this);

        _main_window.show();
        _main_window.raise();

        top_layout->addWidget(&_main_window);


        this->move(300, 200);
        //        this->resize(200, 200);
        //        this->setCentralWidget(&_main_window);


        this->show();

    }

    virtual void startNewGame(int rows, int cols, int num_mines)
    {
        _main_window.newGame(rows, cols, num_mines);
    }

    virtual void executeAction(const SweepAction& action)
    {

        if (_main_window.getMineField()->getCell(action.getRow(), action.getCol()).getState() !=
            Cell::UNKNOWN)
        {
            return;
        }

        switch (action.getAction())
        {
        case SweepAction::FLAG_CELL:
            flagCell(action);
            break;
        case SweepAction::CLICK_CELL:
            clickCell(action);
            break;
        default:
            assert(0);
        }

        pauseGui(10);

    }

    const MineField* getMineField()
    {
        return _main_window.getMineField();
    }

    virtual GameState getGameState() const
    {
        //        return GameState::STOP;
        return _main_window.getState();
    }



protected:
    void flagCell(const SweepAction& a)
    {
        //        printf("FLAGGING CELL %d,%d\n", a.getCol(), a.getRow());
        moveCursor(a.getLocation());
        click(a.getLocation(), MouseButton::RIGHT);
    }

    void clickCell(const SweepAction& a)
    {
        //        printf("DIGGING CELL %d,%d\n", a.getCol(), a.getRow());
        moveCursor(a.getLocation());
        click(a.getLocation(), MouseButton::LEFT);
    }


    void pauseGui(int ms)
    {
        QTime time_to_die = QTime::currentTime().addMSecs(ms);
        while (QTime::currentTime() < time_to_die)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    void moveCursor(const Location& location)
    {
        QPoint cursor_pos = QPoint(this->pos().x() + _main_window.cellToPixelX(location.col),
                                   this->pos().y() + _main_window.cellToPixelY(location.row));

        QCursor::setPos(cursor_pos);
    }

    void click(const Location& location, MouseButton button)
    {
        QWidget* child = _main_window.childAt(location.col, location.row);

        if (!child) return;

        assert(child);



        Qt::MouseButton qt_button;

        if (button == MouseButton::RIGHT)
            qt_button = Qt::RightButton;
        else
            qt_button = Qt::LeftButton;

        QMouseEvent event(QEvent::MouseButtonPress, QCursor::pos(), qt_button, 0, 0);
        event.setAccepted(true);
        QApplication::sendEvent(child, &event);

        QMouseEvent event2(QEvent::MouseButtonRelease, QCursor::pos(), qt_button, 0, 0);
        event2.setAccepted(true);
        QApplication::sendEvent(child, &event2);
    }



private:
    MineFieldWidget _main_window;

    int _origin_x;
    int _origin_y;
};


#endif // _Q_GUI_CONTROLLER_H_
