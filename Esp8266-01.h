#ifndef ESP8266_H
#define ESP8266_H

#include "Arduino.h"
#include "SoftwareSerial.h"

class Esp8266
{
	SoftwareSerial* espSerial;
	int timeout;

	boolean echoFind(String keyword);
	
	void sendAction();
	
	int readHumidity();
	void sendToApi(String device, String action, String result);
	
	void queryEsp(String cmd);
	
	public:
	
		// constructor
		Esp8266();
	
		// destructor
		~Esp8266();

		// general
		boolean SendCommand(String cmd, String ack);
		void readSerial();

};

#endif
