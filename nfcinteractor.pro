TARGET = nfcinteractor
VERSION = 2.00.0

symbian {
    # IAP only available on the Symbian platform
    DEFINES += USE_IAP
    # Enables test mode for IAP
    DEFINES += IAP_TEST_MODE
}

CONFIG += mobility qt-components
# In App Purchasing
contains(DEFINES,USE_IAP) {
    symbian:CONFIG += inapppurchasing
}
MOBILITY += sensors connectivity systeminfo versit contacts location

# Define QMLJSDEBUGGER to allow debugging of QML in debug builds
# (This might significantly increase build time)
# DEFINES += QMLJSDEBUGGER

# Define for detecting Harmattan in .cpp files.
# Only needed for experimental / beta Harmattan SDKs.
# Will be defined by default in the final SDK.
exists($$QMAKE_INCDIR_QT"/../qmsystem2/qmkeys.h"):!contains(MEEGO_EDITION,harmattan): {
    MEEGO_VERSION_MAJOR     = 1
    MEEGO_VERSION_MINOR     = 2
    MEEGO_VERSION_PATCH     = 0
    MEEGO_EDITION           = harmattan
    DEFINES += MEEGO_EDITION_HARMATTAN
}

OTHER_FILES += \
    qml/images/*.svg \
    qml/images/*.png \
    qml/symbian/IapPage.qml \
    qml/symbian/IapItem.qml

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    nfcinfo.cpp \
    ndefnfcsprecord.cpp \
    ndefnfcmimevcardrecord.cpp \
    tagimagecache.cpp \
    ndefnfcmimeimagerecord.cpp \
    nfcrecordmodel.cpp \
    nfcrecorditem.cpp \
    nfcmodeltondef.cpp \
    nfctargetanalyzer.cpp \
    nfcndefparser.cpp \
    ndefnfcgeorecord.cpp \
    ndefnfcsmarturirecord.cpp \
    ndefnfcsmsrecord.cpp \
    nearfieldtargetinfo.cpp \
    ndefnfcsocialrecord.cpp \
    ndefnfcstorelinkrecord.cpp \
    nfcstats.cpp

HEADERS += \
    nfcinfo.h \
    ndefnfcsprecord.h \
    ndefnfcmimevcardrecord.h \
    tagimagecache.h \
    ndefnfcmimeimagerecord.h \
    nfctypes.h \
    nfcrecordmodel.h \
    nfcrecorditem.h \
    nfcmodeltondef.h \
    nfctargetanalyzer.h \
    nfcndefparser.h \
    ndefnfcgeorecord.h \
    ndefnfcsmarturirecord.h \
    ndefnfcsmsrecord.h \
    nearfieldtargetinfo.h \
    ndefnfcsocialrecord.h \
    ndefnfcstorelinkrecord.h \
    nfcstats.h

simulator {
    # The simulator uses the QML and images from Symbian,
    # as it doesn't have support for simulating Qt Quick Components for
    # MeeGo Harmattan.
    OTHER_FILES += \
        qml/symbian/*.qml

    qmlFolder.source = qml/symbian/*
    qmlFolder.target = qml
    QML_IMPORT_PATH = qml/symbian

    OTHER_FILES += \
        qml/images-symbian/*.qml
    imagesFolderSymbian.source = qml/images-symbian/*
    imagesFolderSymbian.target = qml
    DEPLOYMENTFOLDERS += imagesFolderSymbian
}

symbian {
    DEPLOYMENT.display_name = "NfcInteractor"
    TARGET.UID3 = 0xE0D154A1

    # Allow network access on Symbian
    TARGET.CAPABILITY += NetworkServices LocalServices

    # add NfcSettings support
    include(nfcsettings/nfcsettings.pri)

    OTHER_FILES += \
        qml/symbian/*.qml

    qmlFolder.source = qml/symbian/*
    qmlFolder.target = qml
    QML_IMPORT_PATH = qml/symbian

    # Symbian specific images
    OTHER_FILES += \
        qml/images-symbian/*.qml
    imagesFolderSymbian.source = qml/images-symbian/*
    imagesFolderSymbian.target = qml
    DEPLOYMENTFOLDERS += imagesFolderSymbian

    #qmlImages.sources = qml/images/*
    #qmlImages.path = .
    #DEPLOYMENT += qmlImages

    # Smart Installer package's UID
    # This UID is from the protected range and therefore the package will
    # fail to install if self-signed. By default qmake uses the unprotected
    # range value if unprotected UID is defined for the application and
    # 0x2002CCCF value if protected UID is given to the application
    #symbian:DEPLOYMENT.installer_header = 0x2002CCCF

    # Add dependency to symbian components
    # CONFIG += qtquickcomponents
	
    # Autostart
    ndefhandler.sources = ndefhandler_nfcinteractor.xml
    ndefhandler.path = c:/private/2002AC7F/import/
    DEPLOYMENT += ndefhandler

    # Localisation support.
    CODECFORTR = UTF-8
    TRANSLATIONS += loc/$${TARGET}_en.ts \
                    loc/$${TARGET}_ar.ts \
                    loc/$${TARGET}_zh_HK.ts \
                    loc/$${TARGET}_zh_CN.ts \
                    loc/$${TARGET}_zh_TW.ts \
                    loc/$${TARGET}_cs.ts \
                    loc/$${TARGET}_da.ts \
                    loc/$${TARGET}_nl.ts \
                    loc/$${TARGET}_en_US.ts \
                    loc/$${TARGET}_fi.ts \
                    loc/$${TARGET}_fr.ts \
                    loc/$${TARGET}_fr_CA.ts \
                    loc/$${TARGET}_de.ts \
                    loc/$${TARGET}_el.ts \
                    loc/$${TARGET}_he.ts \
                    loc/$${TARGET}_hi.ts \
                    loc/$${TARGET}_hu.ts \
                    loc/$${TARGET}_id.ts \
                    loc/$${TARGET}_it.ts \
                    loc/$${TARGET}_ms.ts \
                    loc/$${TARGET}_no.ts \
                    loc/$${TARGET}_pl.ts \
                    loc/$${TARGET}_pt.ts \
                    loc/$${TARGET}_pt_BR.ts \
                    loc/$${TARGET}_ro.ts \
                    loc/$${TARGET}_ru.ts \
                    loc/$${TARGET}_sk.ts \
                    loc/$${TARGET}_es.ts \
                    loc/$${TARGET}_es_419.ts \
                    loc/$${TARGET}_sv.ts \
                    loc/$${TARGET}_th.ts \
                    loc/$${TARGET}_tr.ts \
                    loc/$${TARGET}_uk.ts \
                    loc/$${TARGET}_vi.ts

    translationfiles.source = loc/*.qm
    DEPLOYMENTFOLDERS += translationfiles

    vendorName = "Nokia"
    vendorinfo = \
        "; Localised Vendor name" \
        "%{$$addLanguageDependentPkgItem(vendorName)}" \
        " " \
        "; Unique Vendor name" \
        ":\"$$vendorName\"" \
        " "

    deployment_vendor.pkg_prerules += vendorinfo
    DEPLOYMENT += deployment_vendor

    # In App Purchasing
    contains(DEFINES,USE_IAP) {
        message(Using In-App Purchasing)
#        LIBS += -lcaf \
#            -lcafutils \
#            -lapmime \

        # *** including inappbilling below
        LIBS += -liapclientapi
        # NOTE: this is enabling stdlib to prevent error 'undefined symbol __aeabi_atexit'
        LIBS += -lusrt2_2

        # capabilities required for IAP API
        TARGET.CAPABILITY += NetworkServices

        iap_dependency.pkg_prerules = "; Has dependency on IAP component" \
                                      "(0x200345C8), 0, 2, 2, {\"IAP\"}"
        DEPLOYMENT += iap_dependency

        SOURCES += \
            iap/iapmanager.cpp \
            iap/iapproduct.cpp

        HEADERS += \
            iap/iapmanager.h \
            iap/iapproduct.h

        DEPENDPATH += iap

        # IAP API files to include in package
        addIapFiles.sources = ./iap_data/IAP_VARIANTID.txt
        addIapFiles.path = ./

        # For testing In-App Purchase without Nokia Store
        contains(DEFINES, IAP_TEST_MODE) {
            message(In-App Purchase API in TEST_MODE)
            addIapTestFiles.sources = ./iap_data/TEST_MODE.txt
            addIapTestFiles.path = .
            DEPLOYMENT += addIapTestFiles
        }

        # resources to be DRM protected
        addDrmFiles01.sources = ./iap_data/advancedtags/active.dat
        addDrmFiles01.path = ./drm/data/resourceid_524700/

        addDrmFiles02.sources = ./iap_data/removeads/active.dat
        addDrmFiles02.path = ./drm/data/resourceid_524701/
        DEPLOYMENT += addIapFiles addDrmFiles01 addDrmFiles02
    }
}

contains(MEEGO_EDITION,harmattan) {
    # Temp
    DEFINES += MEEGO_EDITION_HARMATTAN

    # Speed up launching on MeeGo/Harmattan when using applauncherd daemon
    CONFIG += qdeclarative-boostable

    OTHER_FILES += qtc_packaging/debian_harmattan/*

    OTHER_FILES += \
        qml/meego/*.qml

    qmlFolder.source = qml/meego/*
    qmlFolder.target = qml
    QML_IMPORT_PATH = qml/meego

    # Don't use nfcinfo_harmattan.desktop. Otherwise,
    # the NDEF Autostart handler won't find the desktop file and
    # will not be able to auto-launch this app on tag-touch.
    # See: https://bugreports.qt.nokia.com/browse/QTMOBILITY-1848
    harmattandesktopfile.files = nfcinteractor.desktop
    harmattandesktopfile.path = /usr/share/applications
    INSTALLS += harmattandesktopfile

    # To avoid conflicts, recommended to name this file according to the
    # full service name instead of just the app name.
    # See: https://bugreports.qt.nokia.com/browse/QTMOBILITY-1848
    ndefhandler_service.files = com.nokia.qtmobility.nfc.nfcinteractor.service
    ndefhandler_service.path = /usr/share/dbus-1/services/
    INSTALLS += ndefhandler_service
	
    launchericon.files = nfcinteractor80.png splash-nfcinteractor-l.png splash-nfcinteractor-p.png
    launchericon.path = /opt/nfcinteractor/
    INSTALLS += launchericon
}

commonFolder.source = qml/common/*
commonFolder.target = qml
imagesFolder.source = qml/images/*
imagesFolder.target = qml
DEPLOYMENTFOLDERS += qmlFolder commonFolder imagesFolder

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

