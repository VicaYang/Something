#include "something.h"
#include "ui_something.h"
#include <QVBoxLayout>
#include <QMetaType>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTime>
#include "BuildIndexThread.h"


Something::Something(QWidget *parent) : QMainWindow(parent), ui(new Ui::Something) {
  ui->setupUi(this);
  this->setWindowTitle("Something");
  this->createUI();
}

void Something::createUI() {
  setFixedHeight(600);
  setFixedWidth(800);
  input = new QLineEdit;
  table = new QTableView;
  model = new QStandardItemModel;
  QVBoxLayout *vBoxLayout = new QVBoxLayout;
  QHBoxLayout *hBoxlayout = new QHBoxLayout;
  QWidget *widget = new QWidget;
  QWidget *subwidget = new QWidget;
  hBoxlayout->addWidget(input);
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
  qRegisterMetaType<PUSN_RECORD>("Myclass");
  qRegisterMetaType<QVector<int>>("Myclass2");
  qRegisterMetaType<QList<QPersistentModelIndex>>("Myclass3");
  qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("Myclass4");
  menu = menuBar()->addMenu("Build");
  buildIndex = new QAction(this);
  buildIndex->setText("Build Index");
  menu->addAction(buildIndex);
  connect(buildIndex, SIGNAL(triggered()), this, SLOT(buildIndexSlot()));

  pProgressBar = new QProgressBar();
  pLabel = new QLabel();
  pLabel->setText("Building Index...... Please wait for seconds");
  pProgressBar->setRange(0, 100);
  pProgressBar->setValue(0);
  pProgressBar->setFixedWidth(0.4 * width());
  this->statusBar()->addWidget(pLabel);
  this->statusBar()->addPermanentWidget(pProgressBar);
  list = new QListWidget(this);
  list->move(18, 68);
  list->resize(input->width(), 17);
  list->hide();
}

void Something::initEngine() {
  QTime timer;
  timer.start();
  searcher = new Searcher();
  size_t cnt = 0;
  for (auto driver : searcher->drivers) cnt += driver->all_entries.size();
  pLabel->setText(QString("Finish. %1 records %2 ms").arg(cnt).arg(timer.elapsed()));
  pProgressBar->setValue(100);
  for (int i = 0; i < searcher->_drivers.size(); ++i) {
    connect(searcher->monitors[i], SIGNAL(sendPUSN(int, PUSN_RECORD)), this, SLOT(recvPUSN(int, PUSN_RECORD)), Qt::DirectConnection);
    searcher->monitors[i]->start();
  }
  connect(input, SIGNAL(textChanged(const QString &)), this, SLOT(search()), Qt::DirectConnection);
  connect(input, SIGNAL(textChanged(const QString &)), this, SLOT(showRecommend(const QString &)));
  connect(list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(click_rec(QListWidgetItem*)));
}

void Something::search() {
  searcher->lock.lock();
  auto query = input->text().toStdWString();
  history.addHistory(query);
  searcher->parseQuery(query);
  pLabel->setText(QString("%1 records").arg(searcher->path_result.size() + searcher->content_result.size()));
  updateResult();
  searcher->lock.unlock();
}

void Something::updateResult() {
  model->removeRows(0, model->rowCount());
  table->setUpdatesEnabled(false);
  int display_max = 100;
  std::map<char, int> drivers_to_id;
  int id = 0;
  for (auto ch : searcher->_drivers) drivers_to_id.insert({ ch, id++ });
  if (!searcher->content_result.empty() && !searcher->path_result.empty()) {
    std::set<FileEntry*> tmp;
    std::set_intersection(searcher->content_result.begin(), searcher->content_result.end(),
      searcher->path_result.begin(), searcher->path_result.end(), std::inserter(tmp, tmp.end()));
    int i = 0;
    for (auto ptr : tmp) {
      ptr->genPath(searcher->drivers[drivers_to_id[ptr->driver_letter]]->all_entries);
      model->setItem(i, 0, new QStandardItem(QString::fromStdWString(searcher->addHighLight(ptr->file_name))));
      model->setItem(i, 1, new QStandardItem(QString::fromStdWString(searcher->addHighLight(ptr->full_path))));
      i++;
      if (i == display_max)break;
    }
  } else {
    if (!searcher->content_result.empty()) {
      int i = 0;
      for (auto ptr : searcher->content_result) {
        ptr->genPath(searcher->drivers[drivers_to_id[ptr->driver_letter]]->all_entries);
        model->setItem(i, 0, new QStandardItem(QString::fromStdWString(searcher->addHighLight(ptr->file_name))));
        model->setItem(i, 1, new QStandardItem(QString::fromStdWString(searcher->addHighLight(ptr->full_path))));
        i++;
        if (i == display_max)break;
      }
    }
    else if (!searcher->path_result.empty()) {
      int i = 0;
      for (auto ptr : searcher->path_result) {
        ptr->genPath(searcher->drivers[drivers_to_id[ptr->driver_letter]]->all_entries);
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
  searcher->lock.lock();
  auto needUpdate = searcher->recvPUSN(id, pusn);
  if (needUpdate) {
    updateResult();
    searcher->content_result.clear();
    searcher->searchContent(searcher->_content);
  }
  searcher->lock.unlock();
}

Something::~Something() {
  delete ui;
}

void Something::closeEvent(QCloseEvent* e) {
  for (auto ptr : searcher->monitors) ptr->terminate();
  e->accept();
}

void Something::buildIndexSlot() {
	QString path = QFileDialog::getExistingDirectory(this, tr("Choose folders"), ".");
  if (path.length() == 0) return;
	int id = 0;
	for (id = 0; id < searcher->_drivers.size(); ++id) {
		if (path[0] == searcher->_drivers[id]) break;
	}
  if (id == searcher->_drivers.size()) {
    QMessageBox messageBox;
    messageBox.critical(0,"Error","The file has not been indexed before!");
    return;
  }
	std::wstring temp = path.toStdWString();
	while (true) {
		auto pos = temp.find(47);
		if (pos == std::wstring::npos)
			break;
		temp = temp.replace(pos, 1, L"\\");
	}
  const auto ref_num = searcher->drivers[id]->getFileRef(temp);
  if (ref_num == 0) return;
  std::set<FileEntry*> files;
  searcher->drivers[id]->recursiveAdd(ref_num, files);
  auto dataProcessor = new BuildIndexThread(std::move(files), searcher->drivers[id], searcher->indexs[id]);
  connect(dataProcessor, SIGNAL(setValue(int)), pProgressBar, SLOT(setValue(int)));
  connect(dataProcessor, SIGNAL(setLabel(QString)), pLabel, SLOT(setText(QString)));
  connect(dataProcessor, SIGNAL(resume()), this, SLOT(unlock()));
  connect(dataProcessor, SIGNAL(enableBtn(bool)), buildIndex, SLOT(setEnabled(bool)));
  connect(dataProcessor, SIGNAL(enableBtn(bool)), input, SLOT(setEnabled(bool)));
  searcher->lock.lock();
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
	list->resize(input->width(), static_cast<int>(17 * result.size()));
	list->setCurrentRow(static_cast<int>(result.size()));
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

void Something::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    list->clear();
    list->hide();
  }
}
