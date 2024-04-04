#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <ArduinoHttpClient.h>


// Replace with your network credentials
const char* ssid     = "WIFI";
const char* password = "PASSWORD";
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

// HTTP server to send the POST request
char serverAddress[] = "hooks.slack.com";
char uri[] = "/services/webhoocklink"; // your Webhook URL

// Button connected to pin 2
const int buttonPin = 2;
int buttonState = 0;
// LED connected to pin 3
const int ledPin = 3;

WiFiSSLClient  wifi;
HttpClient client = HttpClient(wifi, serverAddress, 443);


void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLDOWN);
  pinMode(ledPin, OUTPUT);  // Set the LED pin as output

  WiFi.begin(ssid, password);

}


void setup_wifi() {
  Serial.println("WIFI Connecting");
  // Connect to WiFi network
  WiFi.begin(ssid, password);
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
     digitalWrite(ledPin, HIGH);  // Turn on the LED
    // Check if WiFi is already connected
    if (WiFi.status() != WL_CONNECTED) {
      // Reconnect to WiFi
      setup_wifi();
    }
    // Post data
    postData();
    // Disconnect from WiFi
    WiFi.disconnect();
    digitalWrite(ledPin, LOW);  // Turn off the LED
  }
      delay(1000);
}