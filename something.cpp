#include "something.h"
#include "ui_something.h"
#include <QVBoxLayout>
#include <QMetaType>
#include <QHBoxLayout>
#include <QFileDialog>

Something::Something(QWidget *parent) : QMainWindow(parent), ui(new Ui::Something) {
  ui->setupUi(this);
  this->setWindowTitle("Something");
  this->createUI();
  //this->initEngine();
}

void Something::createUI() {
  input = new QLineEdit;
  searchBtn = new QPushButton;
  table = new QTableView;
  model = new QStandardItemModel;
  searchBtn->setText("Search");
  QVBoxLayout *vBoxLayout = new QVBoxLayout;
  QHBoxLayout *hBoxlayout = new QHBoxLayout;
  QWidget *widget = new QWidget;
  QWidget *subwidget = new QWidget;
  hBoxlayout->addWidget(input);
  hBoxlayout->addWidget(searchBtn);
  subwidget->setLayout(hBoxlayout);
  vBoxLayout->addWidget(subwidget);
  vBoxLayout->addWidget(table);
  widget->setLayout(vBoxLayout);
  setCentralWidget(widget);
  table->setShowGrid(true);
  table->setSelectionMode(QAbstractItemView::SingleSelection);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->horizontalHeader()->setStretchLastSection(true);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Name")));
  model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Path")));
  table->setModel(model);
  delegate = new HTMLDelegate;
  table->setItemDelegate(delegate);
  connect(searchBtn, SIGNAL(released()), this, SLOT(search()));
  qRegisterMetaType<PUSN_RECORD>("Myclass");
  menu = menuBar()->addMenu("Add");
  buildIndex = new QAction(this);
  buildIndex->setText("Add polygon");
  menu->addAction(buildIndex);
  connect(buildIndex, SIGNAL(triggered()), this, SLOT(buildIndexSlot()));

}

void Something::initEngine() {
  searchBtn->setEnabled(false);
  this->statusBar()->showMessage("Building Index......");
  int id = 0;
  for (auto ch : _drivers) {
    drivers.push_back(new USNParser(ch));
    monitors.push_back(new Monitor(id++, drivers.back()->root_handle, drivers.back()->journal));
    monitors.back()->start();
    connect(monitors.back(), SIGNAL(sendPUSN(int, PUSN_RECORD)), this, SLOT(recvPUSN(int, PUSN_RECORD)));
    fileindexs.push_back(new FileIndex(drivers.back()));
  }
  searcher = new Searcher(drivers, fileindexs, model);
  this->statusBar()->showMessage("Finish");
  searchBtn->setEnabled(true);
}

void Something::search() {
  auto query = input->text().toStdWString();
  searcher->parseQuery(query);
  //std::vector<FileEntry*> res;
  //for (auto& driver : drivers) {
  //  auto tmp = driver->query(pattern.toStdWString());
  //  std::copy(tmp.begin(), tmp.end(), std::back_inserter(res));
  //}
  //int i = 0;
  //for(auto ptr : res) {
  //  table->insertRow(i);
  //  table->setItem(i, 0, new QTableWidgetItem(QString::fromStdWString(ptr->file_name)));
  //  table->setItem(i, 1, new QTableWidgetItem(QString::fromStdWString(ptr->full_path)));
  //  i++;
  //}
  //table->resizeColumnsToContents();
}

void Something::recvPUSN(int id, PUSN_RECORD pusn) {
  drivers[id]->recvPUSN(pusn);
}

Something::~Something() {
  delete ui;
}

void Something::closeEvent(QCloseEvent* e) {
  for (auto* ptr : monitors) ptr->wait();
  e->accept();
}

void Something::buildIndexSlot() {
  QString path = QFileDialog::getOpenFileName(this, tr("Choose files"), ".", tr("All (*.*)"));
  int id = 0;
  for (id = 0; id < _drivers.size(); ++id) {
    if (path[0] == _drivers[id]) break;
  }
  fileindexs[id]->InsertFiles(path.toStdWString());
}