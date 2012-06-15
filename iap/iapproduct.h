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
#ifndef IAPPRODUCT_H
#define IAPPRODUCT_H

#include <QObject>

class IapProduct : public QObject
{
    Q_OBJECT
public:
    explicit IapProduct(const QString& productId, QObject *parent = 0);
    
public:
    QString productId() const;
    void setProductId(const QString& productId);
    QString price() const;
    void setPrice(const QString& price);
    bool purchased() const;
    void setPurchased(const bool purchased);
    int requestId() const;
    void setRequestId(const int requestId);
    void setRequestFinished();

signals:
    
public slots:
    
private:
    QString m_productId;
    QString m_price;
    bool m_purchased;
    int m_requestId;
};

#endif // IAPITEM_H
