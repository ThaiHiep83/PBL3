#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <DHT.h>
#include <SimpleTimer.h>
#include <wifi_provisioning/manager.h>
#include <EEPROM.h>
//---------------------------------------------------
// Set Defalt Values
#define DEFAULT_Temperature 0
#define DEFAULT_Humidity 0
//---------------------------------------------------
const char *service_name = "Cali House";
const char *pop = "anbinh88";
//---------------------------------------------------
#define EEPROM_SIZE 4
//---------------------------------------------------
// define the Device Names
char device1[] = "Switch1"; 
char device2[] = "Switch2"; 
char device3[] = "Switch3"; 
//---------------------------------------------------
// define the GPIO connected with Relays and switches
static uint8_t RELAY_1 = 6;  //D6
static uint8_t RELAY_2 = 2;
static uint8_t RELAY_3 = 4;
static uint8_t DHTPIN = 9;
//---------------------------------------------------

bool wifi_connected = 0;

DHT dht(DHTPIN, DHT11);

SimpleTimer Timer;

static TemperatureSensor temperature("Temperature");
static TemperatureSensor humidity("Humidity");

static uint8_t WIFI_LED    = 2;   //D2
static uint8_t gpio_reset = 0;
//---------------------------------------------------
/* Variable for reading pin status*/
// Relay State
bool STATE_RELAY_1 = LOW; //Define integer to remember the toggle state for relay 1 
bool STATE_RELAY_2 = LOW; //Define integer to remember the toggle state for relay 2 
bool STATE_RELAY_3 = LOW; //Define integer to remember the toggle state for relay 3 


//---------------------------------------------------
//The framework provides some standard device types
//like switch, lightbulb, fan, temperature sensor.
static Switch my_switch1(device1, &RELAY_1);
static Switch my_switch2(device2, &RELAY_2); 
static Switch my_switch3(device3, &RELAY_3); 


//---------------------------------------------------

/****************************************************************************************************
 * sysProvEvent Function
*****************************************************************************************************/
void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {      
        case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
        printQR(service_name, pop, "ble");
#else
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
        printQR(service_name, pop, "softap");
#endif        
        break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.printf("\nConnected to Wi-Fi!\n");
        digitalWrite(WIFI_LED, HIGH);
        wifi_connected = 1;
        delay(500);
        break;
    }
}

/****************************************************************************************************
 * write_callback Function
*****************************************************************************************************/
void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();
    //----------------------------------------------------------------------------------
    if(strcmp(device_name, device1) == 0) {
      
      Serial.printf("Lightbulb1 = %s\n", val.val.b? "true" : "false");
      
      if(strcmp(param_name, "Power") == 0) {
        //Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        STATE_RELAY_1 = val.val.b;
        STATE_RELAY_1 = !STATE_RELAY_1;
        control_relay(1, RELAY_1, STATE_RELAY_1);
        //(STATE_RELAY_1 == false) ? digitalWrite(RELAY_1, HIGH) : digitalWrite(RELAY_1, LOW);
        //param->updateAndReport(val);
      }
    }
    //----------------------------------------------------------------------------------
    if(strcmp(device_name, device2) == 0) {
      
      Serial.printf("Lightbulb2 = %s\n", val.val.b? "true" : "false");
      
      if(strcmp(param_name, "Power") == 0) {
        //Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        STATE_RELAY_2 = val.val.b;
        STATE_RELAY_2 = !STATE_RELAY_2;
        control_relay(2, RELAY_2, STATE_RELAY_2);
        //(STATE_RELAY_1 == false) ? digitalWrite(RELAY_1, HIGH) : digitalWrite(RELAY_1, LOW);
        //param->updateAndReport(val);
      }
    }
    //----------------------------------------------------------------------------------
    if(strcmp(device_name, device3) == 0) {
      
      Serial.printf("Lightbulb3 = %s\n", val.val.b? "true" : "false");
      
      if(strcmp(param_name, "Power") == 0) {
        //Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        STATE_RELAY_3 = val.val.b;
        STATE_RELAY_3 = !STATE_RELAY_3;
        control_relay(3, RELAY_3, STATE_RELAY_3);
        //(STATE_RELAY_1 == false) ? digitalWrite(RELAY_1, HIGH) : digitalWrite(RELAY_1, LOW);
        //param->updateAndReport(val);
      }
    }
}

 
/****************************************************************************************************
 * setup Function
*****************************************************************************************************/
void setup(){
  //------------------------------------------------------------------------------
  uint32_t chipId = 0;
  Serial.begin(115200);
   
  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  
  //------------------------------------------------------------------------------
  // Set the Relays GPIOs as output mode
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT); 
  pinMode(RELAY_3, OUTPUT); 


  //Beginning Sensor
  dht.begin();
  
  //------------------------------------------------------------------------------
   
  //------------------------------------------------------------------------------
  pinMode(gpio_reset, INPUT);
  pinMode(WIFI_LED, OUTPUT);
  digitalWrite(WIFI_LED, LOW);
  //------------------------------------------------------------------------------
  // Write to the GPIOs the default state on booting
  digitalWrite(RELAY_1, !STATE_RELAY_1);
  digitalWrite(RELAY_2, !STATE_RELAY_2); 
  digitalWrite(RELAY_3, !STATE_RELAY_3); 

  //------------------------------------------------------------------------------
  Node my_node;    
  my_node = RMaker.initNode("ThaiHiep");
  //------------------------------------------------------------------------------
  //Standard switch device
  my_switch1.addCb(write_callback);
  my_switch2.addCb(write_callback); 
  my_switch3.addCb(write_callback); 

  //------------------------------------------------------------------------------
  //Add switch device to the node   
  my_node.addDevice(my_switch1);
  my_node.addDevice(my_switch2);
  my_node.addDevice(my_switch3);
  my_node.addDevice(temperature);
  my_node.addDevice(humidity);
  //------------------------------------------------------------------------------
  //This is optional 
  RMaker.enableOTA(OTA_USING_PARAMS);
  RMaker.setTimeZone("Asia/Ho_Chi_Minh");
  //If you want to enable scheduling, set time zone for your region using setTimeZone(). 
  //The list of available values are provided here https://rainmaker.espressif.com/docs/time-service.html
  // RMaker.setTimeZone("Asia/Shanghai");
  // Alternatively, enable the Timezone service and let the phone apps set the appropriate timezone
  RMaker.enableTZService();
  RMaker.enableSchedule();
  //------------------------------------------------------------------------------
  // Timer for Sending Sensor's Data
  Timer.setInterval(3000);
  //Service Name
  for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  Serial.printf("\nChip ID:  %d Service Name: %s\n", chipId, service_name);
  //------------------------------------------------------------------------------
  Serial.printf("\nStarting ESP-RainMaker\n");
  RMaker.start();
  //------------------------------------------------------------------------------
  WiFi.onEvent(sysProvEvent);
  #if CONFIG_IDF_TARGET_ESP32
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
  #else
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
  #endif
  //------------------------------------------------------------------------------
  STATE_RELAY_1 = EEPROM.read(0);
  STATE_RELAY_2 = EEPROM.read(0); 
  STATE_RELAY_3 = EEPROM.read(0); 

  
  digitalWrite(RELAY_1, STATE_RELAY_1);
  digitalWrite(RELAY_2, STATE_RELAY_2); 
  digitalWrite(RELAY_3, STATE_RELAY_3); 

  
  my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, STATE_RELAY_1); 
  my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, STATE_RELAY_2); 
  my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, STATE_RELAY_3); 

  Serial.printf("Relay1 is %s \n", STATE_RELAY_1? "ON" : "OFF");
  Serial.printf("Relay2 is %s \n", STATE_RELAY_2? "ON" : "OFF"); 
  Serial.printf("Relay3 is %s \n", STATE_RELAY_3? "ON" : "OFF"); 

  //------------------------------------------------------------------------------
}

