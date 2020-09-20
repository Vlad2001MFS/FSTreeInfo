#pragma once
#include <QMainWindow>
#include <QFileSystemModel>
#include <QScopedPointer>
#include <QThreadPool>
#include <QAtomicInt>
#include "DirInfoModel.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_fsTreeView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QScopedPointer<QFileSystemModel> mFSModel;
    QScopedPointer<DirInfoModel> mDirInfoModel;
    QModelIndex mCurrentFSModelIndex;
    QAtomicInt::QAtomicInteger mIsTerminateScanningNeeded;
    QThreadPool mThreadPool;
};
