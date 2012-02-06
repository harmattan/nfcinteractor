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

#ifndef IAPMANAGER_H
#define IAPMANAGER_H

#include <QObject>
#include <QDebug>
#include <QSettings>
#include <iapclient.h>
#include "iapproduct.h"
#include "nfctypes.h"

class IapManager : public QObject
{
    Q_OBJECT
public:
    explicit IapManager(QObject *parent = 0);
    

public:
    Q_INVOKABLE void initIapEngine();
    Q_INVOKABLE void getProductData();
    Q_INVOKABLE bool isProductPurchased(const QString &productId);
    Q_INVOKABLE QString productPrice(const QString &productId);
    Q_INVOKABLE void purchaseProduct(const QString &productId);
    /*! Remove purchased product. Debug purposes only, no use in release version. */
    Q_INVOKABLE void removePurchasedProduct(const QString &productId);
    Q_INVOKABLE void restoreProducts();

    void addIapProduct(const QString &productId);

signals:
    void productDataAvailable(const QString& productId);
    void productPurchased(const QString& productId);
    void productRestored(const QString& productId);
    void iapError(const QString& errorMsg);
    
public slots:

protected slots:
    // Get product data
    void productDataReceived (int requestId, QString status, IAPClient::ProductDataHash productData);

    // Purchase product
    void purchaseCompleted (int requestId, QString status, QString purchaseTicket);
    void purchaseFlowFinished (int requestId);

    // Restoring
    void restorableProductsReceived (int requestId, QString status, IAPClient::ProductDataList items);
    void restorationCompleted (int requestId, QString status, QString purchaseTicket);

private:
    void checkIapClientInstance();
    IapProduct *getIapProduct(const QString &productId);
    IapProduct *getProductWithRequestId(const int requestId);
    bool iDbCheckProductPurchased(const QString &productId);
    void iDbSetProductPurchased(const QString &productId, const bool productPurchased);

private:
    // In-Application Purchase API
    IAPClient* m_client;
    QList<IapProduct*> m_iapProducts;
};

#endif // IAPMANAGER_H