/****************************************************************************************************
 * loop Function
*****************************************************************************************************/
void loop()
{
  //------------------------------------------------------------------------------
    // if (Timer.isReady() && wifi_connected) {                    // Check is ready a second timer
    Serial.println("Sending Sensor's Data");
    Send_Sensor();
    delay(2000);
    // Timer.reset();                        // Reset a second timer
  // }
  //-----------------------------------------------------------  Logic to Reset RainMaker
  // Read GPIO0 (external button to reset device
  if(digitalRead(gpio_reset) == LOW) { //Push button pressed
    Serial.printf("Reset Button Pressed!\n");
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while(digitalRead(gpio_reset) == LOW) delay(50);
    int endTime = millis();
    //_______________________________________________________________________
    if ((endTime - startTime) > 10000) {
      // If key pressed for more than 10secs, reset all
      Serial.printf("Reset to factory.\n");
      wifi_connected = 0;
      RMakerFactoryReset(2);
    } 
    //_______________________________________________________________________
    else if ((endTime - startTime) > 3000) {
      Serial.printf("Reset Wi-Fi.\n");
      wifi_connected = 0;
      // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
      RMakerWiFiReset(2);
    }
    //_______________________________________________________________________
  }
  //------------------------------------------------------------------------------
  delay(100);
  
  if (WiFi.status() != WL_CONNECTED){
    //Serial.println("WiFi Not Connected");
    digitalWrite(WIFI_LED, LOW);
  }
  else{
    //Serial.println("WiFi Connected");
    digitalWrite(WIFI_LED, HIGH);
  }
  //------------------------------------------------------------------------------
  //button_control();
  //remote_control();
}
/****************************************************************************************************
 * Sensor DHT Function
*****************************************************************************************************/
void Send_Sensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Temperature - "); Serial.println(t);
  Serial.print("Humidity - "); Serial.println(h);

  temperature.updateAndReportParam("Temperature", t);
  humidity.updateAndReportParam("Temperature", h);
}

/****************************************************************************************************
 * control_relay Function
*****************************************************************************************************/
void control_relay(int relay_no, int relay_pin, boolean &status){
  status = !status;
  digitalWrite(relay_pin, status);
  EEPROM.write(relay_no-1, status);
  EEPROM.commit();
  String text = (status)? "ON" : "OFF";
  Serial.println("Relay"+String(relay_no)+" is "+text);
}
 
