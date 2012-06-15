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

#include "tagimagecache.h"

TagImageCache::TagImageCache() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

TagImageCache::~TagImageCache()
{

}

/*!
  \brief Add an image to the cache. Returns the id, which can then be requested from QML.

  \param img the image to add to the cache.
  */
int TagImageCache::addImage(QImage img)
{
    // Add the image to our list to cache it
    m_imageCache.append(img);
    return m_imageCache.size() - 1;
}

/*!
  \brief Request an image from the cache by specifying its \a id.

  Request the image if possible in \a requestedSize. The image will be scaled
  keeping its aspect ratio. The final size is stored in \a size.
  \return the requested image from the cache, or an empty QImage() otherwise.
  */
QImage TagImageCache::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << "TagImageCache: Requesting image Id: " << id << ", requested size: " << requestedSize;

    // Convert id to a number
    bool ok;
    int imgId = id.toInt(&ok);
    if (!ok)
        // Conversion to a number failed
        return QImage();

    // See if we have an image stored with the requested ID
    if (imgId < 0 || imgId >= m_imageCache.size())
        // Out of range
        return QImage();

    QImage finalImg;

    if (requestedSize.isValid())
    {
        // Need to resize the image
        finalImg = m_imageCache.at(imgId).scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        // Send back the original image
        finalImg = m_imageCache.at(imgId);
    }
    *size = finalImg.size();

    return finalImg;
}
