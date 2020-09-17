#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    mFSModel.reset(new QFileSystemModel());
    mFSModel->setRootPath(QDir::rootPath());

    ui->fsTreeView->setModel(mFSModel.get());
    ui->fsTreeView->setCurrentIndex(mFSModel->index(QDir::currentPath()));
    ui->fsTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

MainWindow::~MainWindow() {
    delete ui;
}
