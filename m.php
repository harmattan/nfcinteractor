<?php
/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Andreas Jakl (andreas.jakl@nokia.com)
**
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

/*
   Nfc Geo Tags Redirection Script
   v1.0.0

   Summary
   ---------------------------------------------------------------------------
   Generate platform-independent NFC Geo tags.
   
   
   Use case
   ---------------------------------------------------------------------------
   Direct the user to a certain point on a map. The NFC tag
   contains location information (point of interest, POI).
   The user touches the tag with his NFC phone. The phone opens the maps
   application at the coordinates of the POI. This allows the user to
   navigate to the point or to get more information.
   
   
   Description
   ---------------------------------------------------------------------------
   Some phones support the "geo:" URI scheme and connect that to a maps
   app on the phone (e.g., the Nokia N9 or Android).
   Other phones do not support this URI scheme, and should be redirected
   to a web-based maps client instead (e.g., Symbian). On Symbian, this
   method also triggers the Nokia Maps application.
   
   To maximize compatibility of a tag to phones, you need to take the phone
   OS into account and use the supported URL scheme. However, it is still 
   desirable to have only a single NFC tag, regardless of the phone OS.
   
   The solution is to write a URL to this script to the tag. The script
   detects the phone OS through its user agent, and will redirect it to
   the appropriate URL. This allows having a single tag, which is compatible
   to multiple mobile operating systems.
   
   
   Usage
   ---------------------------------------------------------------------------
   Upload m.php to PHP 4.x+ compatible web server.
   
   Call with location information as parameter:
   
       m.php?l=[latitude],[longitude]
   
   Example:
       http://www.nfcinteractor.com/m.php?l=60.17,24.829
   
   Alternatively, you can configure a custom place name in 
   the [CUSTOM PLACE NAMES] section. Then, you can store the 
   coordinates in the script and don't have to pass them via parameters:
   
       m.php?c=[custom place name]
   
   Example:
       http://www.nfcinteractor.com/m.php?c=nokia
   
   
   Advanced Usage
   ---------------------------------------------------------------------------
   
   If your Apache-based web server supports URL rewrites using the 
   mod_rewrite module, you can allow calling the script without adding 
   the ".php" to the URL, thus saving four bytes on the tag.
   
   Example:
       http://www.nfcinteractor.com/m?l=60.17,24.829
	   
   To enable this, add the following .htaccess file in the same directory where
   you place the m.php script:
   
       <IfModule mod_rewrite.c>
       RewriteEngine On
       RewriteBase /
       RewriteRule ^m$ m.php
       </IfModule>
   
 */

// Encoded platform names, in order or priority
// c = Custom name, app id stored in PHP code
// n = Nokia Store
// s = Symbian
// h = MeeGo Harmattan
// f = Series 40
// w = Windows Phone
// a = Android
// i = iPhone
// b = RIM BlackBerry
$allplatforms = array('n', 's', 'h', 'f', 'w', 'a', 'i', 'b');
// Stores 1-letter, encoded platform name
$ua_enc = '';
// Check user agent and assign 1-letter, encoded platform name
$user_agent = $_SERVER['HTTP_USER_AGENT'];
if (preg_match('/symbian/i',$user_agent)) {
	$ua_enc = 's';
} elseif (preg_match('/meego/i',$user_agent)) {
	$ua_enc = 'h';
} elseif (preg_match('/nokia/i',$user_agent)) {
	$ua_enc = 'f';
} elseif (preg_match('/windows phone/i',$user_agent)) {
	$ua_enc = 'w';
} elseif (preg_match('/android/i',$user_agent)) {
	$ua_enc = 'a';
} elseif (preg_match('/iphone/i',$user_agent) || preg_match('/ipad/i',$user_agent)) {
	$ua_enc = 'i';
} elseif (preg_match('/blackberry/i',$user_agent)) {
	$ua_enc = 'b';
}

// Default location: Nokia House, Espoo, Finland
$location = "60.17,24.829";
// Check if any coordinates are set in the URL
if (isset($_GET['l']) || isset($_GET['c'])) {
	$lparam = isset($_GET['l']) ? $_GET['l'] : $_GET['c'];
	// Encode a + as %2B in the URL parameter if needed
	if (preg_match('/^[\-+]?\d+\.?\d+[,]{1}[\-+]?\d+\.?\d+$/', $lparam) == 1) {
		// Use provided coordinates
		$location = $lparam;
	} else {
		$locationString = strtolower($lparam);
		// [CUSTOM PLACE NAMES]
		// Provide custom place names here.
		// Allows calling the script by just using a textual place name
		// instead of coordinates, which allows for greater flexibility.
		// The script then replaces the place name with the coordinates.
		switch ($locationString) {
		case "nokia":
			$location = "60.17,24.829";
			break;
		case "nokiaworld":
			// Excel, Nokia World, London, UK
			$location = "51.508489,0.027337";
			break;
		case "qtdevdays":
			// Qt Dev Days, Dolce Munich, Germany
			$location = "48.2849983,11.5620476";
			// Qt Dev Days, Hyatt Regency San Francisco Airport
			//$location = "37.594231,-122.365317";
			break;
		}
	}
}
if ($ua_enc == 'h' || $ua_enc == 'a') {
	// Use the "geo:" URI scheme for MeeGo Harmattan and Android
	// The phones support direct redirection by sending a new header.
	header("Location: geo:".$location);
	exit;
}
// For other phones (including Symbian), redirect the phone via a JavaScript
// redirect (using the header redirection on Symbian would not trigger opening the
// Nokia Maps client).
?>
<html>
<head>
<script type="text/javascript">
<!--
window.location = "http://m.ovi.me/?c=<?php echo $location; ?>";
//-->
</script>
</head>
<body>
<noscript>
<font face="Tahoma,Arial"><a href="http://m.ovi.me/?c=<?php echo $location; ?>">Open Nokia Maps</a></font>
</noscript>
</body>
</html>