#ifndef _MINE_FIELD_BUTTON_H
#define _MINE_FIELD_BUTTON_H

#include <QtGui>

class MineFieldWidget;   //forward declaration

class MineFieldButton : public QPushButton
{
    Q_OBJECT
private:
    int i, j;   //  location

public:         
    MineFieldButton(QWidget* parent, int _i, int _j);

    void setTag(int tag)
    {
        _tag = tag;
    }

    static int state;
    static MineFieldWidget* gui;
protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void paintEvent(QPaintEvent* e);
private:
    int  _tag;

};
#endif // _MINE_FIELD_BUTTON_H
