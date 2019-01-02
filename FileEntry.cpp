#include "FileEntry.h"

FileEntry::FileEntry(const PUSN_RECORD usn_record) {
  file_ref = usn_record->FileReferenceNumber;
  parent_ref = usn_record->ParentFileReferenceNumber;
  file_name = std::wstring(usn_record->FileName).substr(0, usn_record->FileNameLength / 2);
  is_folder = (usn_record->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

FileEntry::FileEntry(const char driver_letter) {
  file_ref = ROOT_REFERENCE;
  parent_ref = 0;
  wchar_t tmp[10] = L"C:";
  tmp[0] = driver_letter;
  file_name = std::wstring(tmp);
  full_path = file_name;
  is_folder = true;
}

void FileEntry::genPath(std::map<FILEREF, FileEntry*>& table) {
    if (full_path.length() > 0) return;
    auto parent = table[parent_ref];
    parent->genPath(table);
    full_path = table[parent_ref]->full_path + L"\\" + file_name;
}

bool FileEntry::operator<(const FileEntry& rhs) {
  return file_ref < rhs.file_ref || (file_ref == rhs.file_ref && file_name < rhs.file_name);
}
bool FileEntry::operator==(const FileEntry& rhs) {
  return file_ref == rhs.file_ref && file_name == rhs.file_name;
}

FileEntry::~FileEntry() {
}
