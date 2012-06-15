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
