#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

/*********
  Sensoil - Soil Salinity Measurement device with Temperature and Humidity Sensors
*********/

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include "DHT.h"

// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Replace with your network details
//const char* ssid = "RCWx";
//const char* password = "Rcx261523";
const char* ssid = "rbc236";
const char* password = "royce236";

// Web Server on port 80
WiFiServer server(80);

// DHT Sensor
const int DHTPin = D3;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

int sensor_pin = A0; //Declare the pin for soil moisture sensor
int output_value ;  //Store the value from analog pin A0

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Sensoil Tech");
  lcd.setCursor(0,1);
  lcd.print("Powered by ESP");
  delay(1000);
  lcd.clear();
  
  dht.begin();
  
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  lcd.setCursor(0,0);
  lcd.print("Connecting to ");
  Serial.println(ssid);
  lcd.setCursor(0,1);
  lcd.print(ssid);
  WiFi.begin(ssid, password);
  delay(1000);
  
  while (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    delay(500);
    Serial.print(".");
    lcd.setCursor(0,0);
    lcd.print("Connecting......");
    delay(500);
  }
  lcd.clear();
  Serial.println("");
  Serial.println("WiFi connected");
  lcd.setCursor(0,0);
  lcd.print("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());

}

// runs over and over again
void loop() {
  // Listenning for new clients
  WiFiClient client = server.available();


  
  if (client) {
    lcd.clear();
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n' && blank_line) {

            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float h = dht.readHumidity();
            // Read temperature as Celsius (the default)
            float t = dht.readTemperature();
            // Read temperature as Fahrenheit (isFahrenheit = true)
            float f = dht.readTemperature(true);
            // Check if any reads failed and exit early (to try again).
            
            output_value= analogRead(sensor_pin); //Store the value from analog pin A0
            output_value = map(output_value,550,0,0,100); //Store the value from analog pin A0

            // After value is retrieved load it onto LCD immediately
            lcd.setCursor(0,0);
            lcd.print("Temp(C): ");
            lcd.print(t);
            lcd.print(" C");
            lcd.setCursor(0,1);
            lcd.print("Humidity: ");
            lcd.print(h);
            delay(2000); // you can something here to make it toggleable 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Moisture: ");
            lcd.print(output_value);
            lcd.print(" %");
            lcd.setCursor(0,1);
            lcd.print("Salinity: ");
            // add your adjustment here
            lcd.print("N/A"); // dummy
            delay(2000);
            lcd.clear();
            
            if (isnan(h) || isnan(t) || isnan(f)) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");
              strcpy(fahrenheitTemp, "Failed");
              strcpy(humidityTemp, "Failed");         
            }
            else{
              // Computes temperature values in Celsius + Fahrenheit and Humidity
              float hic = dht.computeHeatIndex(t, h, false);       
              dtostrf(hic, 6, 2, celsiusTemp);             
              float hif = dht.computeHeatIndex(f, h);
              dtostrf(hif, 6, 2, fahrenheitTemp);         
              dtostrf(h, 6, 2, humidityTemp);
              // You can delete the following Serial.print's, it's just for debugging purposes
              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.print(" %\t Temperature: ");
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.print(" *F");
              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.print(" %\t Temperature: ");
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.println(" *F");
              
              Serial.print("Mositure : "); //Added Line
              Serial.print(output_value); //Added Line
              Serial.println("%"); //Added Line
            }
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println("Refresh: 5"); //The page refreshed every 5 seconds 
            client.println();
            // your actual web page that displays temperature and humidity
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head></head><body><h1>ESP8266 - Temperature and Humidity</h1><h3>Temperature in Celsius: ");
            client.println(t);
            client.println("*C</h3><h3>Temperature in Fahrenheit: ");
            client.println(f);
            client.println("*F</h3><h3>Humidity: ");
            client.println(h);
            client.println("%</h3><h3>");
            client.println("<h1>Soil Moisture</h1>"); //Added Line
            client.println("Moisture: "); //Added Line
            client.println(output_value); //Added Line
            client.println("%</h3><h3>"); //Added Line
            client.println("</body></html>");     
            break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}   
