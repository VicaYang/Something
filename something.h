#ifndef SOMETHING_H
#define SOMETHING_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVector>
#include "USNParser.h"

namespace Ui {
class Something;
}

class Something : public QMainWindow
{
    Q_OBJECT

public:
    explicit Something(QWidget *parent = 0);
    ~Something();
    void initEngine();

public slots:
    void search();
private:
    Ui::Something *ui;
    QLineEdit* input;
    QPushButton* searchBtn;
    QTableWidget* table;
    QVector<USNParser*> drivers;
    void createUI();
    
};

#endif // SOMETHING_H
