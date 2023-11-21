#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//#include <HttpClient.h>
#include <WiFiClientSecureBearSSL.h>

//WiFiClient wifiClient;

const char* ssid = "hola 3";
const char* password = "loki1879";

const int pinPuerta = 5;   // Número del pin para el sensor de la puerta
const int pinVentana = 4;  // Número del pin para el sensor de la ventana
const int pinUsuario = 16;  // Número del pin para simular la notificación del usuario

String token = "Bearer EAAMn7Yo1OKkBO57e3y9atyAldGoLJiAd8r5mfRNBU5wdT43etHzsZC3mjDVuWbG1r9XBnzIT9ZB8DEVZC8cmxJ4W6VZCvZBWf8o6goBu0PL7DkL6PpvGCHGTggL6URiUZAVwAAd9Pr71PknfHepCJrgzhi7A0q0x6pFEFsK0y6ATThBSp1VVydCyy4J6sewfyc3JXewYnWIRwpfDnI";
String servidorWhatsApp = "https://graph.facebook.com/v17.0/175396735655719/messages";
String payload1 = "{\"messaging_product\":\"whatsapp\",\"to\":\"523321321879\",\"type\":\"text\",\"text\": {\"body\": \"¡ALERTA! La puerta está abierta\"}}";
String payload2 = "{\"messaging_product\":\"whatsapp\",\"to\":\"523321321879\",\"type\":\"text\",\"text\": {\"body\": \"¡ALERTA! La ventana está abierta\"}}";

const String skillEndpoint = "https://api.amazonalexa.com/v1/skillnotifications/Skills/{skillId}/enablement";

// Reemplaza "{skillId}" con el ID de tu skill
const String skillId = "amzn1.ask.skill.9f964a95-817e-4823-a5b9-644f16eafdb0";

void setup() {
  Serial.begin(115200);
  pinMode(pinPuerta, INPUT);
  pinMode(pinVentana, INPUT);
  pinMode(pinUsuario, INPUT_PULLUP);
  connectToWiFi();
}

void loop() {
  int estadoUsuario = digitalRead(pinUsuario);

  Serial.print("Estado usuario: ");
  Serial.println(estadoUsuario == HIGH ? "En casa" : "ausente");

  if (estadoUsuario == LOW) {
    int estadoPuerta = digitalRead(pinPuerta);
    int estadoVentana = digitalRead(pinVentana);

    Serial.print("Estado de la puerta: ");
    Serial.println(estadoPuerta == HIGH ? "Abierta" : "Cerrada");

    Serial.print("Estado de la ventana: ");
    Serial.println(estadoVentana == HIGH ? "Abierta" : "Cerrada");

    if (estadoPuerta == HIGH) {
      sendMessage(servidorWhatsApp, payload1);
      sendNotificationToAlexa();
    }

    if (estadoVentana == HIGH) {
      sendMessage(servidorWhatsApp, payload2);
      sendNotificationToAlexa();
    }
  }

  delay(5000);
}

void connectToWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red WiFi...");
  }

  Serial.println("Conexión exitosa");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void sendMessage(String server, String message) {
  HTTPClient http;
  WiFiClient wifiClient;
  http.begin(wifiClient, server);
  http.setTimeout(30000);
  //HttpClient http(wifiClient, server, 443);
  //http.post("/path"); // Coloca la ruta correcta para enviar mensajes

  http.addHeader("Content-Type", "application/json");

  http.addHeader("Authorization", token);


  int httpPostCode = http.sendRequest("POST", payload2);


  if (httpPostCode > 0) {
    String payload = http.getString();
    Serial.println("Mensaje enviado con éxito:");
    Serial.println(payload);
  } else {
    Serial.print("Error al enviar el mensaje. Código HTTP: ");
    Serial.println(httpPostCode);
  }

  http.end();
}

void sendNotificationToAlexa() {
  HTTPClient http;
  WiFiClient wifiClient;

  // Crear una cadena temporal para almacenar el resultado de replace
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


