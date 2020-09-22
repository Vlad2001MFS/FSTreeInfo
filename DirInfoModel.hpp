#pragma once
#include <QAbstractTableModel>
#include <QAtomicInteger>
#include <array>

class DirInfoModel : public QAbstractTableModel {
    Q_OBJECT

public:
    DirInfoModel();

    bool scanDirectory(const QString &dirPath, size_t &subDirsCount, QAtomicInt::QAtomicInteger &isTerminateScanningNeeded);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    static QString mGetHumanReadableSize(size_t size);

    struct FileGroupInfo {
        QString name;
        size_t filesCount = 0;
        size_t totalSize = 0;
        size_t avgSize = 0;
    };

    QVector<FileGroupInfo> mGroupsData;
    FileGroupInfo mTotalInfo;

    const std::array<QVariant, 4> TABLE_SECTIONS;
};
