#include "Esp8266-01.h"

// #define ESP8266_DEBUG_RESPONSE
// #define ESP8266_DEBUG_COMMAND

#define ESP8266_RX_PIN 3
#define ESP8266_TX_PIN 2
#define ESP8266_BAUD 9600

Esp8266::Esp8266()
{
	espSerial = new SoftwareSerial(ESP8266_RX_PIN, ESP8266_TX_PIN); // TODO
	espSerial->begin(ESP8266_BAUD);
	
	timeout = 5000; // TODO
	
	Serial.println(F("Created ESP8266 ESP-01"));
}

Esp8266::~Esp8266() {}

void Esp8266::sendCommand(String cmd)
{
	sendCommand(cmd, "");
}

bool Esp8266::sendCommand(String cmd, String response)
{
	while (espSerial->available()) { espSerial->read(); }

	#ifdef ESP8266_DEBUG_COMMAND
	Serial.println(F("-------------------------"));
	Serial.println(F(">> ESP8266 command start"));
	Serial.println(cmd);
	Serial.println(F("<< ESP8266 command end"));
	#endif
	
	espSerial->println(cmd);

	if (response != "") { return(findResponse(response)); }
	else { return true; }
}

String Esp8266::urlEncode(String str)
{
	str.replace(" ", "%20");
	return str;
}

// >> private
bool Esp8266::findResponse(String keyword)
{
	#ifdef ESP8266_DEBUG_RESPONSE
	Serial.print(F("ESP8266 response keyword: "));
	Serial.println(keyword);
	
	Serial.println(F(">> ESP8266 response start"));
	#endif
	
	bool keywordFound = _findResponse(keyword);
	
	#ifdef ESP8266_DEBUG_RESPONSE
	Serial.println(F("<< ESP8266 response end"));
	
	if (keywordFound) Serial.println(F("ESP8266 response keyword found"));
	else Serial.println(F("ESP8266 response keyword was not found"));
	#endif
	
	return keywordFound;
}

bool Esp8266::_findResponse(String keyword)
{
	bool keywordFound = false;
	
	byte keywordPosition = 0;
	byte keywordLength = keyword.length();
	char currentSerialChar;
	
	long deadline = millis() + timeout;
	int noSerialCounter = 0;
	
	while (millis() < deadline)
	{
		if (espSerial->available())
		{
			currentSerialChar = espSerial->read();
			
			#ifdef ESP8266_DEBUG_RESPONSE
			Serial.print(currentSerialChar);
			#endif
			
			if (currentSerialChar == keyword[keywordPosition]) { if (++keywordPosition == keywordLength) keywordFound = true; }
			else { keywordPosition = 0; }
			
			// if (keywordFound) return true; // will result in quicker response, but serial might be longer
		} else
		{
			if (++noSerialCounter > 15) { return keywordFound; }
			delay(200);
		}
	}
	Serial.println(F("ESP8266 response timeout"));
	return keywordFound;
}

/*
bool Esp8266::_findResponse(String keyword)
{
	char ch[50];
	keyword.toCharArray(ch, 50); // TODO buffer size
	if (espSerial->find(ch)) return true;
	return false;
}
*/

// << private
