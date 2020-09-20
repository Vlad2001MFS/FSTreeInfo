#include "DirInfoModel.hpp"
#include <QDirIterator>
#include <array>

void gatherFilesInfo(const QString &dirPath, QMap<QString, FileGroupInfo> &data) {
    QFileInfoList filesList = QDir(dirPath).entryInfoList(QDir::Filter::Files | QDir::Filter::Hidden | QDir::Filter::System);
    for(const auto &file : filesList) {
        QString fileExtension = file.suffix().toLower();
        FileGroupInfo &info = data[fileExtension];
        info.name = fileExtension;
        info.filesCount++;
        info.totalSize += file.size();
    }
}

static const std::array<QVariant, 4> TABLE_SECTIONS = {
    "Group",
    "Files count",
    "Total size",
    "Average size",
};

bool DirInfoModel::scanDirectory(const QString &dirPath, QAtomicInt::QAtomicInteger &isTerminateScanningNeeded) {
    QMap<QString, FileGroupInfo> groupsData;
    QDirIterator dirIt(dirPath,
                       QDir::Filter::AllDirs | QDir::Filter::Hidden | QDir::Filter::System | QDir::Filter::NoDotAndDotDot,
                       QDirIterator::Subdirectories);

    while (dirIt.hasNext()) {
        QString nextDirPath = dirIt.next();
        gatherFilesInfo(nextDirPath, groupsData);

        if (isTerminateScanningNeeded) {
            isTerminateScanningNeeded = false;
            return false;
        }
    }
    gatherFilesInfo(dirPath, groupsData);

    FileGroupInfo totalInfo;
    totalInfo.name = "All types";
    for (auto &group : groupsData) {
        if (group.filesCount > 0) {
            group.avgSize = group.totalSize / group.filesCount;
        }

        totalInfo.filesCount += group.filesCount;
        totalInfo.totalSize += group.totalSize;
    }
    if (totalInfo.filesCount > 0) {
        totalInfo.avgSize = totalInfo.totalSize / totalInfo.filesCount;
    }

    this->beginResetModel();

    mGroupsData = groupsData.values();
    mTotalInfo = totalInfo;

    this->endResetModel();

    return true;
}

int DirInfoModel::rowCount(const QModelIndex &parent) const {
    if (mGroupsData.size() > 0) {
        return mGroupsData.size() + 1;
    }
    else {
        return 0;
    }
}

int DirInfoModel::columnCount(const QModelIndex &parent) const {
    return static_cast<int>(TABLE_SECTIONS.size());
}

QVariant DirInfoModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        if (index.row() == 0) {
            switch (index.column()) {
                case 0: return QVariant(mTotalInfo.name); break;
                case 1: return QVariant(mTotalInfo.filesCount); break;
                case 2: return QVariant(mTotalInfo.totalSize); break;
                case 3: return QVariant(mTotalInfo.avgSize); break;
            }
        }
        else {
            auto group = mGroupsData[index.row() - 1];
            switch (index.column()) {
                case 0: return QVariant(group.name); break;
                case 1: return QVariant(group.filesCount); break;
                case 2: return QVariant(group.totalSize); break;
                case 3: return QVariant(group.avgSize); break;
            }
        }
    }

    return QVariant();
}

QVariant DirInfoModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Vertical) {
            return QVariant(section + 1);
        }
        else {
            return TABLE_SECTIONS[section];
        }
    }
    else {
        return QVariant();
    }
}
