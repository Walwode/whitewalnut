#ifndef ESP8266_H
#define ESP8266_H

#include "Arduino.h"
#include "SoftwareSerial.h"

class Esp8266
{
	SoftwareSerial* espSerial;
	int timeout;

	bool findResponse(String keyword);
	bool _findResponse(String keyword);
	
	public:
	
		// constructor
		Esp8266(SoftwareSerial* espSerial, int baud, int timeout);
	
		// destructor
		~Esp8266();

		// general
		void sendCommand(String cmd);
		bool sendCommand(String cmd, String response);
		
		String urlEncode(String str);
};

#endif
