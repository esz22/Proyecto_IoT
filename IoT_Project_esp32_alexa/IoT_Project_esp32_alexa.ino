#include <WiFi.h>
#include <HTTPClient.h>
#include "fauxmoESP.h"

const char* ssid = "hola 3";
const char* password = "loki1879";

const int pinPuerta = 22;   // Número del pin para el sensor de la puerta
const int pinVentana = 21;  // Número del pin para el sensor de la ventana

#define LED          2
#define ID_ESP       "switch"
fauxmoESP fauxmo;
bool usuarioEnCasa = true;  // Inicialmente, el usuario está en casa

String token="Bearer EAAMn7Yo1OKkBO57e3y9atyAldGoLJiAd8r5mfRNBU5wdT43etHzsZC3mjDVuWbG1r9XBnzIT9ZB8DEVZC8cmxJ4W6VZCvZBWf8o6goBu0PL7DkL6PpvGCHGTggL6URiUZAVwAAd9Pr71PknfHepCJrgzhi7A0q0x6pFEFsK0y6ATThBSp1VVydCyy4J6sewfyc3JXewYnWIRwpfDnI";
String servidor = "https://graph.facebook.com/v17.0/175396735655719/messages";
String payload1 = "{\"messaging_product\":\"whatsapp\",\"to\":\"523321321879\",\"type\":\"text\",\"text\": {\"body\": \"¡ALERTA! La puerta está abierta\"}}";
String payload2 = "{\"messaging_product\":\"whatsapp\",\"to\":\"523321321879\",\"type\":\"text\",\"text\": {\"body\": \"¡ALERTA! La ventana está abierta\"}}";

void setup() {
  Serial.begin(9600);
  pinMode(pinPuerta, INPUT);
  pinMode(pinVentana, INPUT);
  pinMode(LED, OUTPUT);
  connectToWiFi();
  connectToAlexa();
}

void loop() {
  fauxmo.handle();

  Serial.print("Estado usuario: ");
  Serial.println(usuarioEnCasa == true ? "En casa" : "Ausente");

  if (usuarioEnCasa == false) {
    digitalWrite(LED, HIGH);
    int estadoPuerta = digitalRead(pinPuerta);
    int estadoVentana = digitalRead(pinVentana);

    Serial.print("Estado de la puerta: ");
    Serial.println(estadoPuerta == HIGH ? "Abierta" : "Cerrada");

    Serial.print("Estado de la ventana: ");
    Serial.println(estadoVentana == HIGH ? "Abierta" : "Cerrada");

    if (estadoPuerta == HIGH) {
      sendMessage(servidor, payload1);
    }

    if (estadoVentana == HIGH) {
      sendMessage(servidor, payload2);
    }
  } else {
    digitalWrite(LED, LOW);
  }

  delay(1000);
}

void connectToWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Conectando a la red WiFi...");
  }

  Serial.println("Conexión exitosa");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void sendMessage(String server, String message) {
  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      http.begin(servidor.c_str());
      http.addHeader("Content-Type", "application/json"); 
      http.addHeader("Authorization", token);    
      int httpPostCode = http.POST(message);
      if (httpPostCode > 0) {
        int httpResponseCode = http.GET();
        if (httpResponseCode>0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
      }
      http.end();
    }
    else {
      Serial.println("WiFi Desconectado");
    }
}

void connectToAlexa() {
  fauxmo.createServer(true);
  fauxmo.setPort(80);
  fauxmo.enable(true);
  fauxmo.addDevice(ID_ESP);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    if (strcmp(device_name, ID_ESP) == 0) {
      if (state) {
        // Usuario en casa
        usuarioEnCasa = true;
      } else {
        // Usuario ausente
        usuarioEnCasa = false;
      }
    }
  });
}

