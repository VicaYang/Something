#ifndef SOMETHING_H
#define SOMETHING_H

#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QLineEdit>
#include "USNParser.h"
#include "Monitor.h"
#include "FileIndex.h"
#include <windows.h>
#include <QCloseEvent>
#include "Searcher.h"
#include "HTMLDelegate.h"

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
private slots:
  void search();
  void recvPUSN(int id, PUSN_RECORD pusn);
  void buildIndexSlot();
private:
  Ui::Something *ui;
  QLineEdit* input;
  QPushButton* searchBtn;
  QTableView* table;
  QStandardItemModel* model;
  HTMLDelegate* delegate;
  std::vector<char> _drivers = { 'D' };
  std::vector<USNParser*> drivers;
  std::vector<Monitor*> monitors;
  std::vector<FileIndex*> fileindexs;
  Searcher* searcher;
  QMenu* menu;
  QAction* buildIndex;
  void createUI();

};
Q_DECLARE_METATYPE(PUSN_RECORD);
#endif // SOMETHING_H
