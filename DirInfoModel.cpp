#include "DirInfoModel.hpp"
#include <QDirIterator>

void gatherFilesInfo(const QString &dirPath, QMap<QString, FileGroupInfo> &data) {
    QFileInfoList filesList = QDir(dirPath).entryInfoList(QDir::Filter::Files | QDir::Filter::Hidden | QDir::Filter::System);
    for(auto &file : filesList) {
        FileGroupInfo &info = data[file.suffix()];
        info.name = file.suffix();
        info.filesCount++;
        info.totalSize += file.size();
    }
}

void gatherFilesInfoRecursive(const QString &dirPath, QMap<QString, FileGroupInfo> &data) {
    QDirIterator dirIt(dirPath, QDir::Filter::AllDirs | QDir::Filter::Hidden | QDir::Filter::System | QDir::Filter::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (dirIt.hasNext()) {
        QString nextDirPath = dirIt.next();
        gatherFilesInfo(nextDirPath, data);
    }
    gatherFilesInfo(dirPath, data);
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

void DirInfoModel::setTargetDir(const QString &dirPath) {
    mGroupsData.clear();

    gatherFilesInfoRecursive(dirPath, mGroupsData);

    mTotalInfo = FileGroupInfo();
    for (auto &group : mGroupsData) {
        group.avgSize = group.totalSize / group.filesCount;

        mTotalInfo.name = "All types";
        mTotalInfo.filesCount += group.filesCount;
        mTotalInfo.totalSize += group.totalSize;
    }
    mTotalInfo.avgSize = mTotalInfo.totalSize / mTotalInfo.filesCount;
}

int DirInfoModel::rowCount(const QModelIndex &parent) const {
    return mGroupsData.size();
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
