#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mIsTerminateScanningNeeded(false) {
    ui->setupUi(this);

    mFSModel.reset(new QFileSystemModel());
    mFSModel->setRootPath(QDir::rootPath());
    mFSModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

    ui->fsTreeView->setModel(mFSModel.get());
    ui->fsTreeView->setCurrentIndex(mFSModel->index(QDir::currentPath()));
    ui->fsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    mDirInfoModel.reset(new DirInfoModel());
    ui->dirInfoView->setModel(mDirInfoModel.get());
    ui->dirInfoView->horizontalHeader()->setSizeAdjustPolicy(QHeaderView::SizeAdjustPolicy::AdjustToContents);
    ui->dirInfoView->verticalHeader()->setSizeAdjustPolicy(QHeaderView::SizeAdjustPolicy::AdjustToContents);

    mThreadPool.setMaxThreadCount(1);
}

MainWindow::~MainWindow() {
    mIsTerminateScanningNeeded = true;
    mThreadPool.waitForDone();
    delete ui;
}

void MainWindow::on_fsTreeView_clicked(const QModelIndex &index) {
    if (mCurrentFSModelIndex != index && mFSModel->isDir(index)) {
        mCurrentFSModelIndex = index;

        mThreadPool.clear();
        mIsTerminateScanningNeeded = true;
        mThreadPool.waitForDone();
        mIsTerminateScanningNeeded = false;

        mThreadPool.start([&]() {
            ui->statusbar->showMessage("Scanning directory...");
            QFileInfo fileInfo = mFSModel->fileInfo(index);
            if (mDirInfoModel->scanDirectory(fileInfo.absoluteFilePath(), mIsTerminateScanningNeeded)) {
                ui->dirInfoBox->setTitle("Current Directory: " + fileInfo.absoluteFilePath());
                ui->subdirsCountLabel->setNum(QDir(fileInfo.absoluteFilePath()).entryList(QDir::AllDirs | QDir::NoDotAndDotDot).size());
                ui->statusbar->showMessage("Ready");
            }
        });
    }
}
