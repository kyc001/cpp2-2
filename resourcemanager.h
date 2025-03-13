//2025年3月12日
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <QPixmap>
#include <QHash>

class ResourceManager : public QObject {
    Q_OBJECT
public:
    explicit ResourceManager(QObject *parent = nullptr);
    ~ResourceManager();

    // 获取图片资源
    QPixmap getPixmap(const QString& path);

private:
    // 存储图片资源的映射
    QHash<QString, QPixmap> pixmapCache;
};

#endif // RESOURCEMANAGER_H
