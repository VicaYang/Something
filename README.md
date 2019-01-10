# Something

A toy example for searching file, just like [everything](https://www.voidtools.com/).

We use C++ to finish the engine and Qt for GUI

It can
- Fast build the index for files: It took ~20 seconds for ~2 millions files in my 256G SSD disk
- Build the index for content and search file by content(support .doc/docx/ppt/pptx/pdf/txt/html etc...)
- Monitor the file change. It means when you create/move the file, the change will be record and update immediatelly.
  - Also, if you have built a content index for file A, then the content change will also be detected
- Guess your input

## Demo

[video](https://cloud.tsinghua.edu.cn/f/75092e9ebeac40ccb6d9/)


