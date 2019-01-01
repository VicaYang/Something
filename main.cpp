#include "something.h"
#include <QApplication>
#include <string>
#include <iostream>
#pragma warning (disable: 4267)

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  Something w;
  w.show();
  a.processEvents();
  w.initEngine();
  return a.exec();
}
