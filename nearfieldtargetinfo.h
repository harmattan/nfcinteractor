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
#ifndef NEARFIELDTARGETINFO_H
#define NEARFIELDTARGETINFO_H

#include <QDebug>

/*!
  \brief Stores additional information about a QNearFieldTarget.
  */
class NearFieldTargetInfo
{
public:
    NearFieldTargetInfo();
    void resetInfo();

    enum NfcTagAccessStatus {
        NfcAccessUnknown,
        NfcAccessAllowed,
        NfcAccessForbidden
    };
    enum NfcTagMemoryType {
        NfcMemoryUnknown,
        NfcMemoryStatic,
        NfcMemoryDynamic
    };

    NearFieldTargetInfo::NfcTagAccessStatus combinedReadAccess() const;

    NearFieldTargetInfo::NfcTagAccessStatus combinedWriteAccess() const;

private:
    NearFieldTargetInfo::NfcTagAccessStatus combineAccess(const NfcTagAccessStatus accessCC, const NfcTagAccessStatus accessLockBits) const;

public:
    QString tagTypeName;
    int tagMajorVersion;
    int tagMinorVersion;
    int tagMemorySize;
    int tagWritableSize;
    NfcTagAccessStatus tagReadAccessCC;
    NfcTagAccessStatus tagWriteAccessCC;
    NfcTagAccessStatus tagReadAccessLockBits;
    NfcTagAccessStatus tagWriteAccessLockBits;
    NfcTagMemoryType tagMemoryType;

};


#endif // NEARFIELDTARGETINFO_H
