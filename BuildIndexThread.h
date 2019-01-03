#pragma once
#include <QThread>
#include "FileIndex.h"

class BuildIndexThread : public QThread
{
  Q_OBJECT
public:
  BuildIndexThread(std::set<FileEntry*>&& files, USNParser* driver, FileIndex* index);
  void run();
signals:
  void resume();
  void setValue(int);
  void setLabel(QString);
  void enableBtn(bool);
private:
  std::set<FileEntry*> files;
  USNParser* driver;
  FileIndex* index;
};


