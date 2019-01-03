#pragma once
#include <QThread>
#include <QtCore>
#include <QObject>
#include <QMutex>
#include <windows.h>
#include <memory>

class Monitor : public QThread
{
  Q_OBJECT
public:
  Monitor(int id, HANDLE root_handle, USN_JOURNAL_DATA journal);
  Monitor(const Monitor&) = delete;
  Monitor& operator=(const Monitor&) = delete;
signals:
  void sendPUSN(int id, PUSN_RECORD pusn);

private slots:
  void run();

private:
  HANDLE root_handle;
  USN_JOURNAL_DATA journal;
  USN last_usn;
  int id;
  std::unique_ptr<READ_USN_JOURNAL_DATA> getNext(USN start_usn, int byte_wait);
  bool waitNext(PREAD_USN_JOURNAL_DATA data) const;
};

