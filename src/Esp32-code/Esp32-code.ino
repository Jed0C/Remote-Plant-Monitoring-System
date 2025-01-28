#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <Adafruit_AHTX0.h>

#define uS_TO_S_FACTOR 1000000  // Conversion factor for micro seconds to seconds 
#define TIME_TO_SLEEP  600  //time to sleep in seconds (10 min)

TinyGPSPlus gps;
HardwareSerial SerialGPS(1);

Adafruit_AHTX0 aht;
Adafruit_Sensor *aht_humidity;

//GPS pins
const int GPS_RX = 17; // RX pin for GPS
const int GPS_TX = 16; // TX pin for GPS

//lora pins
const int CS = 5;
const int RST = 13;
const int DIO0 = 14;


void sendData(){
  String Data ="";

  //read from moisure sensor
  sensors_event_t moisureData;
  aht_humidity->getEvent(&moisureData);
  
  //read from gps
  while (SerialGPS.available()){
    gps.encode(SerialGPS.read());
  }

  if (gps.location.isValid()){
    Data = "Lat: " + String(gps.location.lat(), 6) + ", Lng: " + String(gps.location.lng(), 6) + ", ";
    Data = Data + "Date: " + String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year());
    Data = Data + "  Time: " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
    Data = Data + String(moisureData.relative_humidity);
  }

//send data
  LoRa.beginPacket();
  LoRa.print(Data);
  LoRa.endPacket();
  
}


void setup() {
  
  //Serial.begin(115200); // Serial for debugging
  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  aht.begin();
  aht_humidity = aht.getHumiditySensor();

  LoRa.setPins(CS, RST, DIO0);//setup LoRa module
  LoRa.begin(868E6); //Lora frequency used in Tunisia
  LoRa.setSyncWord(0xA0);//sync word with the receiver ( word can be into range 0 to 0xFF)
  
  sendData();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); //sleep 10 minutes
  esp_deep_sleep_start();

}

void loop() {//nothing here
}
