#include <SPI.h> // Biblioteca utilizada para comunicação com o Arduino
#include <Ethernet.h>
// A linha abaixo permite definir o endereço físico (MAC ADDRESS) da placa de rede. 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = {  169, 254, 24, 203 }; // Define o endereço IP.
// Porta onde estará aberta para comunicação Internet e Arduino.
EthernetServer server(80);
String readString;
int Pin = 2; // Pino digital onde será ligado e desligado o LED.
void setup(){ 
	pinMode(Pin, OUTPUT); // Define o Pino 9 como saída.
	Ethernet.begin(mac, ip); // Chama o MAC e o endereço IP da placa Ethernet.
	// Inicia o servidor que esta inserido junto a placa Ethernet.
	server.begin(); 
}
void loop() { 
	EthernetClient client = server.available();
	if (client) {
		while (client.connected()) {
			if (client.available()) {
				char c = client.read();
				if (readString.length() < 100) {
					readString += c;
				}
				if (c == '\n') {
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println();
					client.println("Acende LED <br />");
					client.println("Projeto basico para demonstracao com Shield Ethernet <br />");
					client.println("<a href=""LedOn"">Acender led</a><br />");
					client.println("<a href=""LedOff"">Apagar led</a><br />");
					delay(1);
					client.stop();
					if(readString.indexOf("LedOn") > 0) {
						digitalWrite(Pin, HIGH); // Liga LED.
					} 
					else {
						if(readString.indexOf("LedOff") > 0) {
					    digitalWrite(Pin, LOW); // Desliga LED.
					  }
				  }
				  readString="";
			  }
		  }
	  }
  }
}