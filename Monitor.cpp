#include "Monitor.h"
#include <QDebug>
#include <string>

Monitor::Monitor(int id, HANDLE root_handle, USN_JOURNAL_DATA journal) : id(id), root_handle(root_handle), journal(journal) {
  last_usn = journal.NextUsn;
}

void Monitor::run() {
    while(true) {
      constexpr int BUFLEN = 1 << 18;
      CHAR buffer[BUFLEN];
      auto query = getNext(last_usn, 1);
      while (true) {
        waitNext(query.get());
        auto journal_query = getNext(query->StartUsn, 0);
        DWORD br;
        while (DeviceIoControl(root_handle, FSCTL_READ_USN_JOURNAL,
          journal_query.get(), sizeof(*journal_query.get()),
          buffer, BUFLEN,
          &br, nullptr)) {
          if (br == 8)break;
          auto PUsnRecord = (PUSN_RECORD)(((PCHAR)buffer) + sizeof(USN));
          DWORD dwRetBytes = br - sizeof(USN);
          while (dwRetBytes>0) {
            if (PUsnRecord->FileName[0] != L'$' && PUsnRecord->FileName[0] != L'~') {
              qDebug() << "-----------------\n";
              qDebug() << "Filename " << QString::fromWCharArray(PUsnRecord->FileName, PUsnRecord->FileNameLength / 2) << "\n";
              qDebug() << "Reason " << QString("%1").arg(PUsnRecord->Reason, 0, 16) << "\n";
              qDebug() << "FileRef " << PUsnRecord->FileReferenceNumber << "\n";
              qDebug() << "ParentFileRef " << PUsnRecord->ParentFileReferenceNumber << "\n";
              qDebug() << query->StartUsn;
              qDebug() << *(USN*)buffer - query->StartUsn;
              emit sendPUSN(id, PUsnRecord);
            }
            DWORD recordLen = PUsnRecord->RecordLength;
            dwRetBytes -= recordLen;
            PUsnRecord = (PUSN_RECORD)(((PCHAR)PUsnRecord) + recordLen);
          }
          journal_query->StartUsn = *(USN*)buffer;
        }
        last_usn = *(USN*)buffer;
        query->StartUsn = last_usn;
      }
    }
    qDebug() << "Monitor end\n";
}

std::unique_ptr<READ_USN_JOURNAL_DATA> Monitor::getNext(USN start_usn, int byte_wait) {
  auto query = std::make_unique<READ_USN_JOURNAL_DATA>();
  query->StartUsn = start_usn;
  query->ReasonMask = 0xFFFFFFFF;
  query->ReturnOnlyOnClose = FALSE;
  query->Timeout = 0;
  query->BytesToWaitFor = byte_wait;
  query->UsnJournalID = journal.UsnJournalID;
  query->MinMajorVersion = 2;
  query->MaxMajorVersion = 2;
  return query;
}

bool Monitor::waitNext(PREAD_USN_JOURNAL_DATA data) const {
  DWORD br;
  bool ok = true;
  ok = DeviceIoControl(root_handle, FSCTL_READ_USN_JOURNAL,
    data, sizeof(*data),
    &data->StartUsn, sizeof(data->StartUsn),
    &br, nullptr);
  return ok;
}
