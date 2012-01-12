/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Andreas Jakl (andreas.jakl@nokia.com)
**
** This file is part of an NFC example for Qt Mobility.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
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
