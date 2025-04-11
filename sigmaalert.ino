#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Ethernet.h>

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Rede
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[]  = {  169, 254, 3, 148 };
EthernetServer server(80);

// Estado
String readString = "";
String mensagem = "";
bool novaMensagem = false;
bool buzzerAtivo = false;

// Pinos
const int pinoBuzzer = 3;
const int botaoLimpar = 2;
const int botaoSilenciar = 4;
const int ledVermelho = 5;
const int ledVerde = 6;

// Controle de piscar
unsigned long ultimaTrocaLed = 0;
bool estadoLed = false;
const int intervaloPiscar = 300;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Aguardando msg");

  pinMode(3, OUTPUT);
  pinMode(botaoLimpar, INPUT);
  pinMode(botaoSilenciar, INPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  digitalWrite(ledVermelho, LOW);
  digitalWrite(ledVerde, LOW);
}

void loop() {
  // Botão silenciar
  if (digitalRead(botaoSilenciar) == HIGH && buzzerAtivo) {
    noTone(3);
    Serial.println("silenciar clicado");
    digitalWrite(pinoBuzzer, LOW);
    digitalWrite(ledVermelho, LOW);
    buzzerAtivo = false;
  }

  // Botão limpar
  if (digitalRead(botaoLimpar) == HIGH && mensagem != "") {
    noTone(3);
    Serial.println("limpar clicado");
    digitalWrite(pinoBuzzer, LOW);
    digitalWrite(ledVermelho, LOW);
    buzzerAtivo = false;
    mensagem = "";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Aguardando msg");

    novaMensagem = true;

    // Pisca LED verde
    digitalWrite(ledVerde, HIGH);
    delay(200);
    digitalWrite(ledVerde, LOW);
  }

  // LED vermelho + buzzer piscando
  if (buzzerAtivo) {
    // Serial.println("buzzer on");
    // tone(pinoBuzzer, 262, 300);
    unsigned long agora = millis();
    if (agora - ultimaTrocaLed >= intervaloPiscar) {
      ultimaTrocaLed = agora;
      estadoLed = !estadoLed;
      if (estadoLed) {
        tone(3, 25);
      } else {
        noTone(3);
      }
      digitalWrite(ledVermelho, estadoLed ? HIGH : LOW);
    }
  }
  else {
    // Serial.println("buzzer off");
    // noTone(pinoBuzzer);
  }

  // Comunicação com navegador
  EthernetClient client = server.available();
  if (client) {
    readString = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        readString += c;
        if (c == '\n') {
          Serial.println(readString);

          int msgIndex = readString.indexOf("GET /?msg=");
          if (msgIndex != -1) {
            int start = msgIndex + 10;
            int end = readString.indexOf(' ', start);
            mensagem = readString.substring(start, end);
            mensagem.replace('+', ' ');

            if (mensagem.length() > 16) {
              mensagem = mensagem.substring(0, 16);
            }

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Msg recebida:");
            lcd.setCursor(0, 1);
            lcd.print(mensagem);

            // Ativa buzzer e LED vermelho
            buzzerAtivo = true;
            novaMensagem = true;
            ultimaTrocaLed = millis(); // reinicia controle de piscar
          }

          // Página web
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html><body>");
          if (readString.indexOf("/?msg=") != -1) {
            client.println("<h1>Mensagem recebida</h1>");
            client.println("<a href='/'>Enviar outra mensagem</a>");
          } else {
            client.println("<form action='/' method='GET'>");
            client.println("Mensagem (max 16):<br>");
            client.println("<input type='text' name='msg' maxlength='16'>");
            client.println("<input type='submit' value='Enviar'>");
            client.println("</form>");
          }
          client.println("</body></html>");
          client.stop();
        }
      }
    }
  }
}
