#ifndef SOMETHING_H
#define SOMETHING_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVector>
#include "USNParser.h"
#include "Monitor.h"
#include <windows.h>
#include <QCloseEvent>

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
protected:
  void closeEvent(QCloseEvent *e);
public slots:
    void search();
    void recvPUSN(int id, PUSN_RECORD pusn);
private:
    Ui::Something *ui;
    QLineEdit* input;
    QPushButton* searchBtn;
    QTableWidget* table;
    QVector<USNParser*> drivers;
    QVector<Monitor*> monitors;
    void createUI();
    
};
Q_DECLARE_METATYPE(PUSN_RECORD);

#endif // SOMETHING_H
