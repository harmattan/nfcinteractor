// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import QtMobility.systeminfo 1.1
import QtMobility.location 1.1
/*!
    \qmlclass AdParameters
    \ingroup com.inneractive
    \brief Parameters for ad requests
    This item is used to give request parameters for AdItem.
    The AdParameters component is part of the \l {inneractive QML Components} module.
  */
Item {
    DeviceInfo {
        id: deviceInfo
    }

    //Required parameters, these must be setted
    /*!
      Application Id used in ad request.
      You get one when you register your application to inneractive.
      */
    property string applicationId
    /*!
      Distribution Id used in ad request.
      This is id of application distribution origin (eg. "551" for ovi store).
      The default value is "551"
      */
    property string distributionId: "551"
    /*!
      Version used in ad request.
      This is version of inneractive API.
      The default value is "Sm2m-1.5.3"
      */
    property string version: "Sm2m-1.5.3" //Is there need to change this at some point?

    //Optional parameters
    /*!
      Age of applications user.
      */
    property string userAge
    /*!
      Gender of application user.
      Possible values are M, m or Male; or F, f or Female.
      */
    property string userGender
    /*!
      Application keywords.
      Comma separated list of keywords relevant to this application.
      */
    property string keywords
    /*!
      Category of application
      A single word description of the application.
      */
    property string category
    /*!
      Users mobile number.
      MSISDN format, with international prefix.
      */
    property string mobileNumber

    /*!
      Use GPS coordinates in ad request.
      The default value is false
      */
    property bool usePositioning: false
    /*!
      Use location data of users position.
      Country,State,City
      The default value is false
      \warning This functionality is not yet implemented
      */
    property bool useLocation: false


    /*!
      \internal
      Will be set after first request and next requests update
      */
    property string __clientId // Handled by request functions
    /*!
      \internal
      */
    property string __imei: deviceInfo.imei // Use mobility to get imei of device
    /*!
      \internal
      */
    property string __screenWidth: screen ? screen.displayWidth : ""
    /*!
      \internal
      */
    property string __screenHeight: screen ? screen.displayHeight : ""
    /*!
      \internal
      GPS position in format <latitude>,<longitude>
      */
    property string __gpsLoc: ""
    /*!
      \internal
      Location in readable format country,state/province,city (US,NY,NY)
      */
    property string __location: useLocation ? ""
                                            : ""
}
