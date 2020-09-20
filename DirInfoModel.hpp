#pragma once
#include <QAbstractTableModel>
#include <QAtomicInteger>
#include <array>

class DirInfoModel : public QAbstractTableModel {
public:
    bool scanDirectory(const QString &dirPath, size_t &subDirsCount, QAtomicInt::QAtomicInteger &isTerminateScanningNeeded);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    struct FileGroupInfo {
        QString name;
        size_t filesCount = 0;
        size_t totalSize = 0;
        size_t avgSize = 0;
    };

    QVector<FileGroupInfo> mGroupsData;
    FileGroupInfo mTotalInfo;

    static const std::array<QVariant, 4> TABLE_SECTIONS;
};
