#include "USNParser.h"
#include <QDebug>
USNParser::USNParser(const char dl)
{
    driver_letter = dl;
    char vol[10] = ".:\\";
    vol[0] = dl;
    char sysNameBuf[128];
    int status = GetVolumeInformationA(vol, NULL, 0, NULL, NULL, NULL, sysNameBuf, 128);
    if (status && strcmp(sysNameBuf, "NTFS") == 0) {
        getRootHandle();
    } else {
        qDebug() << "Only NTFS supported yet";
    }
    DWORD br;
    bool isSuccess = DeviceIoControl(root_handle, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &journal, sizeof(journal), &br, NULL);
    if (!isSuccess) isSuccess = createUsnJournal();
    genEntries();
}

std::vector<FileEntry*> USNParser::query(const std::wstring& pattern) {
    std::vector<FileEntry*> res;
    for (auto& kvp : all_entries) {
        if (kvp.second->file_name.find(pattern) != std::wstring::npos) {
            res.push_back(kvp.second);
            if (kvp.second->is_folder && sub_entries.count(kvp.second->file_ref)) {
                recursiveAdd(kvp.second->file_ref, res);
            }
        }
    }
    for (auto ptr : res)
        ptr->genPath(all_entries);
    return res;
}

void USNParser::recursiveAdd(FILEREF folder, std::vector<FileEntry*>& res) {
    auto& childs = sub_entries[folder];
    for (auto child : childs) {
        res.push_back(child);
        if (child->is_folder && sub_entries.count(child->file_ref)) {
            recursiveAdd(child->file_ref, res);
        }
    }
}


bool USNParser::createUsnJournal()
{
    CREATE_USN_JOURNAL_DATA cujd;
    DWORD br;
    cujd.MaximumSize = 0;
    cujd.AllocationDelta = 0;
    auto status = DeviceIoControl(root_handle, FSCTL_CREATE_USN_JOURNAL, &cujd, sizeof(cujd), NULL, 0, &br, NULL);
    return status;
}

void USNParser::getRootHandle() {
    char vol[10] = "\\\\.\\C:";
    vol[4] = driver_letter;
    root_handle = CreateFileA(vol,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_READONLY,
        NULL);

    if (root_handle == INVALID_HANDLE_VALUE)
    {
        qDebug() << "Fail to parse USN, please use administrator mode";
    }
}

void USNParser::genEntries() {
    constexpr int BUFLEN = 1 << 18;
      all_entries[ROOT_REFERENCE] = new FileEntry(driver_letter);
      MFT_ENUM_DATA_V0 mftEnumData;
      mftEnumData.StartFileReferenceNumber = 0;
      mftEnumData.LowUsn = 0;
      mftEnumData.HighUsn = journal.NextUsn;
      CHAR buffer[BUFLEN];
      DWORD usnDataSize;
      PUSN_RECORD UsnRecord;
      while (DeviceIoControl(root_handle, FSCTL_ENUM_USN_DATA, &mftEnumData, sizeof(mftEnumData), buffer, BUFLEN, &usnDataSize, NULL))
      {
          DWORD dwRetBytes = usnDataSize - sizeof(USN);
          UsnRecord = (PUSN_RECORD)(((PCHAR)buffer) + sizeof(USN));
          while (dwRetBytes>0) {
              if (UsnRecord->FileName[0] != L'$') {
                auto ptr = new FileEntry(UsnRecord);
                all_entries.insert({ UsnRecord->FileReferenceNumber, ptr });
                all_entries[UsnRecord->FileReferenceNumber] = ptr;
                if (sub_entries.count(UsnRecord->ParentFileReferenceNumber) == 0)
                  sub_entries.insert({ UsnRecord->ParentFileReferenceNumber , std::vector<FileEntry*>() });
                sub_entries[UsnRecord->ParentFileReferenceNumber].push_back(ptr);
              }
              DWORD recordLen = UsnRecord->RecordLength;
              dwRetBytes -= recordLen;
              UsnRecord = (PUSN_RECORD)(((PCHAR)UsnRecord) + recordLen);
          }
          mftEnumData.StartFileReferenceNumber = *(USN *)&buffer;
      }
}
void USNParser::recvPUSN(PUSN_RECORD pusn) {
  if (pusn->Reason & USN_REASON_FILE_CREATE) {
    auto ptr = new FileEntry(pusn);
    all_entries.insert_or_assign(pusn->FileReferenceNumber, ptr);
    all_entries[pusn->FileReferenceNumber] = ptr;
    if (sub_entries.count(pusn->ParentFileReferenceNumber) == 0)
      sub_entries.insert({ pusn->ParentFileReferenceNumber , std::vector<FileEntry*>() });
    sub_entries[pusn->ParentFileReferenceNumber].push_back(ptr);
  }
  if (pusn->Reason & USN_REASON_RENAME_NEW_NAME) {
    all_entries[pusn->FileReferenceNumber]->file_name = std::wstring(pusn->FileName).substr(0, pusn->FileNameLength / 2);
  }
}

USNParser::~USNParser()
{
    if (root_handle != NULL && root_handle != INVALID_HANDLE_VALUE)
        CloseHandle(root_handle);
}
