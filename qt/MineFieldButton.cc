
#include "MineFieldButton.h"
#include "MineFieldWidget.h"
#include "utils.h"

#include <QPainter>

int MineFieldButton::state = 0;
MineFieldWidget* MineFieldButton::gui = 0;

MineFieldButton::MineFieldButton(QWidget* parent, int _i, int _j) :
    QPushButton(parent), i(_i), j(_j),
    _tag(0)
{
    std::string name = "Button";
    name += int2str(_i);
    name += ":";
    name += int2str(_j);
    setObjectName(name.c_str());
}

void MineFieldButton::paintEvent(QPaintEvent* e)
{
    QPushButton::paintEvent(e);

    if (_tag != 0)
    {
        QBrush brush(Qt::Dense5Pattern);
        QPainter p(this);

        if (_tag == -1)
            brush.setColor(QColor(0, 0, 0));
        else if (_tag == 1)
            brush.setColor(QColor(200, 0, 0));
        else if (_tag == 2)
            brush.setColor(QColor(0, 200, 0));
        else if (_tag == 3)
            brush.setColor(QColor(0, 0, 200));
        else if (_tag == 4)
            brush.setColor(QColor(0, 200, 200));
        else
            brush.setColor(QColor(200, 200, 0));

        p.fillRect(rect(), brush);
    }
}

void MineFieldButton::mousePressEvent(QMouseEvent* e)
{
    //    printf("MineFieldButton: MOUSE %d PRESS %d,%d, state=%d\n", e->button(), i, j, state);

    try
    {
        if (e->button() == Qt::LeftButton)
        {
            gui->sinkWidgetsIfUnknown(i, j);
        }
        else if (e->button() == Qt::RightButton)
        {
            gui->toggle(i, j);
        }
    }
    catch (std::exception e)
    {
        //do nothing
    }

}

void MineFieldButton::mouseReleaseEvent(QMouseEvent* e)
{
    //    printf("MOUSE %d RELEASE %d,%d, state=%d\n", e->button(), i, j, state);

    try
    {
        if (e->button() == Qt::LeftButton)
        {
            gui->dig(i, j);
        }
    }
    catch (std::exception e)
    {
        //also do nothing
    }

}

