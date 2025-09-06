#include <Wire.h>
#include <SoftwareSerial.h>
#include "Adafruit_SHT31.h"

SoftwareSerial WebcamSerial(6, 7); // RX, TX
Adafruit_SHT31 sht31 = Adafruit_SHT31();

const unsigned long HEARTBEAT_TIMEOUT = 600000; // 10 minuti in millisecondi
unsigned long lastHeartbeatTime = 0;
String inputString = "";

void setup() {
	Wire.begin();
	Serial.begin(9600);
	WebcamSerial.begin(9600);
	sht31.begin(0x44);
	lastHeartbeatTime = millis(); // Inizializza il timer all'avvio
	pinMode(2, OUTPUT); // sets pin 2 as output for Camera Power
	pinMode(3, OUTPUT); // sets pin 3 as output for Heating
	pinMode(4, OUTPUT); // sets pin 4 as output for PC
	pinMode(5, OUTPUT); // sets pin 5 as output for Fan
	
	delay(1000); // let serial console settle
}


void loop() {
	
	// Controllo del timeout dell'heartbeat
	// Se non riceve un segnale di Heartbeat da 10 minuti resettta in PC
	if (millis() - lastHeartbeatTime > HEARTBEAT_TIMEOUT) {
		resetFunc();
	}	
	
	// Read and execute commands from serial port
	if (WebcamSerial.available()) {
		String command = WebcamSerial.readString();
	
	if (command == "b") { // Heartbeat signal
			lastHeartbeatTime = millis();
	} else if (command == "C") {  // turn on Camera
			digitalWrite(2, LOW);
	} else if (command == "c") {  // turn off Camera
			digitalWrite(2, HIGH);
	} else if (command == "H") {  // turn on Heating
			digitalWrite(3, HIGH);
	} else if (command == "h") {  // turn off Heating
			digitalWrite(3, LOW);
	} else if (command == "P") {  // turn on PC
			digitalWrite(4, LOW);
	} else if (command == "p") {  // turn off PC
			digitalWrite(4, HIGH);
	} else if (command == "F") {  // turn on Fan
			digitalWrite(5, HIGH);
	} else if (command == "f") {  // turn off Fan
			digitalWrite(5, LOW);
	}  else if (command == "n") {  // Test Software Serial
			WebcamSerial.print("Software Serial OK");
	} else if (command == "t") {  // Test Software Serial
			float t_celsius = sht31.readTemperature();
			Serial.println(t_celsius, 1); // Stampa Celsius con 1 cifra decimale
	} else if (command == "u") {  // Test Software Serial
			float h_percent = sht31.readHumidity();
			Serial.println(h_percent, 1); // Stampa Umidita' con 1 cifra decimale
	} 
	}

	// Read and execute commands from serial port
	if (Serial.available()) {
		String command = Serial.readString();
	
	if (command == "b") { // Heartbeat signal
			lastHeartbeatTime = millis();
	} else if (command == "C") {  // turn on Camera
			digitalWrite(2, LOW);
	} else if (command == "c") {  // turn off Camera
			digitalWrite(2, HIGH);
	} else if (command == "H") {  // turn on Heating
			digitalWrite(3, HIGH);
	} else if (command == "h") {  // turn off Heating
			digitalWrite(3, LOW);
	} else if (command == "P") {  // turn on PC
			digitalWrite(4, LOW);
	} else if (command == "p") {  // turn off PC
			digitalWrite(4, HIGH);
	} else if (command == "F") {  // turn on Fan
			digitalWrite(5, HIGH);
	} else if (command == "f") {  // turn off Fan
			digitalWrite(5, LOW);
	} else if (command == "n") {  // Test Software Serial
			Serial.print("Serial OK");
	} else if (command == "t") {  // Test Software Serial
			float t_celsius = sht31.readTemperature();
			Serial.println(t_celsius, 1); // Stampa Celsius con 1 cifra decimale
	} else if (command == "u") {  // Test Software Serial
			float h_percent = sht31.readHumidity();
			Serial.println(h_percent, 1); // Stampa Umidita' con 1 cifra decimale
	} 
	}

}

void resetFunc() {
			digitalWrite(4, HIGH);
			lastHeartbeatTime = millis();
			delay(1000);
			digitalWrite(4, LOW);
}
