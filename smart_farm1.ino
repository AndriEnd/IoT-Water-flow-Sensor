#include <Arduino.h>

//Firebase Library
#include <Firebase_ESP_Client.h>

// DHT library
#include <DHT.h> 
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define sensorMoisture 4
#define relay1 5
#define relay2 18

int MoistureValue;

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "IoT"
#define WIFI_PASSWORD "jujuvalo"

// Insert Firebase project API Key
#define API_KEY "AIzaSyA6U7oUoW7zpeBbRPu6R8BzUvpMHbYrDuE"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://dht11-feb85-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

void setup() {
  Serial.begin(115200);

  //pin Relay
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  dht.begin();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
    
  float t = dht.readTemperature();
  int h = dht.readHumidity();
  String suhu = (String)t + "°C";
  String hum = (String)h + "%";
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    // menulis data suhu dari DHT11 ke firebase
    if (Firebase.RTDB.setString(&fbdo, "value1", suhu)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      delay(5000);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;

    // menulis data kelembaban dari DHT11 ke firebase
    if (Firebase.RTDB.setString(&fbdo, "value2", hum)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      delay(5000);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
  
    Serial.print("MoistureValue Value : ");
    Serial.println(MoistureValue);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));

    if (t>30 ) {
      digitalWrite(relay1, LOW); 
    }else{
      digitalWrite(relay1, HIGH); 
    }
  
    if (MoistureValue<4000) {
      digitalWrite(relay2, HIGH); 
    }else{
      digitalWrite(relay2, LOW); 

    }
    delay(500);
}
