#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mIsTerminateScanningNeeded(false) {
    ui->setupUi(this);

    mFSModel.reset(new QFileSystemModel(this));
    mFSModel->setRootPath(QDir::rootPath());
    mFSModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System | QDir::Readable | QDir::Writable | QDir::Executable);

    ui->fsTreeView->setModel(mFSModel.get());
    ui->fsTreeView->setCurrentIndex(mFSModel->index(QDir::currentPath()));
    ui->fsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    mDirInfoModel.reset(new DirInfoModel());
    ui->dirInfoView->setModel(mDirInfoModel.get());
    ui->dirInfoView->horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    ui->dirInfoView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->dirInfoView->verticalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    ui->dirInfoView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

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
            ui->statusbar->showMessage(tr("Scanning directory..."));
            QString dirPath = mFSModel->fileInfo(index).absoluteFilePath();
            size_t subDirsCount;
            if (mDirInfoModel->scanDirectory(dirPath, subDirsCount, mIsTerminateScanningNeeded)) {
                ui->dirInfoBox->setTitle(tr("Current Directory: ") + dirPath);
                ui->subdirsCountLabel->setNum(static_cast<int>(subDirsCount));
                ui->statusbar->showMessage(tr("Ready"));
            }
        });
    }
}
