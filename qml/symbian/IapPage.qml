import QtQuick 1.1
import com.nokia.symbian 1.1 // Symbian Qt Quick components

Page {
    id: iapPage
    tools: ToolBarLayout {
        ToolButton {
            flat: true
            iconSource: "toolbar-back";
            onClicked: pageStack.depth <= 1 ? Qt.quit() : pageStack.pop()
        }
//        ToolButton {
//            flat: true
//            visible: !allItemPurchased()
//            text: "Restore"
//            onClicked: iapManager.restoreItems()
//        }
        ToolButton {
            flat: true
            // TODO: debug only
            iconSource: "toolbar-delete";
            onClicked: {
                iapManager.removePurchasedProduct(iapIdAdvTags);
                iapManager.removePurchasedProduct(iapIdRemoveAds);
                applyPurchaseStatus(iapIdAdvTags);
                applyPurchaseStatus(iapIdRemoveAds);
            }
        }
    }

    Connections {
        target: iapManager
        onProductDataAvailable: {
            console.log("QML: Product data received");
            applyPurchasePrice(productId);
        }
        onProductPurchased: {
            applyPurchaseStatus(productId);
        }
        onProductRestored: {
            applyPurchaseStatus(productId);
        }
        onIapError: {
            // TODO
        }
    }

    function applyPurchaseStatus(productId) {
        // Get purchase status from the item
        var isPurchased = iapManager.isProductPurchased(productId);
        console.log("isPurchased: " + isPurchased);
        var color = isPurchased ? customPlatformStyle.colorNormalMid : customPlatformStyle.colorNormalLight;
        applyPurchasePrice(productId);
        var applyToItem = getQmlItemForProductId(productId);
        applyToItem.textColor = color;
        if (productId === iapIdAdvTags) {
            window.setUnlimitedAdvancedMsgs(isPurchased);
        }
    }

    function applyPurchasePrice(productId) {
        var price = iapManager.productPrice(productId);
        var isPurchased = iapManager.isProductPurchased(productId);
        var applyToItem = getQmlItemForProductId(productId);
        console.log("Price: " + price + ", isPurchased: " + isPurchased + ", item: " + applyToItem);

        if (isPurchased) {
            applyToItem.price = qsTr("Activated (already purchased)");
        } else {
            applyToItem.price = price;
        }
    }

    function getQmlItemForProductId(productId) {
        return (productId === iapIdAdvTags) ? iapAdvTagsItem : iapRemoveAdsItem;
    }

    function allItemPurchased() {
        return iapManager.isProductPurchased(iapIdAdvTags) && iapManager.isProductPurchased(iapIdRemoveAds);
    }

    Text {
        id: messageHeader
        text: qsTr("Purchase Upgrades")
        font.bold: true
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        wrapMode: Text.WordWrap
        height: customPlatformStyle.fontSizeLarge + customPlatformStyle.paddingMedium
        font.family: customPlatformStyle.fontFamilyRegular;
        font.pixelSize: customPlatformStyle.fontSizeLarge
        color: "aliceblue";
    }

    // -------------------------------------------------------------
    // Purchasable Items
    Flickable {
        anchors.top: messageHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        contentHeight: iapAdvTagsItem.height + iapRemoveAdsItem.height
        contentWidth: parent.width
        clip: true

        IapItem {
            id: iapAdvTagsItem
            productId: iapIdAdvTags
            anchors.top: parent.top
            imageUrl: "icon.png"
            title: qsTr("Advanced Tag Writing")
            description: qsTr("Write an unlimited number of advanced tag types, like business cards, geo tags or social network tags.")
            price: qsTr("Checking...")
        }
        IapItem {
            id: iapRemoveAdsItem
            productId: iapIdRemoveAds
            anchors.top: iapAdvTagsItem.bottom
            imageUrl: "icon.png"
            title: qsTr("Remove Ads")
            description: qsTr("Use the application without ads, also enabling the use in offline mode and without data connection when roaming.")
            price: qsTr("Checking...")
        }
    }


    Component.onCompleted: {
        // Check if the products have been purchased already
        applyPurchaseStatus(iapIdAdvTags);
        applyPurchaseStatus(iapIdRemoveAds);
        // Let the page transition animations finish, and then
        // put the IAP APIs into use (which will stall the app
        // briefly as the IAP Client APIs initialize themselves).
        timer.restart();
    }

    Timer {
        id: timer
        interval: 500   // Give enough time for page transitions to finish
        repeat: false
        onTriggered: {
            // Connect to the IAP service and check the product data
            console.log("QML: Checking product data");
            iapManager.getProductData();
            // Check if we can restore any items
            iapManager.restoreProducts();
        }
    }

}
