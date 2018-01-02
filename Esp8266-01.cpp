#include "Esp8266.h"

#define ESP_DEBUG_RESPONSE

#define SERIAL_RX_PIN 3
#define SERIAL_TX_PIN 2
#define SERIAL_BAUD 9600

Esp8266::Esp8266()
{
	espSerial = new SoftwareSerial(SERIAL_RX_PIN, SERIAL_TX_PIN); // TODO
	espSerial->begin(SERIAL_BAUD);
	
	this->timeout = 5000; // TODO
	
	Serial.println("Created ESP8266 ESP-01");
}

Esp8266::~Esp8266() {}

void Esp8266::sendCommand(String cmd)
{
	sendCommand(cmd, "");
}

bool Esp8266::sendCommand(String cmd, String response)
{
	while (espSerial->available()) { espSerial->read(); }
	
	Serial.println(">> ESP8266 command start")
	Serial.println(cmd);
	Serial.println(">> ESP8266 command end");
	espSerial->println(cmd);
	
	if (response != "") { return(findReponse(response)) };
	else { return true; }
}

String Esp8266::urlEncode(String str)
{
	str.replace(" ", "%20");
	return str;
}

// >> private
bool Esp8266::findReponse(String keyword)
{
	#ifdef ESP_DEBUG_RESPONSE
	Serial.print("ESP8266 response keyword: ");
	Serial.println(keyword);
	
	Serial.println(">> ESP8266 response start");
	#endif
	
	bool keywordFound = _findReponse(keyword);
	
	#ifdef ESP_DEBUG_RESPONSE
	Serial.println("<< ESP8266 response end");
	
	if (keywordFound) Serial.println("ESP8266 response keyword found");
	else Serial.println("ESP8266 response keyword was not found");
	#endif
	
	return keywordFound;
}

bool Esp8266::_findReponse(String keyword)
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
			
			#ifdef ESP_DEBUG_RESPONSE
			Serial.print(currentSerialChar);
			#endif
			
			if (currentSerialChar == keyword[keywordPosition]) { if (++keywordPosition == keywordLength) keywordFound = true; }
			else { keywordPosition = 0; }
			
			// if (keywordFound) return true; // will result in quicker response, but serial might be longer
		} else
		{
			if (++noSerialCounter > 25) { return keywordFound; }
			delay(20);
		}
	}
	Serial.println("ESP8266 response timeout");
	return keywordFound;
}
// << private