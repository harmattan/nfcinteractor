import QtQuick 1.1
import "adFunctions.js" as AdF

Item {
    id: root
    Component.onCompleted: {
        requestAd()
    }
    width: scaleAd ? width : adImage.width
    height: scaleAd ? height : (showText ? adImage.height + adText.paintedHeight : adImage.height)

    property alias adTextString: adText.text
    property alias adImageUrl: adImage.source
    property url adClickUrl
    property alias textClip: adText.clip
    property bool showText: true
    property bool hideOnClick: true
    property bool scaleAd: false
    property bool retryOnError: false
    property int rotationInterval: 0 // interval in seconds to get new ad, 0 no rotate
    property string status: "Null" //Null, Loading, Error, Done
    property string errorString
    property AdParameters parameters: null

    signal adLoaded()
    signal adError(string errorString)
    signal adClicked()

    function requestAd()
    {
        rotationTimer.running = false;
        retryTimer.running = false;
        status = "Loading";
        adInterface.requestAd(root);
    }

    property alias __xml: adModel.xml
    property url __query

    function __createQuery()
    {
        AdF.createQuery(root);
    }
    function __idUpdated(id)
    {
        parameters.__clientId = id;
    }

    // Handle networkaccessibility changing
    Connections {
        target: adInterface
        onNetworkAccessibilityChanged: {
            if (!adInterface.networkAccessible)
                return;
            if ((status == "Error" && retryOnError) || status == "Null")
                requestAd();
        }
    }

    onAdError: {
        status = "Error";
        root.errorString = errorString;
        if (retryOnError)
            retryTimer.start();
        console.debug("Ad Error: " + errorString);
    }
    onAdLoaded: {
        status = "Done";
        console.debug("Ad Loaded");
    }


    Timer {
        id: rotationTimer
        interval: rotationInterval * 1000
        onTriggered: requestAd()
    }
    Timer {
        id: retryTimer
        interval: 5000
        onTriggered: requestAd()
    }

    XmlListModel {
        id: adModel
        namespaceDeclarations: "declare default element namespace 'http://www.inner-active.com/SimpleM2M/M2MResponse';"
        query: "/Response/Ad"
        XmlRole { name: "text"; query: "Text/string()" }
        XmlRole { name: "url"; query: "URL/string()" }
        XmlRole { name: "image"; query: "Image/string()" }
        onStatusChanged: {
            if (status == XmlListModel.Ready && adModel.count > 0) {
                    adTextString = adModel.get(0).text;
                    adClickUrl = adModel.get(0).url;
                    adImageUrl = adModel.get(0).image;
                    root.visible = true;
                    adLoaded();
                    if (rotationInterval > 0) {
                        rotationTimer.start();
                    }
            } else if (status == XmlListModel.Error) {
                root.visible = false;
                adError(errorString());
            }
        }
    }

    Image {
        id: adImage
        width: scaleAd ? root.width : sourceSize.width
        height: scaleAd
                ? (showText ? root.height - adText.paintedHeight : root.height)
                : sourceSize.height
        anchors.horizontalCenter: root.horizontalCenter
        anchors.top: root.top
        fillMode: Image.PreserveAspectFit
    }
    Text {
        id: adText
        width: scaleAd ? root.width : adImage.sourceSize.width
        clip: true
        wrapMode: Text.WordWrap
        visible: showText
        anchors.horizontalCenter: root.horizontalCenter
        anchors.top: adImage.bottom
    }

    MouseArea {
        id: clickArea
        anchors.fill: root
        onClicked: {
            Qt.openUrlExternally(root.adClickUrl);
            adClicked();
            if (root.hideOnClick) {
                root.visible = false;
            }
        }
    }
}
