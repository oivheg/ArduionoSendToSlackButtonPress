#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <ArduinoHttpClient.h>


// Replace with your network credentials
const char* wifiname     = "Wifi";
const char* wifipassword = "wifipassword";
// List of sales related messages
String salesMessages[] = {
  "Et salg har blitt gjort", 
  "En ny ordre er plassert", 
  "En kunde har gjort et kjøp", 
  "Salget er vellykket",
  "En vare er solgt",
  "En bestilling er fullført",
  "Vi har et nytt salg",
  "A sale has been made",
  "A new order has been placed",
  "A customer made a purchase",
  "The sale was successful",
  "An item has been sold",
  "An order has been completed",
  "We have a new sale"
};


// // Add your MQTT Broker IP address, Port, User and Password
const char* mqtt_server = "mqttbroker";
const int mqtt_port = 1888;
const char* mqtt_user = "user";
const char* mqtt_password = "password";

// HTTP server to send the POST request
char serverAddress[] = "hooks.slack.com";
char uri[] = "/services/webhook"; // your Webhook URL

// Button connected to pin 2
const int buttonPin = 2;
int buttonState = 0;
// LED connected to pin 3
const int ledPin = 3;

WiFiSSLClient  wifi;
WiFiClient wifiNoSLL;
HttpClient client = HttpClient(wifi, serverAddress, 443);
PubSubClient mqtt(wifiNoSLL);

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLDOWN);
  pinMode(ledPin, OUTPUT);  // Set the LED pin as output
 
   WiFi.begin(wifiname, wifipassword);

}



void setup_wifi() {
  Serial.println("WIFI Connecting");
  // Connect to WiFi network
  WiFi.begin(wifiname, wifipassword);
  delay(2000);
  unsigned long startTime = millis();
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    // If it's been 5 seconds and still not connected, break the loop
    if (millis() - startTime > (5000 * attempts)) {
      Serial.println("Failed to connect to WIFI, retrying...");
      attempts++;
    }
    delay(1000);
    Serial.println("WIFI not connected");
  }
}

  void postData() {
  // if there's a successful connection:
 Serial.println("making POST request");
   String contentType = "application/json";
   int randomIndex = random(0, sizeof(salesMessages) / sizeof(salesMessages[0]));
  String postData = "{\"text\":\"" + salesMessages[randomIndex] + "\"}";

 client.post(uri, contentType, postData);

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

}

void loop() {
    // Check if button is pressed
    buttonState = digitalRead(buttonPin);
    Serial.println(buttonState);

    if (buttonState == HIGH) {
        digitalWrite(ledPin, HIGH); // Turn on the LED

        // Check if WiFi is already connected
        if (WiFi.status() != WL_CONNECTED) {
            setup_wifi(); // Reconnect to WiFi
        }

        if (!mqtt.connected()) {
            connectmqtt();
        }
        mqtt.loop();

        // Send MQTT message to trigger remoteBlinker

        // Post data
        postData();

        // Disconnect from WiFi
        WiFi.disconnect();
        digitalWrite(ledPin, LOW); // Turn off the LED
    }

    // Non-blocking delay using millis()
    static unsigned long previousMillis = 0;
    const unsigned long interval = 1000; // 1 second interval

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        // Other tasks can run here without blocking
    }
}

void connectmqtt() {
  // Loop until we're reconnected
   mqtt.setServer(mqtt_server, mqtt_port);
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    Serial.println(WiFi.status());
    if (mqtt.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("MQTTconnected");
      mqtt.publish("remoteblinker/on", "notRelevant");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
