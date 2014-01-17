#pragma managed

#include "stdafx.h"
#include <iostream>
#include <fstream>



using namespace System;
using namespace std;
using namespace System::Runtime::InteropServices;
using namespace Interop::QBXMLRP2;

// Definitions
Dart_NativeFunction ResolveName(Dart_Handle name, int argc, bool* auto_setup_scope);

// Entry point

DART_EXPORT Dart_Handle QBXMLRP2_DART_Init(Dart_Handle parent_library) {
	if (Dart_IsError(parent_library)) { return parent_library; }

	Dart_Handle result_code = Dart_SetNativeResolver(parent_library, ResolveName);
	if (Dart_IsError(result_code)) return result_code;

	return Dart_Null();
}

Dart_Handle HandleError(Dart_Handle handle) {
	if (Dart_IsError(handle)) Dart_PropagateError(handle);
	return handle;
}

enum MethodNames {
	error,
	closeConnection,
	openConnection,
	beginSession,
	endSession,
	processRequest,
	processSubscription,
	getQBLastError,
	getCurrentCompanyFileName,
	getAuthPreferences,
	getConnectionType,
	getMinorVersion,
	getMajorVersion,
	getVersionsForSession,
	getVersionsForSubscription,
	getReleaseLevel,
	getReleaseNumber
};

int getMethodFromString (String^ str) {
	if (str == "closeConnection") return MethodNames::closeConnection;
	if (str == "openConnection") return MethodNames::openConnection;
	if (str == "beginSession") return MethodNames::beginSession;
	if (str == "endSession") return MethodNames::endSession;
	if (str == "processRequest") return MethodNames::processRequest;
	if (str == "processSubscription") return MethodNames::processSubscription;
	if (str == "getQBLastError") return MethodNames::getQBLastError;
	if (str == "getCurrentCompanyFileName") return MethodNames::getCurrentCompanyFileName;
	// NOT YET IMPLEMENTED:
	if (str == "getAuthPreferences") return MethodNames::getAuthPreferences;
	if (str == "getConnectionType") return MethodNames::getConnectionType;
	if (str == "getMinorVersion") return MethodNames::getMinorVersion;
	if (str == "getMajorVersion") return MethodNames::getMajorVersion;
	if (str == "getVersionsForSession") return MethodNames::getVersionsForSession;
	if (str == "getVersionsForSubscription") return MethodNames::getVersionsForSubscription;
	if (str == "getReleaseLevel") return MethodNames::getReleaseLevel;
	if (str == "getReleaseNumber") return MethodNames::getReleaseNumber;
	return MethodNames::error;
}

void SendResponse (Dart_Port port,Dart_CObject response) {
  Dart_PostCObject(port, &response);
}

void SendBool (Dart_Port port, bool response) {
	Dart_CObject boolresp;
	boolresp.type = Dart_CObject_kBool;
	boolresp.value.as_bool = response;
	SendResponse(port, boolresp);
}

void log (String ^str) {
	// Cant be bothered to learn how to do this properly
	if (str->Length < 250) {
		ofstream fileStream;
		fileStream.open("log.txt",ios::app);
		IntPtr ptr = Marshal::StringToHGlobalAnsi(str);
		char* str2 = reinterpret_cast<char*>(static_cast<void*>(ptr));
		char buffer[256];
		strcat(buffer,str2);
		strcat(buffer,"\r\n");
		fileStream << buffer;
		Marshal::FreeHGlobal(ptr);
		fileStream.close();
	}
}

void SendString (Dart_Port port, String ^response) {
	Dart_CObject strresp;
	strresp.type = Dart_CObject_kString;

	IntPtr ptr = Marshal::StringToHGlobalAnsi(response);
	char* str = reinterpret_cast<char*>(static_cast<void*>(ptr));
	strresp.value.as_string = str;
	SendResponse(port, strresp);
	
	Marshal::FreeHGlobal(ptr);

}

