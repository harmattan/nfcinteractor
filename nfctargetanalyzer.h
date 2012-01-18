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
#ifndef NFCTARGETANALYZER_H
#define NFCTARGETANALYZER_H

#include <QObject>
#include <QByteArray>
#include <QVariant>
#include <QDebug>
#include <QUrl>
#include <QNearFieldTarget>
#include <QNearFieldTagType1>
#include <QNearFieldTagType2>
#include "nearfieldtargetinfo.h"

#define TYPE1_STATIC_WRITABLE_SIZE 96
#define TYPE2_STATIC_MEMORY_SIZE 48
#define NDEF_MAGIC_NUMBER 0xE1

// Typical writable tag size
#define GUESS_TAG_WRITABLE_SIZE_TYPICAL_TLV_SIZE 6

// Memory availability is 96 bytes and expandable to 2 kbyte.
#define GUESS_TYPE1_SIZE (TYPE1_STATIC_WRITABLE_SIZE - GUESS_TAG_WRITABLE_SIZE_TYPICAL_TLV_SIZE)
// Memory availability is 48 bytes and expandable to 2 kbyte.
// Assume a dynamic tag with 144 bytes memory size, as 48 bytes is really small
#define GUESS_TYPE2_SIZE (144 - GUESS_TAG_WRITABLE_SIZE_TYPICAL_TLV_SIZE)
// Memory availability is variable, theoretical memory limit is 1MByte per service.
// FeliCa Lite provides 14 blocks of user memory (224 bytes)
// Standard FeliCa provides 154 blocks (2464 bytes)
#define GUESS_TYPE3_SIZE 224
// The memory availability is variable, up to 32 KBytes per service
#define GUESS_TYPE4_SIZE 2048
// The MIFARE Classic 1K offers 1024 bytes of data storage
#define GUESS_MIFARE_SIZE 1024

QTM_USE_NAMESPACE

/*!
  \brief Analyze the NFC target and return the gathered information
  in human-readable textual form.

  The output contains in all cases information about the tag type,
  UID and access methods.

  If the underlying Qt Mobility implementation allows tag-type specific
  access, it will further analyze the memory size and several other
  parts of the tag.

  For Type 1 & 2 targets, deeper analysis is performed using low-level
  commands. Note that tag-type specific access is currently only
  implemented for Symbian, and does not work on the N9.

  The NfcNdefParser has a similar task, but returns the tag contents
  in textual form.
  */
class NfcTargetAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit NfcTargetAnalyzer(QObject *parent = 0);
    QString analyzeTarget(QNearFieldTarget *target);
    QString analyzeType1Target(QNearFieldTagType1 *target);
    QString analyzeType2Target(QNearFieldTagType2 *target);

    QString convertTagTypeToString(const QNearFieldTarget::Type type);
    
private:
    
public:
    NearFieldTargetInfo m_tagInfo;
};

#endif // NFCTARGETANALYZER_H
