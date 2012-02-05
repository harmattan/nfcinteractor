// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import QtMobility.systeminfo 1.1
Item {
    DeviceInfo {
        id: deviceInfo
    }


    //Required parameters, these must be setted
    property string applicationId
    property string distributionId
    property string version: "Sm2m-1.5.3" //Is there need to change this at some point?

    //Optional parameters
    property string userAge
    property string userGender // M/m, F/f, Male, Female
    property string keywords /* Comma separated list relevant to this user’s
                                specific session. */
    property string category // a single word description of the application.
    property string mobileNumber // MSISDN format, with international prefix

    property bool usePositioning: false
    property bool useLocation: false

    //No need to touch these
    property string __clientId // Handled by request functions
    property string __imei: deviceInfo.imei // Use mobility to get imei of device
    property string __screenWidth: screen ? screen.displayWidth : ""
    property string __screenHeight: screen ? screen.displayHeight : ""
    property string __gpsLoc: "" // lat,long // if usePositioning true
    property string __location: useLocation ? "" // TODO: need implementation
                                            : "" // country,state/province,city (US,NY,NY)
}
