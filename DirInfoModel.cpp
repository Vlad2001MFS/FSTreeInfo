#include "DirInfoModel.hpp"
#include <QDirIterator>

const std::array<QVariant, 4> DirInfoModel::TABLE_SECTIONS = {
    "Group",
    "Files count",
    "Total size",
    "Average size",
};

bool DirInfoModel::scanDirectory(const QString &dirPath, size_t &subDirsCount, QAtomicInt::QAtomicInteger &isTerminateScanningNeeded) {
    QFlags<QDir::Filter> additionalFilters = QDir::NoDotAndDotDot | QDir::Hidden | QDir::System | QDir::Readable | QDir::Writable | QDir::Executable;

    subDirsCount = QDir(dirPath).entryList(QDir::AllDirs | additionalFilters).size();

    QMap<QString, FileGroupInfo> groupsData;
    QDirIterator dirIt(dirPath, QDir::AllDirs | QDir::Files | additionalFilters, QDirIterator::Subdirectories);
    while (true) {
        if (isTerminateScanningNeeded) {
            isTerminateScanningNeeded = false;
            return false;
        }

        QFileInfo fileInfo = dirIt.fileInfo();
        if (fileInfo.isFile() || fileInfo.isShortcut()) {
            QString fileExtension = fileInfo.suffix().toLower();
            FileGroupInfo &info = groupsData[fileExtension];
            info.name = fileExtension;
            info.filesCount++;
            info.totalSize += fileInfo.size();
        }

        if (dirIt.hasNext()) {
            dirIt.next();
        }
        else {
            break;
        }
    }

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

    mGroupsData = groupsData.values().toVector();
    mTotalInfo = totalInfo;

    this->endResetModel();

    return true;
}

int DirInfoModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    if (mGroupsData.size() > 0) {
        return mGroupsData.size() + 1;
    }
    else {
        return 0;
    }
}

int DirInfoModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

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
            const FileGroupInfo &group = mGroupsData.at(index.row() - 1);
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
