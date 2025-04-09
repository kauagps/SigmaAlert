#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h> // ou <LiquidCrystal_I2C.h> se for I2C

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 169, 254, 24, 203 };
EthernetServer server(80);

String readString = "";
String mensagem = "";

const int lcdRS = 7, lcdEN = 6, lcdD4 = 5, lcdD5 = 4, lcdD6 = 3, lcdD7 = 2;
LiquidCrystal lcd(lcdRS, lcdEN, lcdD4, lcdD5, lcdD6, lcdD7);

int buzzerPin = 8;
int botaoPin = 9;

bool aguardandoConfirmacao = false;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Sistema iniciado");

  pinMode(buzzerPin, OUTPUT);
  pinMode(botaoPin, INPUT_PULLUP); // Botão com resistor interno
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    readString = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        readString += c;
        if (c == '\n') {
          Serial.println(readString);

          // Verifica se tem parâmetro ?msg=
          int msgIndex = readString.indexOf("GET /?msg=");
          if (msgIndex != -1) {
            int start = msgIndex + 10;
            int end = readString.indexOf(' ', start);
            mensagem = readString.substring(start, end);
            mensagem.replace('+', ' '); // Ajusta espaço no texto

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Msg recebida:");
            lcd.setCursor(0, 1);
            lcd.print(mensagem);

            aguardandoConfirmacao = true;
          }

          // Resposta para navegador
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html><body>");
          client.println("<h1>Mensagem recebida</h1>");
          client.println("</body></html>");
          client.stop();
        }
      }
    }
  }

  // Alarme enquanto espera confirmação
  if (aguardandoConfirmacao) {
    digitalWrite(buzzerPin, HIGH);
    if (digitalRead(botaoPin) == LOW) { // Botão pressionado
      digitalWrite(buzzerPin, LOW);
      aguardandoConfirmacao = false;

      lcd.clear();
      lcd.print("Confirmado!");
      delay(2000);
      lcd.clear();
    }
  } else {
    digitalWrite(buzzerPin, LOW);
  }
}
