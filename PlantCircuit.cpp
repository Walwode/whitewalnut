#include "PlantCircuit.h"

#define CHECK_HUMIDITY_ACTION 1
#define STOP_IRRIGATION_ACTION 2

PlantCircuit::PlantCircuit() {}
PlantCircuit::PlantCircuit(String name)
{
	plantName = name;
	
	Serial.print("Created ");
	Serial.println(getName());
}
PlantCircuit::~PlantCircuit() {}

// >> general
String PlantCircuit::getName()
{
	return plantName;
}

void PlantCircuit::timerCallback(Timer* timer)
{
	// Serial.println("TimerCallback " + String(timer->getAction()) + " for " + getName());
	switch(timer->getAction()) {
		case CHECK_HUMIDITY_ACTION:
			checkHumidity();
			checkIrrigation();
			break;
		case STOP_IRRIGATION_ACTION:
			stopIrrigation();
			break;
		default:
			break;
	}
}
// << general

// >> humidity
void PlantCircuit::humiditySetup(byte vccPin, byte dataPin, long interval)
{
	humidityVccPin = vccPin;
	humidityDataPin = dataPin;

	StensTimer::getInstance()->setInterval(this, CHECK_HUMIDITY_ACTION, interval);
}

int PlantCircuit::checkHumidity()
{
	Serial.print("Check Humidity (");
	Serial.print(getName());
	Serial.print(")... ");
	
	digitalWrite(humidityVccPin, HIGH);
	delay(1000); // TODO
	humidity = 1023 - analogRead(humidityDataPin);
	digitalWrite(humidityVccPin, LOW);
	
	Serial.println(String(getHumidity()));
	if (internet) logToApi("Check Humidity", String(getHumidity()));
	
	return getHumidity();
}

int PlantCircuit::getHumidity()
{
	return humidity;
}
// << humidity

// >> irrigation
void PlantCircuit::irrigationSetup(byte vccPin, long duration, int humidityLimit, int reIrrigationTimeLock)
{
	irrigationVccPin = vccPin;
	irrigationDuration = duration;
	
	if (humidityLimit > 0)
	{
		irrigation = true;
		irrigationHumidity = humidityLimit;
		irrigationTimeLock = reIrrigationTimeLock;
	}
}

void PlantCircuit::checkIrrigation()
{
	if (irrigation && isDry() && isBeyondIrrigationLock()) startIrrigation();
}

void PlantCircuit::startIrrigation()
{
	if (!irrigating)
	{
		Serial.print("Start Irrigation (");
		Serial.print(getName());
		Serial.println(")... ");
	
		irrigating = true;
		digitalWrite(irrigationVccPin, HIGH);
		StensTimer::getInstance()->setTimer(this, STOP_IRRIGATION_ACTION, irrigationDuration);
		
		Serial.println(String(irrigationDuration));
		if (internet) logToApi("Start Irrigation", String(irrigationVccPin) + " - " + String(irrigationDuration));
	}
}

void PlantCircuit::stopIrrigation()
{
	Serial.print("Stop Irrigation (");
	Serial.print(getName());
	Serial.print(")... ");
	
	irrigating = false;
	nextIrrigationTime = millis() + irrigationTimeLock;
	digitalWrite(irrigationVccPin, LOW);
	
	Serial.println(String(irrigationTimeLock));
	if (internet) logToApi("Stop Irrigation", String(irrigationVccPin));
}

bool PlantCircuit::isDry()
{
	return (humidity < irrigationHumidity);
}

bool PlantCircuit::isBeyondIrrigationLock()
{
	return (millis() > nextIrrigationTime);
}

// << irrigation

// >> internet
void PlantCircuit::wifiSetup(Esp8266* newEsp8266)
{
	esp8266 = newEsp8266;
	
	if (esp8266->sendCommand("AT", "OK"))
	{
		Serial.print("Connected to ESP8266 (");
		Serial.print(getName());
		Serial.println(")...");
		
		internet = true;
	} else
	{
		Serial.print("Error with ESP8266 Connection (");
		Serial.print(getName());
		Serial.println(")...");
	}
}

void PlantCircuit::logToApi(String action, String result)
{
	// TODO: avoid reset, avoid new AP registration
	// TODO: pins to start esp8266 for power saving
	
	if (!esp8266->sendCommand("AT+RST", "OK")) { Serial.println("ESP8266 Reset Error"); return; }
	if (!esp8266->sendCommand("AT+CWMODE=1", "OK")) { Serial.println("ESP8266 CWMode Error"); return; }
	if (!esp8266->sendCommand("AT+CWJAP=\"WLAN-9PGVPK\",\"6763142139087153\"", "IP")) { Serial.println("ESP8266 WiFi Credential Error"); return; }
	
	// esp8266->SendCommand("AT+CIPCLOSE");
	if (!esp8266->sendCommand("AT+CIPSTART=\"TCP\",\"api.walterheger.de\",80", "OK")) { Serial.println("ESP8266 TCP Connection Error"); return; }

	String url;
	url += "/whitewalnut/entry.php?";
	url += "device=" + esp8266->urlEncode(getName());
	url += "&action=" + esp8266->urlEncode(action);
	url += "&result=" + esp8266->urlEncode(result);

	String cmd;
	cmd += "PUT " + url + " HTTP/1.1\r\n";
	cmd += "Host: api.walterheger.de\r\n";
	cmd += "Connection: close";
	// cmd += "Connection: keep-alive";
	
	if (!esp8266->sendCommand("AT+CIPSEND=" + String(cmd.length() + 4), ">")) { Serial.println("ESP8266 Send State Error"); return; }
	if (!esp8266->sendCommand(cmd + "\r\n", "successfully")) { Serial.println("ESP8266 Command Error"); return; }

	// esp8266->sendCommand("AT+CIPCLOSE");
}
// << internet
