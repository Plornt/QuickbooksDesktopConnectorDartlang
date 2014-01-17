Quickbooks Desktop Connector Dartlang
=====================================

This DLL and Native Extension file for Dart enables QBXML requests to be built within a dart:io application and sent directly to Quickbooks. 


It essentially wraps the methods in the QBXMLRP2 Interop DLL.

I suggest for exact usage you consult the QBXMLRP2 documentation as it is the same parameters and responses.


Quick Example:
==============


    import 'QBXMLRP2_DART/QBXMLRP2_DART.dart';
    
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
          qbc.beginSession(companyFileName, QBFileMode.doNotCare).then((String ticketID) {
            qbc.getCurrentCompanyFileName(ticketID).then((String fileName) { 
              print("Connected to company $fileName - Sending XML...");
              
              // Sends the XML to quickbooks
              qbc.processRequest(ticketID, "XML HERE").then((String responseXML) {
                print("Got response: $responseXML");
                
                // Ends the session
                qbc.endSession(ticketID);
              });
            });
          });
        }
      });
    }


TODO
====
 
- Proper error handling (Currently does not send any data back to dart when an error has occured.)
- Object oriented session handling (ie not needing a ticket every request)
- Non-Async methods
- Documentation
