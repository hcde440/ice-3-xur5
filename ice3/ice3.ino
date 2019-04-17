

// Melody Xu
// HCDE 440 
// 4/16/19
// Ice-3

// This sketch will collect temperature , pressure, and humidity data from the dht and mpl
// sensors and display them on the OLED display
// Data is also sent to my adafruit IO dashboard, link is below
// https://io.adafruit.com/xur5/dashboards/melody

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.

#include "config.h"
#include <DHT.h>                // for DHT
#include <SPI.h>                // for mpl
#include <DHT_U.h>              // for DHT
#include <Wire.h>               // for mpl
#include <ESP8266WiFi.h>        // for board
#include <Adafruit_GFX.h>       // for display
#include <Adafruit_Sensor.h>    // for DHT
#include <Adafruit_SSD1306.h>   // for display
#include <Adafruit_MPL115A2.h>  // for mpl

/************************ Example Starts Here *******************************/

// set up the temperature, pressure, and humidity feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *pressure = io.feed("pressure");

// pin connected to DH22 data line
#define DATA_PIN 12

// create DHT22 
DHT_Unified dht(DATA_PIN, DHT22);
Adafruit_MPL115A2 mpl; // initialize mpl pressure sensor

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // initialize oled 

// initialize variables for temperature, humidity, and pressure
float tem;
float hum;
float pre;

void setup() {
  // start the serial connection
  Serial.begin(115200);

  // System status
  while(! Serial);
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));
   
  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  Serial.println("initializing dht");
  dht.begin(); // begin dht (non i2c) sensor

  Serial.println("initializing mpl");
  mpl.begin(); // begin mpl (i2c) sensor


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.display(); // initiate the display
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

  // Initiate sensor values 
  pre = 0;
  hum = 0;
  tem = 0;
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // initialize sensor event
  sensors_event_t event;

  dht.temperature().getEvent(&event);

  tem = event.temperature;

  Serial.print("Temperature: ");
  Serial.print(tem);
  Serial.println("C");

  // save celsius to Adafruit IO
  temperature->save(tem);

  dht.humidity().getEvent(&event);
  hum = event.relative_humidity;

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println("%");
  
  // save humidity to Adafruit IO
  humidity->save(event.relative_humidity);
  
  // gets pressure from mpl sensor
  pre = mpl.getPressure();  
  Serial.print("Pressure: "); 
  Serial.print(pre, 4); 
  Serial.println(" kPa");

  // save pressure to Adafruit IO
  pressure->save(pre);
 
  // displays text on OLED
  testscrolltext(); 
  delay(3000);
}

// prints the temp, pressure and humidity on the OLED
void testscrolltext(void) { 
  // converts to string
  String sPre = String(pre);
  String sTem = String(tem); 
  String sHum = String(hum); 
  
  display.clearDisplay(); 
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(sTem + "C " + sPre + "kPa "); // print temp and pressure
  display.setCursor(10, 25);
  display.println(sHum + "% "); // print humidity in the next line
  display.display();      
  delay(100);

  // Scroll left
  display.startscrollleft(0x00, 0x0F);
  delay(1000);
}
