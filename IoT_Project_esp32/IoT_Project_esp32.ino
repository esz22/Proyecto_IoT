#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "hola 3";
const char* password = "loki1879";

const int pinPuerta = 22;   // Número del pin para el sensor de la puerta
const int pinVentana = 21;  // Número del pin para el sensor de la ventana
const int pinUsuario = 23;  // Número del pin para simular la notificación del usuario

#define LED 2

String token="Bearer EAAMn7Yo1OKkBO57e3y9atyAldGoLJiAd8r5mfRNBU5wdT43etHzsZC3mjDVuWbG1r9XBnzIT9ZB8DEVZC8cmxJ4W6VZCvZBWf8o6goBu0PL7DkL6PpvGCHGTggL6URiUZAVwAAd9Pr71PknfHepCJrgzhi7A0q0x6pFEFsK0y6ATThBSp1VVydCyy4J6sewfyc3JXewYnWIRwpfDnI";
String servidor = "https://graph.facebook.com/v17.0/175396735655719/messages";
String payload1 = "{\"messaging_product\":\"whatsapp\",\"to\":\"523321321879\",\"type\":\"text\",\"text\": {\"body\": \"¡ALERTA! La puerta está abierta\"}}";
String payload2 = "{\"messaging_product\":\"whatsapp\",\"to\":\"523321321879\",\"type\":\"text\",\"text\": {\"body\": \"¡ALERTA! La ventana está abierta\"}}";

const String skillEndpoint = "https://api.amazonalexa.com/v1/skillnotifications/Skills/{skillId}/enablement";
const String skillId = "amzn1.ask.skill.9f964a95-817e-4823-a5b9-644f16eafdb0";

void setup() {
  Serial.begin(9600);
  pinMode(pinPuerta, INPUT);
  pinMode(pinVentana, INPUT);
  pinMode(pinUsuario, INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  connectToWiFi();
}

void loop() {
  int estadoUsuario = digitalRead(pinUsuario);

  Serial.print("Estado usuario: ");
  Serial.println(estadoUsuario == HIGH ? "En casa" : "ausente");

  if (estadoUsuario == LOW) {
    digitalWrite(LED,HIGH);
    int estadoPuerta = digitalRead(pinPuerta);
    int estadoVentana = digitalRead(pinVentana);

    Serial.print("Estado de la puerta: ");
    Serial.println(estadoPuerta == HIGH ? "Abierta" : "Cerrada");

    Serial.print("Estado de la ventana: ");
    Serial.println(estadoVentana == HIGH ? "Abierta" : "Cerrada");

    if (estadoPuerta == HIGH) {
      sendMessage(servidor, payload1);
      sendNotificationToAlexa();
    }

    if (estadoVentana == HIGH) {
      sendMessage(servidor, payload2);
      sendNotificationToAlexa();
    }
  }
  else{
    digitalWrite(LED,LOW);
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

void sendNotificationToAlexa() {
  HTTPClient http;
  WiFiClient wifiClient;

  String endpoint = skillEndpoint;
  endpoint.replace("{skillId}", skillId);

  http.begin(wifiClient, endpoint);
  http.addHeader("Authorization", token);

  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.print("Notificación de Alexa enviada. Código HTTP: ");
    Serial.println(httpCode);
  } else {
    Serial.print("Error al enviar la notificación de Alexa. Código HTTP: ");
    Serial.println(httpCode);
  }

  http.end();
}