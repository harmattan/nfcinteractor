// Helper functions
.pragma library
var __baseUrl = "http://m2m1.inner-active.mobi/simpleM2M/clientRequestAd"

function createQuery(adItem)
{
    var parameters = adItem.parameters;
    function buildRequestUrl()
    {
        var requestsUrl = __baseUrl;
        function addQuery(key, value)
        {
            if (value) { // no need to add if no string to add
                if (requestsUrl !== __baseUrl)
                    requestsUrl +="&";
                else
                    requestsUrl +="?";
                requestsUrl += key + "=" + value;
            }
        }

        addQuery("aid", parameters.applicationId);
        addQuery("v", parameters.version);
        addQuery("po", parameters.distributionId);
        addQuery("cid", parameters.__clientId);
        addQuery("hid", parameters.__imei);
        addQuery("w", parameters.__screenWidth);
        addQuery("h", parameters.__screenHeight);

        //Optionals
        addQuery("a", parameters.userAge);
        addQuery("g", parameters.userGender);
        addQuery("k", parameters.keywords);
        addQuery("c", parameters.category);
        addQuery("mn", parameters.mobileNumber);

        if (parameters.usePositioning) {
            addQuery("l", parameters.__location)
        }
        if (parameters.useLocation) {
            addQuery("lg", parameters.__gpsLoc)
        }
        return requestsUrl;
    }
    if (!parameters) {
        console.log("RequestParameters not setted");
        Qt.quit();
        return;
    }

    if (!parameters.applicationId || !parameters.distributionId) {
        console.log("ApplicationID or DistriputionId is empty");
        Qt.quit();
        return;
    }

    adItem.__query = buildRequestUrl();

}
