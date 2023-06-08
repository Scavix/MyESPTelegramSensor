#include "DHT.h"           // DTH lib
#include "CTBot.h"         // Telegram bot library
#include "credentials.h"

#define DHTPIN 5           // Pin to read sensor
#define DHTTYPE DHT22      // We use a DTH22
#define RELAYPIN 11        // Pin to control relay
#define BAUD_RATE 9600     // Refresh serial rate in baud
#define REFRESH_DELAY 9600 // Refresh rate in loop

CTBot myBot;
DHT dht(DHTPIN, DHTTYPE);
float h, t;
String humidity, temperature, toSend;
bool swap;

void setup() {

  // start serial console
  Serial.begin(BAUD_RATE);
  Serial.println("Starting TelegramBot...");
  pinMode(LED_BUILTIN, OUTPUT);

  // connect the ESP8266 to the desired access point
  myBot.wifiConnect(WIFI_SSID, WIFI_PASS);

  // set the telegram bot token
  myBot.setTelegramToken(TELEGRAM_TOKEN);

  // check if all things are ok
  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");
  
  // init swapper variable
  swap = false;

  // init dht
  dht.begin();
}

void loop() {
  // we read data
  h = dht.readHumidity();
  t = dht.readTemperature();

  // we check for variable missing
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from dht!");
  }

  // we create a string containing the data to send
  humidity = "Humidity: " + String(h) + "\n";
  temperature = "Temperature: " + String(t) + "\n";
  toSend = humidity + temperature;

  // print to serial
  Serial.print(toSend);

  // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (myBot.getNewMessage(msg)) {
    // and the message is /data
    if (msg.text.equalsIgnoreCase("/data")) {
      // ...forward it to the sender
      myBot.sendMessage(msg.sender.id, toSend);
      // else if the message is /swap
    } else if (msg.text.equalsIgnoreCase("/swap")) {
      if (swap) {
        swap = false;
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(RELAYPIN, LOW);
      } else {
        swap = true;
        digitalWrite(LED_BUILTIN, HIGH);
      }
      digitalWrite(RELAYPIN, HIGH);
      myBot.sendMessage(msg.sender.id, "Worka bene, no worries!\n");
    } else {
      myBot.sendMessage(msg.sender.id, msg.text);
    }
  }

  // wait delay in milliseconds
  delay(REFRESH_DELAY);
}
