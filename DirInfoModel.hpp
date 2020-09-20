#pragma once
#include <QAbstractTableModel>
#include <QDir>
#include <QAtomicInt>

struct FileGroupInfo {
    QString name;
    size_t filesCount = 0;
    size_t totalSize = 0;
    size_t avgSize = 0;
};

class DirInfoModel : public QAbstractTableModel {
public:
    bool scanDirectory(const QString &dirPath, QAtomicInt::QAtomicInteger &isTerminateScanningNeeded);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QList<FileGroupInfo> mGroupsData;
    FileGroupInfo mTotalInfo;
};

