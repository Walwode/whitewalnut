#include "PlantCircuit.h"

#define CHECK_HUMIDITY_ACTION 1
#define STOP_IRRIGATION_ACTION 2
#define CHECK_LIGHT_SWITCH 3
#define CHECK_INTERNET_RECONNECT 4

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
	switch(timer->getAction())
	{
		case CHECK_HUMIDITY_ACTION:
			checkHumidity();
			checkIrrigation();
			break;
		case STOP_IRRIGATION_ACTION:
			stopIrrigation();
			break;
		case CHECK_LIGHT_SWITCH:
			checkBrightness();
			break;
		case CHECK_INTERNET_RECONNECT:
			checkEspConnection();
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

	pinMode(vccPin, OUTPUT);
	digitalWrite(vccPin, LOW);

	StensTimer::getInstance()->setInterval(this, CHECK_HUMIDITY_ACTION, interval);
}

int PlantCircuit::checkHumidity()
{
	Serial.print(F("Check Humidity ("));
	Serial.print(getName());
	Serial.print(F(")... "));
	
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
void PlantCircuit::irrigationSetup(byte vccPin, long duration)
{
	irrigationSetup(vccPin, duration, 0, 0);
}

void PlantCircuit::irrigationSetup(byte vccPin, long duration, int humidityThreshold, long reIrrigationTimeLock)
{
	irrigationVccPin = vccPin;
	irrigationDuration = duration;
	
	if (humidityThreshold > 0)
	{
		irrigation = true;
		irrigationHumidity = humidityThreshold;
		irrigationTimeLock = reIrrigationTimeLock;
	}
}

bool PlantCircuit::checkIrrigation()
{
	if (irrigation && isDry() && isBeyondIrrigationLock()) startIrrigation();
	return(irrigating);
}

void PlantCircuit::startIrrigation()
{
	if (!irrigating)
	{
		Serial.print(F("Start Irrigation ("));
		Serial.print(getName());
		Serial.print(F(")... "));
	
		irrigating = true;
		digitalWrite(irrigationVccPin, HIGH);
		StensTimer::getInstance()->setTimer(this, STOP_IRRIGATION_ACTION, irrigationDuration);
		
		Serial.println(String(irrigationDuration));
		if (internet) logToApi("Start Irrigation", String(irrigationDuration));
	}
}

void PlantCircuit::stopIrrigation()
{
	Serial.print(F("Stop Irrigation ("));
	Serial.print(getName());
	Serial.print(F(")... "));
	
	irrigating = false;
	nextIrrigationTime = millis() + irrigationTimeLock;
	digitalWrite(irrigationVccPin, LOW);
	
	Serial.println(String(irrigationTimeLock));
	if (internet) logToApi("Stop Irrigation", String(irrigationVccPin));
}

bool PlantCircuit::isDry()
{
	return (humidity <= irrigationHumidity);
}

bool PlantCircuit::isBeyondIrrigationLock()
{
	return (millis() > nextIrrigationTime);
}

// << irrigation

// >> lamp
void PlantCircuit::lightSetup(byte vccPin, byte dataPin, long interval, int threshold)
{
	lightVccPin = vccPin;
	photoDataPin = dataPin;
	lightThreshold = threshold;
	
	pinMode(lightVccPin, OUTPUT);
	digitalWrite(lightVccPin, LOW);
	
	StensTimer::getInstance()->setInterval(this, CHECK_LIGHT_SWITCH, interval);
}

void PlantCircuit::checkBrightness()
{
	Serial.print(F("Check Brightness ("));
	Serial.print(getName());
	Serial.print(F(")... "));
	
	int brightness = analogRead(photoDataPin);
	
	Serial.println(String(brightness));
 
	if (brightness < lightThreshold) switchLightOn();
	else switchLightOff();
}

void PlantCircuit::switchLightOn()
{
	Serial.print(F("Switch Light On ("));
	Serial.print(getName());
	Serial.println(F(")..."));
	
	digitalWrite(lightVccPin, HIGH);
}

void PlantCircuit::switchLightOff()
{
	Serial.print(F("Switch Light Off ("));
	Serial.print(getName());
	Serial.println(F(")..."));
	
	digitalWrite(lightVccPin, LOW);
}
// << lamp

// >> internet
void PlantCircuit::wifiSetup(Esp8266* instEsp8266)
{
	esp8266 = instEsp8266;
	checkEspConnection();
}

bool PlantCircuit::checkEspConnection()
{
	if (!esp8266) return;
	
	if (esp8266->sendCommand("AT", "OK"))
	{
		Serial.print(F("Connected to ESP8266 ("));
		Serial.print(getName());
		Serial.println(F(")..."));
		
		internet = true;
	} else
	{
		Serial.print(F("Error with ESP8266 Connection ("));
		Serial.print(getName());
		Serial.println(F(")..."));
		
		StensTimer::getInstance()->setTimer(this, CHECK_INTERNET_RECONNECT, 60000);
	}
	return internet;
}

void PlantCircuit::logToApi(String action, String result)
{
	// TODO: avoid reset, avoid new AP registration
	// TODO: pins to start esp8266 for power saving

	if (!esp8266->sendCommand("AT+CWJAP?","+CWJAP"))
	{
		if (!esp8266->sendCommand("AT+RST", "OK")) { Serial.println(F("ESP8266 Reset Error")); return; }
		if (!esp8266->sendCommand("AT+CWMODE=1", "OK")) { Serial.println(F("ESP8266 CWMode Error")); return; }
		if (!esp8266->sendCommand("AT+CWJAP=\"WLAN-9PGVPK\",\"6763142139087153\"", "IP")) { Serial.println(F("ESP8266 WiFi Credential Error")); return; }
	}
	
	// esp8266->SendCommand("AT+CIPCLOSE");
	if (!esp8266->sendCommand("AT+CIPSTART=\"TCP\",\"api.walterheger.de\",80", "OK")) { Serial.println(F("ESP8266 TCP Connection Error")); return; }

	String url = "";
	url += "/whitewalnut/entry.php?";
	url += "device=" + esp8266->urlEncode(getName());
	url += "&action=" + esp8266->urlEncode(action);
	url += "&result=" + esp8266->urlEncode(result);

	String cmd = "";
	cmd += "PUT " + url + " HTTP/1.1\r\n";
	cmd += "Host: api.walterheger.de\r\n";
	cmd += "Connection: close";
	// cmd += "Connection: keep-alive";
	
	if (!esp8266->sendCommand("AT+CIPSEND=" + String(cmd.length() + 4), ">")) { Serial.println(F("ESP8266 Send State Error")); return; }
	if (!esp8266->sendCommand(cmd + "\r\n", "successfully")) { Serial.println(F("ESP8266 Command Error")); return; }

	// esp8266->sendCommand("AT+CIPCLOSE");
 
	Serial.print(F("Uploaded "));
	Serial.print(action);
	Serial.print(F(" to API ("));
	Serial.print(getName());
	Serial.println(F(")..."));
}
// << internet
