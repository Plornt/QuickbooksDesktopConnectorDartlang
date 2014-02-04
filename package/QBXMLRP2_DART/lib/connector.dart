part of QBXMLRP2_DART;

class QuickbooksException implements Exception {
  String message;
  String qbLastError;
  int code;
  StackTrace stackTrace;
  QuickbooksException(this.message, this.qbLastError, this.code);
  String toString () {
    return "Code: $code\nError Message: $message\nQuickbooks Last Error: $qbLastError";
  }
}


class QuickbooksConnector {
  static SendPort _port;
  String _ticket = "";
  bool _isConnected = false;
  bool _sessionBegan = false;
  get sessionBegan => _sessionBegan;
  get isConnected => _isConnected;
    
  Future<bool> openConnection(String appID, String appName) {
    return _get("openConnection", [appID, appName]).then((bool connected) { 
      Completer c = new Completer();
      if (connected) {
         _isConnected = true;
      }
      else _isConnected = false;
      c.complete(connected);
      return c.future;
    });
  }
  Future<bool> closeConnection(String appID, String appName) {
    if (_isConnected) { 
      return _get("closeConnection", []);
    }
    else throw "Not connected to quickbooks";
  }
  
  Future<String> beginSession(String fileName, QBFileMode mode) {
    if (_isConnected) { 
      return _get("beginSession", [fileName, mode._n]).then((String ticket) { 
          Completer c = new Completer();
          _ticket = ticket;
          _sessionBegan = true;
          c.complete(ticket);
          return c.future;      
        });
    }
    else throw "Not connected to quickbooks";
  }
  
  Future<bool> endSession() {
    if (_isConnected && _sessionBegan) { 
      _sessionBegan = false;
      _ticket = "";
      return _get("endSession", [_ticket]);
    }
    else throw "Not connected or no session started in quickbooks";
  }
  
  Future<String> processRequest(String xml) {
    if (_isConnected && _sessionBegan) { 
      return _get("processRequest", [_ticket, xml]);
    }
    else throw "Not connected or no session started in quickbooks";
  }
  
  // No idea what this method does but ive implemented it anyway. Might not work correctly.
  Future<String> processSubscription (String subscription) {
    if (_isConnected) { 
      return _get("processSubscription", [subscription]);
    }
    else throw "Not connected to quickbooks";
  }
  
  Future<String> getQBLastError() {
    if (_isConnected) { 
      return _get("getQBLastError", []);
    }
    else throw "Not connected to quickbooks";
  }
  
  Future<String> getCurrentCompanyFileName() {
    if (_isConnected && _sessionBegan) { 
      return _get("getCurrentCompanyFileName", [_ticket]);
    }
    else throw "Not connected or no session started in quickbooks";
  }
  
  Future<dynamic> _get(String method, List arguments) {
    Completer completer = new Completer();
    RawReceivePort replyPort = new RawReceivePort();
    List args = new List(3);
    args[0] = replyPort.sendPort;
    args[1] = method; 
    args[2] = arguments;
    _servicePort.send(args);
    replyPort.handler = (result) {
      replyPort.close();
      if (result != null && !(result is List)) {
        completer.complete(result);
      } 
      else if (result is List) {
        if (result[0] == "error") {
          completer.completeError(new QuickbooksException(result[2], result[1], result[3]));
        }
      }
      else {
        completer.completeError(new Exception("Unknown result type returned by native extension ${result}"));
      }
    };
    return completer.future;
  }

  SendPort get _servicePort {
    if (_port == null) {
      _port = _newServicePort();
    }
    return _port;
  }
//NATIVE FUNCTIONS CAN ONLY BE DECLARED IN THE SDK AND CODE THAT IS LOADED THROUGH NATIVE EXTENSIONS
  SendPort _newServicePort() native "QBXMLRP2";
}
