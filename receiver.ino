#include <SoftwareSerial.h>
#include <DHT.h>
// #include <Wire.h>

// Pin used in ESP32, GPIO18 and GPIO19
#define LORA_RX 5
#define LORA_TX 19
#define soil_moistrure_pin 34


int LED = 26;
int LED2 = 25;
int LED3 = 14;


SoftwareSerial LoRaSerial(LORA_RX, LORA_TX);

// Định nghĩa các chân và kiểu cảm biến DHT
#define DHTPIN 23         // Chân kết nối cảm biến DHT11
#define DHTTYPE DHT11      // Sử dụng cảm biến DHT11

// #define RELAY_PIN1 0  // định nghĩa rơ le điều khiển máy bơm
// #define RELAY_PIN2 2  // định nghĩa rơ le điều khiển máy quạt

DHT dht(DHTPIN, DHTTYPE);  // Khởi tạo đối tượng cảm biến DHT
unsigned long previousMillis = 0;
const long interval = 2000;  // Định kỳ 2 giây để đọc cảm biến DHT11

void setup() {
  // Init serial communication
  Serial.begin(115200);
  // Wire.begin();
  LoRaSerial.begin(9600);

  dht.begin();

  // pinMode(RELAY_PIN1, OUTPUT);
  // digitalWrite(RELAY_PIN1, LOW);  // Cấu hình rơ le ở mức cao
  // pinMode(RELAY_PIN2, OUTPUT);
  // digitalWrite(RELAY_PIN2, LOW);  // Cấu hình rơ le ở mức cao

  pinMode(LED, OUTPUT); 

  pinMode(LED2, OUTPUT); 

  pinMode(LED3, OUTPUT);

  Serial.println("ESP32 and LoRa AS32 are Ready!");
}

void loop() {
  Readsensor_DHT();
  Led();
}
// Đọc cảm biến DHT11
void Readsensor_DHT() {
  int doc_cambien = analogRead(soil_moistrure_pin);
  int phantram = map(doc_cambien,0,4095,0,100);
  unsigned long currentMillis = millis();
  float h1 = dht.readHumidity();
  float t1 = dht.readTemperature();
  float phantramthuc = 100 - phantram;
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (!isnan(h1) && !isnan(t1)) {
      Serial.print("Nhiệt độ: ");
      Serial.print(t1);
      Serial.print(" °C | Độ ẩm: ");
      Serial.print(h1);
      Serial.println(" %");
      Serial.print("Độ ẩm đất: ");
      Serial.print(phantramthuc); 
      Serial.println("%");
      LoRaSerial.print("t1: ");
      LoRaSerial.print(t1);
      LoRaSerial.print("h1: ");
      LoRaSerial.print(h1);
      LoRaSerial.print("s1: ");
      LoRaSerial.print(phantramthuc);
    } else {
      Serial.println("Failed to read from DHT sensor!");
    }
  delay(3000);
  } 
}

void Led()
{
  if(LoRaSerial.available())
  {
    String receivedData = LoRaSerial.readString();
    if(receivedData != "")
    {
      receivedData.trim();
      LoRaSerial.println(receivedData);
      if(receivedData == "0")
      {
        digitalWrite(LED, LOW);
        receivedData = "";

      }
      else if(receivedData == "1")
      {
        digitalWrite(LED, HIGH);
        receivedData = "";

      }
      else if(receivedData == "2")
      {
        digitalWrite(LED2, LOW);
        receivedData = "";

      }
      else if(receivedData == "3")
      {
        digitalWrite(LED2, HIGH);
        receivedData = "";

      }
      else if(receivedData == "4")
      {
        digitalWrite(LED3, LOW);
        receivedData = "";

      }
      else if(receivedData == "5")
      {
        digitalWrite(LED3, HIGH);
        receivedData = "";

      }
    }
  }
}
