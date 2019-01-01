#include "something.h"
#include "ui_something.h"
#include <QVBoxLayout>
#include <QMetaType>
#include <QHBoxLayout>

Something::Something(QWidget *parent) : QMainWindow(parent), ui(new Ui::Something) {
  ui->setupUi(this);
  this->setWindowTitle("Something");
  this->createUI();
  //this->initEngine();
  initIndex();
}

void Something::createUI() {
  input = new QLineEdit;
  searchBtn = new QPushButton;
  table = new QTableWidget;
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
  table->setColumnCount(2);
  table->setShowGrid(true);
  table->setSelectionMode(QAbstractItemView::SingleSelection);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setHorizontalHeaderLabels(QStringList({ QString("Name"), QString("Path") }));
  table->horizontalHeader()->setStretchLastSection(true);
  connect(searchBtn, SIGNAL(released()), this, SLOT(search()));
  qRegisterMetaType<PUSN_RECORD>("Myclass");
}

void Something::initEngine() {
  searchBtn->setEnabled(false);
  this->statusBar()->showMessage("Building Index......");
  QVector<char> _drivers = { 'D' };
  int id = 0;
  for (auto ch : _drivers) {
    drivers.push_back(new USNParser(ch));
    monitors.push_back(new Monitor(id++, drivers.back()->root_handle, drivers.back()->journal));
    monitors.back()->start();
    connect(monitors.back(), SIGNAL(sendPUSN(int, PUSN_RECORD)), this, SLOT(recvPUSN(int, PUSN_RECORD)));
  }
  this->statusBar()->showMessage("Finish");
  searchBtn->setEnabled(true);
}

void Something::search() {
  auto pattern = input->text();
  std::vector<FileEntry*> res;
  for (auto& driver : drivers) {
    auto tmp = driver->query(pattern.toStdWString());
    std::copy(tmp.begin(), tmp.end(), std::back_inserter(res));
  }
  int i = 0;
  for(auto ptr : res) {
    table->insertRow(i);
    table->setItem(i, 0, new QTableWidgetItem(QString::fromStdWString(ptr->file_name)));
    table->setItem(i, 1, new QTableWidgetItem(QString::fromStdWString(ptr->full_path)));
    i++;
  }
  table->resizeColumnsToContents();
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

void Something::initIndex() {
	process = new QProcess(this);
	fileindex = new FileIndex(process);
}
