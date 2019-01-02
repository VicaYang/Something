#include "BuildIndexThread.h"

BuildIndexThread::BuildIndexThread(std::set<FileEntry*>&& files, USNParser* driver, FileIndex* index)
  : files(std::move(files)), driver(driver), index(index){}

void BuildIndexThread::run()
{
  int idx = 0;
  int all = static_cast<int>(files.size());
  emit enableBtn(false);
  for (auto file : files) {
    idx++;
    if (Reader::isValid(file->file_name)) {
      file->genPath(driver->all_entries);
      index->InsertFileIndex(file->file_ref, file->full_path);
    }
    emit setValue(100 * idx / all);
    emit setLabel(QString("%1").arg(QString::fromStdWString(file->file_name)));
  }
  emit setValue(100 * idx / all);
  emit setLabel(QString("Finish"));
  emit enableBtn(true);
}