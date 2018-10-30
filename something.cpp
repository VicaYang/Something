#include "something.h"
#include "ui_something.h"

Something::Something(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Something)
{
    ui->setupUi(this);
}

Something::~Something()
{
    delete ui;
}
