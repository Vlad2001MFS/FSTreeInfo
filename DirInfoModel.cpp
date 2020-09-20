#include "DirInfoModel.hpp"
#include <QDirIterator>
#include <QCoreApplication>

void gatherFilesInfo(const QString &dirPath, QMap<QString, FileGroupInfo> &data) {
    QFileInfoList filesList = QDir(dirPath).entryInfoList(QDir::Filter::Files | QDir::Filter::Hidden | QDir::Filter::System);
    for(auto &file : filesList) {
        QString fileExtension = file.suffix().toLower();
        FileGroupInfo &info = data[fileExtension];
        info.name = fileExtension;
        info.filesCount++;
        info.totalSize += file.size();
    }
}

static const QVariant TABLE_SECTIONS[] = {
    "Group",
    "Files count",
    "Total size",
    "Average size",
};
static const int TABLE_SECTIONS_COUNT = sizeof(TABLE_SECTIONS) / sizeof(TABLE_SECTIONS[0]);

DirInfoModel::DirInfoModel() {
}

bool DirInfoModel::scanDirectory(const QString &dirPath, QAtomicInt::QAtomicInteger &isTerminateScanningNeeded) {
    QMap<QString, FileGroupInfo> groupsData;

    QDirIterator dirIt(dirPath, QDir::Filter::AllDirs | QDir::Filter::Hidden | QDir::Filter::System | QDir::Filter::NoDotAndDotDot, QDirIterator::Subdirectories);
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
    for (auto &group : groupsData) {
        group.avgSize = group.totalSize / group.filesCount;

        totalInfo.name = "All types";
        totalInfo.filesCount += group.filesCount;
        totalInfo.totalSize += group.totalSize;
    }
    if (totalInfo.filesCount > 0) {
        totalInfo.avgSize = totalInfo.totalSize / totalInfo.filesCount;
    }

    this->beginResetModel();

    mGroupsData = groupsData;
    mTotalInfo = totalInfo;

    this->endResetModel();

    return true;
}

int DirInfoModel::rowCount(const QModelIndex &parent) const {
    return mGroupsData.size() > 0 ? mGroupsData.size() + 1 : 0;
}

int DirInfoModel::columnCount(const QModelIndex &parent) const {
    return TABLE_SECTIONS_COUNT;
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
            int i = 1;
            for (auto &group : mGroupsData) {
                if (i == index.row()) {
                    switch (index.column()) {
                        case 0: return QVariant(group.name); break;
                        case 1: return QVariant(group.filesCount); break;
                        case 2: return QVariant(group.totalSize); break;
                        case 3: return QVariant(group.avgSize); break;
                    }
                    break;
                }
                i++;
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
            assert(section >= 0);
            assert(section < TABLE_SECTIONS_COUNT);
            return TABLE_SECTIONS[section];
        }
    }
    else {
        return QVariant();
    }
}
