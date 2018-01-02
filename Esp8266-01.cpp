#include "Esp8266.h"

#define SerialRxPin 3
#define SerialTxPin 2
#define SerialBaud 9600

Esp8266::Esp8266()
{
	espSerial = new SoftwareSerial(SerialRxPin, SerialTxPin);
	espSerial->begin(SerialBaud);
	
	this->timeout = timeout;
	
	Serial.println("Created ESP8266");
}

Esp8266::~Esp8266() {}

// >> general
boolean Esp8266::SendCommand(String cmd, String ack)
{
	Serial.println("Sending command to ESP-01: " + cmd);
	espSerial->println(cmd);
	
	if (ack != "") { return(echoFind(ack)) };
	else return true;
}

void Esp8266::SendCommand(String cmd)
{
	SendCommand(cmd, "");
}

boolean ESP01::echoFind(String keyword)
{
	byte currentChar = 0;
	byte keywordLength = keyword.length();
	long deadline = millis() + timeout;
	
	while (millis() < deadline)
	{
		if (espSerial->available())
		{
			if (espSerial->find("OK")) // TODO
			{
				Serial.println("Keyword " + keyword + " found");
				return true;
			}
		}
	/*
		if (espSerial->available())
		{
			char ch = espSerial->read();
			
			Serial.write(ch);
			if (ch == keyword[current_char])
				if (++current_char == keyword_length) {
					Serial.println();
					return true;
				}
		}
	 */
	}
	return false; // Timed out
}

void ESP01::readSerial()
{
	if (espSerial->available()) {
		Serial.write(espSerial->read());
	}
}

void ESP01::sendAction()
{
	sendToApi("Plant%201", "ReadHumidity", String(readHumidity()));
}

int ESP01::readHumidity()
{
	Serial.println("Reading humidity...");
	digitalWrite(6, HIGH);
	delay(1000);
	int value = analogRead(A1);
	delay(100);
	digitalWrite(6, LOW);
	return 1023 - value;
}

void ESP01::sendToApi(String device, String action, String result)
{
	digitalWrite(13, HIGH);

	queryEsp("AT+RST");
	if (espSerial->find("OK")) { Serial.println("ESP Reset"); }
	else { Serial.println("RESET Error"); return; }
	
	queryEsp("AT+CWMODE=1");
	if (espSerial->find("OK")) { Serial.println("CWMode changed"); }
	else { Serial.println("CWMODE Error"); return; }
	
	queryEsp("AT+CWJAP=\"WLAN-9PGVPK\",\"6763142139087153\"");
	if (espSerial->find("IP")) { Serial.println("Got IP"); }
	else { Serial.println("WiFi Connection Error"); return; }
	
	// queryEsp("AT+CIPCLOSE");
	
	queryEsp("AT+CIPSTART=\"TCP\",\"api.walterheger.de\",80");
	if (espSerial->find("OK")) { Serial.println("TCP connection stablished"); }
	else { Serial.println("Error when connecting"); return; }

	// String url = "/";
	String url = "/whitewalnut/entry.php?device=" + device + "&action=" + action + "&result=" + result;

	String cmd;
	cmd += "PUT " + url + " HTTP/1.1\r\n";
	cmd += "Host: api.walterheger.de\r\n";
	cmd += "Connection: keep-alive";
	// cmd += "Connection: close";
	
	queryEsp("AT+CIPSEND=" + String(cmd.length()+4));
	if (espSerial->find(">")) { Serial.println("Starting request"); }
	else { Serial.println("Not in sending state"); return; }

	queryEsp(cmd + "\r\n");
	if (espSerial->find("successfully")) { Serial.println("Record created"); }
	else if (espSerial->find("Bad request")) { Serial.println("Bad request"); return; }
	else { Serial.println("Unknown Command error"); return; }
	
	queryEsp("AT+CIPCLOSE");
	if (espSerial->find("OK")) { Serial.println("Connection closed"); }
	else { Serial.println("Unknown close error"); return; }

	Serial.println("Done");
	digitalWrite(13, LOW);
}

void ESP01::queryEsp(String cmd)
{
	Serial.println("ESP Request: " + cmd);
	espSerial->println(cmd);
}

// << general

