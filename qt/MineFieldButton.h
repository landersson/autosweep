#ifndef MSBUTTON_H
#define MSBUTTON_H

#include <QtGui>

class MineFieldWidget;   //forward declaration

class MineFieldButton : public QPushButton
{
    Q_OBJECT
    /*
        before starting new game, state must b 0, gui must be initialized
    */
private:
    int i, j;   //  location

public:         //  in order to initialize...
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
#endif // MSBUTTON_H
