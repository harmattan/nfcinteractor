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

#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include <QtDeclarative>
#include <QDeclarativePropertyMap>
#ifdef Q_OS_SYMBIAN
//#include <QSystemScreenSaver>
#endif
#include "nfcinfo.h"
#include "tagimagecache.h"
#include "nfctypes.h"
#include "nfcrecordmodel.h"
#include "nfcrecorditem.h"
#if defined(USE_IAP) && defined (Q_OS_SYMBIAN)
#include "iapmanager.h"

// Define content IDs
#define IAP_ID_ADV_TAGS "524700"
#define IAP_ID_REMOVE_ADS "524701"
#endif

#if defined(USE_IAA)
#include "iaa/adinterface.h"
#endif


/*!
  \brief Get the height of a font, including its line spacing.

  This is required to vertically center the symbol to the text line
  in the Nfc Info view. The text can have a height of multiple lines,
  nevertheless, the icon needs to be centered to the first line and
  not the multiline text.

  Specially important to adapt to the font height changes between
  MeeGo Harmattan PR 1.1 and PR 1.2 (line spacing increased in PR 1.2,
  to increase text legibility).
  */
int getFontHeight(const QString& fontName, const int fontPixelSize) {
    QFont *font = new QFont(fontName);
    font->setPixelSize(fontPixelSize);
    QFontMetrics *fm = new QFontMetrics(*font);
    const int fontHeight = fm->height();
    delete fm;
    delete font;
    return fontHeight;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#ifdef USE_IAP
    qDebug() << "Using IAP";
#endif

#ifdef Q_OS_SYMBIAN
    // Translation for NFC Status messages
    QString locale = QLocale::system().name();

    QTranslator translator;
    translator.load(QString("nfcinteractor_") + locale);
    app.installTranslator(&translator);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
#endif

    // Create a variable that stores the platform, to customize the
    // screen appearance in QML accordingly.
#if defined(Q_OS_SYMBIAN)
    int platformId = 0;
#elif defined(MEEGO_EDITION_HARMATTAN)
    int platformId = 1;
#elif defined(QT_SIMULATOR)
    int platformId = 2;
#else
    int platformId = 3;
#endif

    // Register the Nfc interaction classes to the QML environment
    qmlRegisterType<NfcInfo>("Nfc", 1, 0, "NfcInfo");
    qmlRegisterType<NfcRecordModel>("Nfc", 1, 0, "NfcRecordModel");
    qmlRegisterType<NfcRecordItem>("Nfc", 1, 0, "NfcRecordItem");
    qmlRegisterUncreatableType<NfcTypes>("com.nfcinfo.types", 1, 0,
                                                 "NfcTypes", "This exports NfcTypes enums to QML");

    // Setup QML Viewer
    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.rootContext()->setContextProperty("platform", platformId);

    // Symbian has style constants defined in platformStyle, MeeGo has a
    // different system.
    // To minimize QML code differences between both versions, define
    // a customPlatformStyle here, used by both versions.
    // (Unfortunately it's not possible to use the original
    // "platformStyle" on Symbian and set a "platformStyle" on MeeGo,
    // because the Window already has platformStyle property, which
    // would have preference over setting a context property from here)
    QDeclarativePropertyMap *platformStyle = new QDeclarativePropertyMap();
    platformStyle->insert("paddingSmall", QVariant(4));
    platformStyle->insert("paddingMedium", QVariant(8));
    platformStyle->insert("paddingLarge", QVariant(12));
    platformStyle->insert("fontSizeSmall", QVariant(18));
    platformStyle->insert("colorNormalLight", QVariant(QColor(255, 255, 255)));
    platformStyle->insert("colorNormalMid", QVariant(QColor(153, 153, 153)));
#if defined(MEEGO_EDITION_HARMATTAN)
    qDebug() << "MeeGo UI platform style";
    platformStyle->insert("fontFamilyRegular", QVariant(QString("Nokia Pure Text")));
    platformStyle->insert("fontSizeMedium", QVariant(22));
    platformStyle->insert("fontHeightMedium", QVariant(getFontHeight(QString("Nokia Pure Text"), 22)));
    platformStyle->insert("fontSizeLarge", QVariant(26));
#else
    qDebug() << "Symbian UI platform style";
    platformStyle->insert("fontFamilyRegular", QVariant(QFont().defaultFamily()));
    platformStyle->insert("fontSizeMedium", QVariant(20));
    platformStyle->insert("fontHeightMedium", QVariant(getFontHeight(QFont().defaultFamily(), 20)));
    platformStyle->insert("fontSizeLarge", QVariant(22));
#endif
    viewer.rootContext()->setContextProperty("customPlatformStyle", platformStyle);

    // Register the image provider class to QML
    TagImageCache *tagImageCache = new TagImageCache();
    viewer.engine()->addImageProvider(QLatin1String("nfcimageprovider"), tagImageCache);

#ifdef USE_IAP
    // In App Purchasing
    viewer.rootContext()->setContextProperty("useIap", QVariant(true));
    #if defined(Q_OS_SYMBIAN)
        // In App Purchasing APIs on Symbian
        IapManager iapManager;
        // Add the known items to the list
        // Also retrieves if the user has already purchased the items
        // from the internal DB, to prevent the need to go online
        // every time.
        iapManager.addIapProduct(IAP_ID_ADV_TAGS);
        iapManager.addIapProduct(IAP_ID_REMOVE_ADS);
        viewer.rootContext()->setContextProperty("iapManager", &iapManager);
        viewer.rootContext()->setContextProperty("iapIdAdvTags", QVariant(IAP_ID_ADV_TAGS));
        viewer.rootContext()->setContextProperty("iapIdRemoveAds", QVariant(IAP_ID_REMOVE_ADS));
    #else
        // Unlimited version on Harmattan
        // No IAP APIs available -> separate product on Nokia Store
    #endif
#else
    viewer.rootContext()->setContextProperty("useIap", QVariant(false));
#endif
#ifdef IAP_TEST_MODE
    viewer.rootContext()->setContextProperty("iapTestMode", QVariant(true));
#else
    viewer.rootContext()->setContextProperty("iapTestMode", QVariant(false));
#endif

#ifdef USE_IAA
    // In App Advertising
    AdInterface adI;
    viewer.rootContext()->setContextProperty("adInterface", &adI);
    viewer.rootContext()->setContextProperty("useIaa", QVariant(true));
#else
    viewer.rootContext()->setContextProperty("useIaa", QVariant(false));
#endif

    // Finally, load the main QML file to the viewer.
    viewer.setMainQmlFile(QLatin1String("qml/main.qml"));

    // Inform the NfcInfo class about the image cache
    // The NfcInfo has been instantiated through QML, so retrieve its instance
    // through the QML object tree.
    NfcInfo *nfcInfoObj = viewer.rootObject()->findChild<NfcInfo*>("nfcInfoObj");
    if (nfcInfoObj) {
        nfcInfoObj->setImageCache(tagImageCache);
        // Pass the record model back to the QML
        // Could be easier to just make the model a property?
        viewer.rootContext()->setContextProperty("recordModel", nfcInfoObj->recordModel());
        // Give the ndef manager a pointer to our declarative view,
        // so that it can raise the app to the foreground when an
        // autostart tag is touched and the app is already running.
        nfcInfoObj->setDeclarativeView(viewer);
#ifdef USE_IAP
    #ifdef Q_OS_SYMBIAN
        nfcInfoObj->setUnlimitedAdvancedMsgs(iapManager.isProductPurchased(IAP_ID_ADV_TAGS));
    #else
        nfcInfoObj->setUnlimitedAdvancedMsgs(false);
    #endif
#endif
    }

    viewer.showExpanded();

//#ifdef Q_OS_SYMBIAN
//    // Prevent screensaver from kicking in on Symbian
//    QSystemScreenSaver *screensaver = new QSystemScreenSaver ( &viewer );
//    screensaver->setScreenSaverInhibit();
//#endif

    return app.exec();
}
