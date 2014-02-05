Quickbooks Desktop Connector Dartlang
=====================================

This DLL and Native Extension file for Dart enables QBXML requests to be built within a dart:io application and sent directly to Quickbooks. 


It essentially wraps the methods in the QBXMLRP2 Interop DLL.

I suggest for exact usage you consult the QBXMLRP2 documentation as it is the same parameters and responses.


Install
=======

Download repository as zip or clone it, move the package folder to somewhere that makes sense.

Add the following to your pubspec

```yaml
    QBXMLRP2_DART: 
        path: '<PATH_TO_PACKAGE_DIRECTORY>\QBXMLRP2_DART'
```

Quick Example:
==============

```dart
import 'package:QBXMLRP2_DART/QBXMLRP2_DART.dart';

void main () {
  QuickbooksConnector qbc = new QuickbooksConnector();
  String appID = "My Test Application";
  String appName = "QBXML Test App";
  
  // Opens a connection
  qbc.openConnection(appID, appName).then((bool connected) {
    if (connected) {
      String companyFileName = ""; // Empty string specifies current open file.
      
      // QBFileModes: doNotCare, multiUser, singleUser
      // Begins a session for the specified file name and mode. Quickbooks *will* prompt for authorization
      return qbc.beginSession(companyFileName, QBFileMode.doNotCare);
    }
  }).then((String ticketID) {
    // Lets check what the current company file name is... 
    return qbc.getCurrentCompanyFileName();
  }).then((String fileName) { 
    print("Connected to company $fileName - Sending XML...");
    
    // Sends the XML to quickbooks
    return qbc.processRequest("XML HERE");
  })
  .then((String responseXML) {
    //Everything worked correctly!
    print("Got response: $responseXML");

    qbc.endSession(ticketID);
    qbc.closeConnection();
  })
  .catchError((QuickbooksException error) { 
    // Error exception has the following fields:
    // error.code = int representing the thrown error
    // error.message = String representation of the error
    // error.qbLastError = The last error given by quickbooks - usually more detailed and helpful for debugging
    throw error;
  });
}
```

TODO
====
 
- Non-Async methods
- Documentation
