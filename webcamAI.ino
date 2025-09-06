#include <Wire.h>
#include <SoftwareSerial.h>
#include "Adafruit_SHT31.h"

SoftwareSerial WebcamSerial(6, 7); // RX, TX
Adafruit_SHT31 sht31 = Adafruit_SHT31();

const unsigned long HEARTBEAT_TIMEOUT = 600000; // 10 minuti in millisecondi
unsigned long lastHeartbeatTime = 0;

void setup() {
	Wire.begin();
	Serial.begin(9600);
	WebcamSerial.begin(9600);
	
	// Inizializzazione del sensore con controllo errori
	if (!sht31.begin(0x44)) {
		Serial.println("ERRORE: Sensore SHT31 non trovato!");
		WebcamSerial.println("ERRORE: Sensore SHT31 non trovato!");
	} else {
		Serial.println("Sensore SHT31 inizializzato correttamente");
		WebcamSerial.println("Sensore SHT31 inizializzato correttamente");
	}
	
	lastHeartbeatTime = millis(); // Inizializza il timer all'avvio
	
	pinMode(2, OUTPUT); // sets pin 2 as output for Camera Power
	pinMode(3, OUTPUT); // sets pin 3 as output for Heating
	pinMode(4, OUTPUT); // sets pin 4 as output for PC
	pinMode(5, OUTPUT); // sets pin 5 as output for Fan
	
	delay(1000); // let serial console settle
	Serial.println("Sistema inizializzato");
	WebcamSerial.println("Sistema inizializzato");
}

void loop() {
	// Controllo del timeout dell'heartbeat
	// Se non riceve un segnale di Heartbeat da 10 minuti resetta il PC
	if (millis() - lastHeartbeatTime > HEARTBEAT_TIMEOUT) {
		Serial.println("Timeout heartbeat - Reset PC");
		WebcamSerial.println("Timeout heartbeat - Reset PC");
		resetFunc();
	}	
	
	// Gestione comandi da WebcamSerial
	if (WebcamSerial.available()) {
		String command = WebcamSerial.readString();
		command.trim(); // Rimuove spazi e newline
		processCommand(command, WebcamSerial);
	}
	
	// Gestione comandi da Serial
	if (Serial.available()) {
		String command = Serial.readString();
		command.trim(); // Rimuove spazi e newline
		processCommand(command, Serial);
	}
}

void processCommand(String command, Stream& serialPort) {
	if (command == "b") { // Heartbeat signal
		lastHeartbeatTime = millis();
		serialPort.println("Heartbeat ricevuto");
		
	} else if (command == "C") {  // turn on Camera
		digitalWrite(2, LOW);
		serialPort.println("Camera ACCESA");
		
	} else if (command == "c") {  // turn off Camera
		digitalWrite(2, HIGH);
		serialPort.println("Camera SPENTA");
		
	} else if (command == "H") {  // turn on Heating
		digitalWrite(3, HIGH);
		serialPort.println("Riscaldamento ACCESO");
		
	} else if (command == "h") {  // turn off Heating
		digitalWrite(3, LOW);
		serialPort.println("Riscaldamento SPENTO");
		
	} else if (command == "P") {  // turn on PC
		digitalWrite(4, LOW);
		serialPort.println("PC ACCESO");
		
	} else if (command == "p") {  // turn off PC
		digitalWrite(4, HIGH);
		serialPort.println("PC SPENTO");
		
	} else if (command == "F") {  // turn on Fan
		digitalWrite(5, HIGH);
		serialPort.println("Ventola ACCESA");
		
	} else if (command == "f") {  // turn off Fan
		digitalWrite(5, LOW);
		serialPort.println("Ventola SPENTA");
		
	} else if (command == "n") {  // Test connessione
		if (&serialPort == &Serial) {
			serialPort.println("Serial OK");
		} else {
			serialPort.println("Software Serial OK");
		}
		
	} else if (command == "t") {  // Lettura temperatura
		float t_celsius = sht31.readTemperature();
		if (!isnan(t_celsius)) {
			serialPort.print("Temperatura: ");
			serialPort.print(t_celsius, 1);
			serialPort.println(" °C");
		} else {
			serialPort.println("ERRORE: Impossibile leggere la temperatura");
		}
		
	} else if (command == "u") {  // Lettura umidità
		float h_percent = sht31.readHumidity();
		if (!isnan(h_percent)) {
			serialPort.print("Umidità: ");
			serialPort.print(h_percent, 1);
			serialPort.println(" %");
		} else {
			serialPort.println("ERRORE: Impossibile leggere l'umidità");
		}
		
	} else if (command == "s") {  // Stato sistema
		printSystemStatus(serialPort);
		
	} else if (command.length() > 0) { // Comando sconosciuto
		serialPort.print("Comando sconosciuto: ");
		serialPort.println(command);
	}
}

void printSystemStatus(Stream& serialPort) {
	serialPort.println("=== STATO SISTEMA ===");
	serialPort.print("Camera: ");
	serialPort.println(digitalRead(2) == LOW ? "ACCESA" : "SPENTA");
	serialPort.print("Riscaldamento: ");
	serialPort.println(digitalRead(3) == HIGH ? "ACCESO" : "SPENTO");
	serialPort.print("PC: ");
	serialPort.println(digitalRead(4) == LOW ? "ACCESO" : "SPENTO");
	serialPort.print("Ventola: ");
	serialPort.println(digitalRead(5) == HIGH ? "ACCESA" : "SPENTA");
	
	// Lettura sensori con gestione errori
	float temp = sht31.readTemperature();
	float hum = sht31.readHumidity();
	
	serialPort.print("Temperatura: ");
	if (!isnan(temp)) {
		serialPort.print(temp, 1);
		serialPort.println(" °C");
	} else {
		serialPort.println("ERRORE");
	}
	
	serialPort.print("Umidità: ");
	if (!isnan(hum)) {
		serialPort.print(hum, 1);
		serialPort.println(" %");
	} else {
		serialPort.println("ERRORE");
	}
	
	unsigned long timeSinceHeartbeat = millis() - lastHeartbeatTime;
	serialPort.print("Ultimo heartbeat: ");
	serialPort.print(timeSinceHeartbeat / 1000);
	serialPort.println(" secondi fa");
	serialPort.println("====================");
}

void resetFunc() {
	digitalWrite(4, HIGH);
	lastHeartbeatTime = millis();
	delay(1000);
	digitalWrite(4, LOW);
	Serial.println("Reset PC completato");
	WebcamSerial.println("Reset PC completato");
}
