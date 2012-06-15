/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Andreas Jakl (andreas.jakl@nokia.com)
**
** Released under Nokia Example Code License.
** See license.txt in the main project folder.
**
****************************************************************************/
#ifndef TAGIMAGECACHE_H
#define TAGIMAGECACHE_H

#include <QDeclarativeImageProvider>
#include <QImage>
#include <QDebug>

/*!
  \brief Caches any images found on tags in memory, for retrieval
  and display in the QML user interface.

  The class is derived from QDeclarativeImageProvider and uses the
  QImage operation mode.
  */
class TagImageCache : public QDeclarativeImageProvider
{
public:
    TagImageCache();
    ~TagImageCache();

    int addImage(QImage img);

    // From QDeclarativeImageProvider interface
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    QList<QImage> m_imageCache;
};

#endif // TAGIMAGECACHE_H
