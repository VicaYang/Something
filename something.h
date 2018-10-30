#ifndef SOMETHING_H
#define SOMETHING_H

#include <QMainWindow>

namespace Ui {
class Something;
}

class Something : public QMainWindow
{
    Q_OBJECT

public:
    explicit Something(QWidget *parent = 0);
    ~Something();

private:
    Ui::Something *ui;
};

#endif // SOMETHING_H
