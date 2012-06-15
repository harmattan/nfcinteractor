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

#include "iapproduct.h"


IapProduct::IapProduct(const QString &productId, QObject *parent) :
    QObject(parent),
    m_productId(productId),
    m_price("Checking..."),
    m_requestId(-1)
{
}

QString IapProduct::productId() const
{
    return m_productId;
}

void IapProduct::setProductId(const QString &productId)
{
    m_productId = productId;
}

QString IapProduct::price() const
{
    return m_price;
}

void IapProduct::setPrice(const QString &price)
{
    m_price = price;
}

bool IapProduct::purchased() const
{
    return m_purchased;
}

void IapProduct::setPurchased(const bool activated)
{
    m_purchased = activated;
}

int IapProduct::requestId() const
{
    return m_requestId;
}

void IapProduct::setRequestId(const int requestId)
{
    m_requestId = requestId;
}

void IapProduct::setRequestFinished()
{
    m_requestId = -1;
}
