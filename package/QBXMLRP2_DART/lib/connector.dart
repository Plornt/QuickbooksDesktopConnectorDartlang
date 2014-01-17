part of QBXMLRP2_DART;

class QuickbooksConnector {
  static SendPort _port;
    
  Future<bool> openConnection(String appID, String appName) {
    return _get("openConnection", [appID, appName]);
  }
  Future<bool> closeConnection(String appID, String appName) {
    return _get("closeConnection", []);
  }
  
  Future<String> beginSession(String fileName, QBFileMode mode) {
    return _get("beginSession", [fileName, mode._n]);
  }
  
  Future<bool> endSession(String ticket) {
    return _get("endSession", [ticket]);
  }
  
  Future<String> processRequest(String ticket, String xml) {
    return _get("processRequest", [ticket, xml]);
  }
  
  // No idea what this method does but ive implemented it anyway. Might not work correctly.
  Future<String> processSubscription (String subscription) {
    return _get("processSubscription", [subscription]);
  }
  
  Future<String> getQBLastError() {
    return _get("getQBLastError", []);
  }
  
  Future<String> getCurrentCompanyFileName(String ticket) {
    return _get("getCurrentCompanyFileName", [ticket]);
  }
  
  Future<dynamic> _get(String method, List arguments) {
    var completer = new Completer();
    var replyPort = new RawReceivePort();
    var args = new List(3);
    args[0] = replyPort.sendPort;
    args[1] = method; 
    args[2] = arguments;
    _servicePort.send(args);
    replyPort.handler = (result) {
      replyPort.close();
      print("got resp");
      if (result != null && !(result is List)) {
        completer.complete(result);
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
