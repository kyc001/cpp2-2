// resourcemanager.cpp
#include "resourcemanager.h"

ResourceManager::ResourceManager(QObject *parent) : QObject(parent) {}

ResourceManager::~ResourceManager() {}

QPixmap ResourceManager::getPixmap(const QString& path) {
    if (!pixmapCache.contains(path)) {
        QPixmap pixmap(path);
        if (!pixmap.isNull()) {
            pixmapCache.insert(path, pixmap);
        }
    }
    return pixmapCache.value(path);
}
