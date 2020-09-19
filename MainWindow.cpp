#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    mFSModel.reset(new QFileSystemModel());
    mFSModel->setRootPath(QDir::rootPath());
    mFSModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    ui->fsTreeView->setModel(mFSModel.get());
    ui->fsTreeView->setCurrentIndex(mFSModel->index(QDir::currentPath()));
    ui->fsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    mDirInfoModel.reset(new DirInfoModel());
    ui->dirInfoView->setModel(mDirInfoModel.get());
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_fsTreeView_clicked(const QModelIndex &index) {
    if (mFSModel->isDir(index)) {
        ui->statusbar->showMessage("Scanning directory...");
        ui->statusbar->repaint();

        auto fileInfo = mFSModel->fileInfo(index);

        ui->dirInfoBox->setTitle("Current Directory: " + fileInfo.absoluteFilePath());
        mDirInfoModel->setTargetDir(fileInfo.absoluteFilePath());
        ui->dirInfoView->setRootIndex(mDirInfoModel->index(0, 0));

        ui->subdirsCountLabel->setNum(QDir(fileInfo.absoluteFilePath()).entryList(QDir::AllDirs | QDir::NoDotAndDotDot).size());

        ui->statusbar->showMessage("Ready");
    }
}
