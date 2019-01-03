#include "something.h"
#include "ui_something.h"
#include <QVBoxLayout>
#include <QMetaType>
#include <QHBoxLayout>
#include <QFileDialog>
#include "BuildIndexThread.h"

Something::Something(QWidget *parent) : QMainWindow(parent), ui(new Ui::Something) {
  ui->setupUi(this);
  this->setWindowTitle("Something");
  this->createUI();
  //this->initEngine();
}

void Something::createUI() {
  setFixedHeight(768);
  setFixedWidth(1024);
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
  menu = menuBar()->addMenu("Build");
  buildIndex = new QAction(this);
  buildIndex->setText("Build Index");
  menu->addAction(buildIndex);
  connect(buildIndex, SIGNAL(triggered()), this, SLOT(buildIndexSlot()));

  pProgressBar = new QProgressBar();
  pLabel = new QLabel();
  pProgressBar->setRange(0, 100);
  pProgressBar->setValue(0);
  pProgressBar->setFixedWidth(0.4 * width());
  this->statusBar()->addWidget(pLabel);
  this->statusBar()->addPermanentWidget(pProgressBar);
  list = new QListWidget(this);
  list->move(18, 68);
  list->resize(907, 17);
  list->hide();
  connect(input, SIGNAL(textChanged(const QString &)), this, SLOT(search()));
  connect(input, SIGNAL(textChanged(const QString &)), this, SLOT(showRecommend(const QString &)));
  connect(list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(click_rec(QListWidgetItem*)));
}

void Something::initEngine() {
  pLabel->setText("Building Index......");
  searchBtn->setEnabled(false);
  int id = 0;
  for (auto ch : _drivers) {
    drivers.push_back(new USNParser(ch));
    monitors.push_back(new Monitor(id++, drivers.back()->root_handle, drivers.back()->journal));
    monitors.back()->start();
    connect(monitors.back(), SIGNAL(sendPUSN(int, PUSN_RECORD)), this, SLOT(recvPUSN(int, PUSN_RECORD)), Qt::DirectConnection);
    fileindexs.push_back(new FileIndex(drivers.back()));
  }
  searcher = new Searcher(drivers, fileindexs);
  pLabel->setText("Finish");
  pProgressBar->setValue(100);
  searchBtn->setEnabled(true);
}

void Something::search() {
  auto query = input->text().toStdWString();
  history.addHistory(query);
  searcher->parseQuery(query);
  pLabel->setText(QString("%1 records").arg(searcher->path_result.size() + searcher->content_result.size()));
  updateResult();
}

void Something::updateResult() {
  model->removeRows(0, model->rowCount());
  table->setUpdatesEnabled(false);
  int display_max = 100;
  if (!searcher->content_result.empty() && !searcher->path_result.empty()) {
    std::set<FileEntry*> tmp;
    std::set_intersection(searcher->content_result.begin(), searcher->content_result.end(),
      searcher->path_result.begin(), searcher->path_result.end(), std::inserter(tmp, tmp.end()));
    int i = 0;
    for (auto ptr : tmp) {
      model->setItem(i, 0, new QStandardItem(QString::fromStdWString(searcher->addHighLight(ptr->file_name))));
      model->setItem(i, 1, new QStandardItem(QString::fromStdWString(searcher->addHighLight(ptr->full_path))));
      i++;
      if (i == display_max)break;
    }
  } else {
    if (!searcher->content_result.empty()) {
      int i = 0;
      for (auto ptr : searcher->content_result) {
        model->setItem(i, 0, new QStandardItem(QString::fromStdWString(searcher->addHighLight(ptr->file_name))));
        model->setItem(i, 1, new QStandardItem(QString::fromStdWString(searcher->addHighLight(ptr->full_path))));
        i++;
        if (i == display_max)break;
      }
    }
    else if (!searcher->path_result.empty()) {
      int i = 0;
      for (auto ptr : searcher->path_result) {
        model->setItem(i, 0, new QStandardItem(QString::fromStdWString(searcher->addHighLight(ptr->file_name))));
        model->setItem(i, 1, new QStandardItem(QString::fromStdWString(searcher->addHighLight(ptr->full_path))));
        i++;
        if (i == display_max)break;
      }
    }
  }
  table->setUpdatesEnabled(true);
}

void Something::recvPUSN(int id, PUSN_RECORD pusn) {
  auto needUpdate = searcher->recvPUSN(id, pusn);
  if (needUpdate) updateResult();
}

Something::~Something() {
  delete ui;
}

void Something::closeEvent(QCloseEvent* e) {
  for (auto* ptr : monitors) ptr->terminate();
  e->accept();
}

void Something::buildIndexSlot() {
	QString path = QFileDialog::getExistingDirectory(this, tr("Choose folders"), ".");
  if (path.length() == 0) return;
	int id = 0;
	for (id = 0; id < _drivers.size(); ++id) {
		if (path[0] == _drivers[id]) break;
	}
	std::wstring temp = path.toStdWString();
	while (true) {
		auto pos = temp.find(47);
		if (pos == std::wstring::npos)
			break;
		temp = temp.replace(pos, 1, L"\\");
	}
  const auto ref_num = drivers[id]->getFileRef(temp);
  if (ref_num == 0) return;
  std::set<FileEntry*> files;
  drivers[id]->recursiveAdd(ref_num, files);
  auto dataProcessor = new BuildIndexThread(std::move(files), drivers[id], fileindexs[id]);
  connect(dataProcessor, SIGNAL(setValue(int)), pProgressBar, SLOT(setValue(int)));
  connect(dataProcessor, SIGNAL(setLabel(QString)), pLabel, SLOT(setText(QString)));
  connect(dataProcessor, SIGNAL(enableBtn(bool)), searchBtn, SLOT(setEnabled(bool)));
  connect(dataProcessor, SIGNAL(enableBtn(bool)), buildIndex, SLOT(setEnabled(bool)));
  dataProcessor->start();
}

void::Something::showRecommend(const QString& path) {
	list->clear();
	std::vector <std::wstring> result = history.recommend(path.toStdWString());
  std::vector<std::wstring>&& result2 = searcher->recommend();
  result.insert(result.end(), std::make_move_iterator(result2.begin()), std::make_move_iterator(result2.end()));
	if (result.size() == 0 || path == "") {
		list->hide();
		return;
	}
	for (int i = 0; i < result.size(); i++) {
		QListWidgetItem *rcList = new QListWidgetItem(list, QListWidgetItem::Type);
		QString temp = QString::fromStdWString(result[i]);
		rcList->setText(temp);
		list->insertItem(i, rcList);
	}
	list->resize(907, 17 * result.size());
	list->setCurrentRow(result.size());
	list->show();
}

void Something::click_rec(QListWidgetItem* item) {
	input->setText(item->text());
	list->clear();
	list->hide();
}

void Something::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Escape) {
		list->clear();
		list->hide();
	}
}
