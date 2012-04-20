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
#include "nearfieldtargetinfo.h"

/*!
  \brief New target info, using default values.
  */
NearFieldTargetInfo::NearFieldTargetInfo()
{
    resetInfo();
}

/*!
  \brief Reset all stored data to the defaults.
  */
void NearFieldTargetInfo::resetInfo()
{
    tagTypeName = "";
    tagMajorVersion = 0;
    tagMinorVersion = 0;
    tagMemorySize = -1;
    tagWritableSize = -1;
    tagReadAccessCC = NfcAccessUnknown;
    tagWriteAccessCC = NfcAccessUnknown;
    tagReadAccessLockBits = NfcAccessUnknown;
    tagWriteAccessLockBits = NfcAccessUnknown;
    tagMemoryType = NfcMemoryUnknown;
}

/*!
  \brief Check if both the lock bits of the tag as well as the
  capability container (CC) indicate that the tag is readable.
  */
NearFieldTargetInfo::NfcTagAccessStatus NearFieldTargetInfo::combinedReadAccess() const
{
    return combineAccess(tagReadAccessCC, tagReadAccessLockBits);
}

/*!
  \brief Check if both the lock bits of the tag as well as the
  capability container (CC) indicate that the tag is writable.
  */
NearFieldTargetInfo::NfcTagAccessStatus NearFieldTargetInfo::combinedWriteAccess() const
{
    return combineAccess(tagWriteAccessCC, tagWriteAccessLockBits);
}

/*!
  \brief Internal method used to combine the access status of CC and lock bits.

  If either CC or lock bits, or both indicate it's forbidden, it is overall forbidden.
  If either CC or lock bits, or both indicate that access is allowed, it is overall allowed.
  In all othercases, the access is unknown.
  */
NearFieldTargetInfo::NfcTagAccessStatus NearFieldTargetInfo::combineAccess(const NfcTagAccessStatus accessCC, const NfcTagAccessStatus accessLockBits) const
{
    if (accessCC == NfcAccessForbidden ||
        accessLockBits == NfcAccessForbidden) {
        // If one access states access is forbidden, best estimate is
        // that it's not allowed in general.
        return NfcAccessForbidden;
    } else if (accessCC == NfcAccessAllowed ||
        accessLockBits == NfcAccessAllowed) {
        // Both CC and lock bits indicate that the access is allowed
        return NfcAccessAllowed;
    } else
    // Mixed status or unknown in general
    return NfcAccessUnknown;
}
