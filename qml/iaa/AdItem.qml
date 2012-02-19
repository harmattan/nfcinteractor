import QtQuick 1.1
import "adFunctions.js" as AdF
/*!
  \qmlclass AdItem
  \ingroup com.inneractive
  \brief Item to show Ad banner.
  AdItem shows image and optionaly text (see \l {showText}).
  The AdItem component is part of the \l {inneractive QML Components} module.
  */
Item {
    id: root
    Component.onCompleted: {
        requestAd()
    }
    width: scaleAd ? width : adImage.width
    height: scaleAd ? height : (showText ? adImage.height + adText.paintedHeight : adImage.height)

    /*!
        \qmlproperty string AdItem::adTextString
         Text string of ad.
         Read-only
      */
    property alias adTextString: adText.text
    /*!
      \qmlproperty string AdItem::adImageUrl
      Url of image shown in ad.
      Read-only
      */
    property alias adImageUrl: adImage.source
    /*!
      Url which is opened when user clicks ad.
      Read-only
      */
    property url adClickUrl
    /*!
        \qmlproperty boolean AdItem::textClip
         Clip text of ad. See Text::clip.
      */
    property alias textClip: adText.clip
    /*!
      Show text in AdItem.
      The default value is true
      */
    property bool showText: true
    /*!
      Hide ad after user has clicked it open.
      The default value is true
      */
    property bool hideOnClick: true
    /*!
      Scale ad image to fit width and height.
      If false, AdItem will be scaled to contain image and text.
      The default value is false
      */
    property bool scaleAd: false
    /*!
      Ad request is automatically retried if previous request fails.
      The default value is false
      */
    property bool retryOnError: false
    /*!
      Interval at which AdItem requests new ad, in seconds.
      Value 0 disables automatic requests and requestAd() needs to be called to get new ad.
      The default value is 0
      */
    property int rotationInterval: 0
    /*!
      Status of the ad request.
      \list
        \o "Null" request not done yet
        \o "Loading" request is not yet ready
        \o "Error" request failed, error message set to \l {errorString}
        \o "Done" ad request loaded successfully
      \endlist
      Read-only
      */
    property string status: "Null"
    /*!
      Readable cause of last occured error.
      Read-only
      */
    property string errorString
    /*!
      Parameters used in ad request.
      This is mandatory and AdParameters have to contain \l {AdParameters::applicationId}{applicationId} and \l {AdParameters::distributionId}{distributionId}.
      */
    property AdParameters parameters: null

    /*!
      This signal is emited when ad request has completed and no error has occured.
      */
    signal adLoaded()
    /*!
      This signal is emited when ad request has failed.
      errorString contains message why request failed.
      */
    signal adError(string errorString)
    /*!
      This signal is emited when ad is clicked.
      */
    signal adClicked()
    /*!
      \brief Requests new Ad
      Status will update to "Loading".
    */
    function requestAd()
    {
        rotationTimer.running = false;
        retryTimer.running = false;
        status = "Loading";
        adInterface.requestAd(root);
    }
    /*!
      \internal
      Property alias for RequestQueue to set reply xml
      */
    property alias __xml: adModel.xml
    /*!
      \internal
      */
    property url __query
    /*!
      \internal
      Used by RequestQueue to create __query url
      */
    function __createQuery()
    {
        AdF.createQuery(root);
    }
    /*!
      \internal
      Used by RequestQueue to update AdParameters::__clientId
      */
    function __idUpdated(id)
    {
        parameters.__clientId = id;
    }

    // Handle networkaccessibility change
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
        if (retryOnError && adInterface.networkAccessible)
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
            adInterface.openAd(root.adClickUrl);
            adClicked();
            if (root.hideOnClick) {
                root.visible = false;
            }
        }
    }
}
