#ifndef ESP8266_H
#define ESP8266_H

#include "Arduino.h"
#include "SoftwareSerial.h"

class Esp8266
{
	SoftwareSerial* espSerial;
	int timeout;

	bool findReponse(String keyword);
	bool _findResponse(String keyword);
	
	public:
	
		// constructor
		Esp8266();
	
		// destructor
		~Esp8266();

		// general
		bool sendCommand(String cmd);
		bool sendCommand(String cmd, String ack);
		
		String urlEncode(String str);
};

#endif