ref class RequestPointer {
   public:
	static Interop::QBXMLRP2::IRequestProcessor5 ^rqPtr = gcnew Interop::QBXMLRP2::RequestProcessor3; 
};
void QBXMLMessageHandler (Dart_Port dest_port_id, Dart_CObject* message) {
	Dart_Port replyPortID = ILLEGAL_PORT;
	if (message->type == Dart_CObject_kArray && message->value.as_array.length == 3) {
		Dart_CObject* replyPort = message->value.as_array.values[0];
		Dart_CObject* method = message->value.as_array.values[1];
		Dart_CObject* params = message->value.as_array.values[2];
		if (replyPort->type == Dart_CObject_kSendPort && method->type == Dart_CObject_kString && params->type == Dart_CObject_kArray) {
			replyPortID = replyPort->value.as_send_port;
			String ^methodStr = gcnew String(method->value.as_string);
			switch (getMethodFromString(methodStr)) {
				case closeConnection:{
					RequestPointer::rqPtr->CloseConnection();
					SendBool(replyPortID, true);
					return;
									 }
					break;
				case openConnection:{
					if (params->value.as_array.length == 2) {
						Dart_CObject* appID = params->value.as_array.values[0];
						Dart_CObject* appName = params->value.as_array.values[0];
						if (appID->type == Dart_CObject_kString && appName->type == Dart_CObject_kString) {
							RequestPointer::rqPtr->OpenConnection(gcnew String(appID->value.as_string), gcnew String(appName->value.as_string));
							SendBool(replyPortID, true);
							return;
						}
					}
									}
					break;
				case beginSession:{
					if (params->value.as_array.length == 2) {
						Dart_CObject* fileName = params->value.as_array.values[0];
						Dart_CObject* fileMode = params->value.as_array.values[1];
						if (fileName->type == Dart_CObject_kString && fileMode->type == Dart_CObject_kInt32) {
						    QBFileMode qbFM = (QBFileMode) fileMode->value.as_int32;
							String ^ticket = RequestPointer::rqPtr->BeginSession(gcnew String(fileName->value.as_string), qbFM);
							SendString(replyPortID, ticket);
							return;	
						}
					}
								  }
					break;
				case endSession:{
					if (params->value.as_array.length == 1) {
						Dart_CObject* ticket = params->value.as_array.values[0];
						if (ticket->type == Dart_CObject_kString) {
							RequestPointer::rqPtr->EndSession(gcnew String(ticket->value.as_string));
							SendBool(replyPortID, true);
							return;	
						}
					}
								}
					break;
				case processRequest:{
					if (params->value.as_array.length == 2) {
						Dart_CObject* ticket = params->value.as_array.values[0];
						Dart_CObject* xml = params->value.as_array.values[1];
						if (ticket->type == Dart_CObject_kString && xml->type == Dart_CObject_kString) {
							String ^responseXML = RequestPointer::rqPtr->ProcessRequest(gcnew String (ticket->value.as_string), gcnew String(xml->value.as_string));
							SendString(replyPortID, responseXML);
							return;	
						}
					}
									}
					break;
				case processSubscription:{
					if (params->value.as_array.length == 1) {
						Dart_CObject* subscription = params->value.as_array.values[0];
						if (subscription->type == Dart_CObject_kString) {
							String ^response = RequestPointer::rqPtr->ProcessSubscription(gcnew String(subscription->value.as_string));
							SendString(replyPortID, response);
							return;
						}
					}
										 }
					break;
				case getQBLastError:{
					String ^resp = RequestPointer::rqPtr->GetQBLastError();
					SendString(replyPortID, resp);
					return;
									}
					break;
				case getCurrentCompanyFileName:{
					if (params->value.as_array.length == 1) {
						Dart_CObject* ticket = params->value.as_array.values[0];
						if (ticket->type == Dart_CObject_kString) {
							String ^response = RequestPointer::rqPtr->GetCurrentCompanyFileName(gcnew String(ticket->value.as_string));
							SendString(replyPortID, response);
							return;
						}
					}
											   }
					break;
			}
		}
	}
	SendBool(replyPortID, false);
}


void QBXMLRP2_Service_Creator (Dart_NativeArguments arguments) {
	Dart_EnterScope();
	Dart_SetReturnValue(arguments, Dart_Null());
	Dart_Port service_port =
	Dart_NewNativePort("QBXMLRP2_Service", QBXMLMessageHandler, true);
	if (service_port != ILLEGAL_PORT) {
	Dart_Handle send_port = HandleError(Dart_NewSendPort(service_port));
	Dart_SetReturnValue(arguments, send_port);
	}
	Dart_ExitScope();
}



Dart_NativeFunction ResolveName(Dart_Handle name, int argc, bool* auto_setup_scope) {
	if (!Dart_IsString(name)) return NULL;
	Dart_NativeFunction result = NULL;
	if (auto_setup_scope == NULL) return NULL;
	*auto_setup_scope = true;
	Dart_EnterScope();
	const char* cname;
	HandleError(Dart_StringToCString(name, &cname));
	if (strcmp("QBXMLRP2", cname) == 0) {	
		result = QBXMLRP2_Service_Creator;
	}
	Dart_ExitScope();
	return result;
}



  