#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"

MAX30105 particleSensor;
BLEServer* pServer = NULL;
BLECharacteristic* pSensorCharacteristic = NULL;
BLECharacteristic* pLedCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

//MAX30105 Variables
//==========================

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
String Gmaps_link;

float beatsPerMinute;
int beatAvg;
int beatThreshhold = 70;

const int ledPin = 2; // Use the appropriate GPIO pin for your setup




String SOS_NUM = "+254702139261"; // Add a number on which you want to receive call or SMS

// Necessary Variables
boolean stringComplete = false;
String inputString = "";
String fromGSM = "";
bool CALL_END = 1;
char* response = " ";
String res = "";
int c = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID       "4fafc201-1fb5-459e-8fcc-c5c9c331914b" //"19b10000-e8f2-537e-4f6c-d104768a1214"
#define SENSOR_CHARACTERISTIC_UUID "beefcafe-36e1-4688-b7f5-00000000000b" //"19b10001-e8f2-537e-4f6c-d104768a1214"
#define LED_CHARACTERISTIC_UUID "19b10002-e8f2-537e-4f6c-d104768a1214"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pLedCharacteristic) {
      std::string value = pLedCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print(F("Characteristic event, written: "));
        Serial.println(static_cast<int>(value[0])); // Print the integer value

        int receivedValue = static_cast<int>(value[0]);
        if (receivedValue == 1) {
          digitalWrite(ledPin, HIGH);
        } else {
          digitalWrite(ledPin, LOW);
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println(F("Initializing..."));
  Serial1.begin(115200, SERIAL_8N1, 16, 17); // For A9G Board
  // Waiting for A9G to setup everything for 20 sec
  delay(20000);

  //  digitalWrite(SLEEP_PIN, LOW); // Sleep Mode OFF

  Serial1.println("AT");               // Just Checking
  delay(1000);

  Serial1.println("AT+GPS = 1");      // Turning ON GPS
  delay(1000);

  Serial1.println("AT+GPSLP = 2");      // GPS low power
  delay(1000);

  Serial1.println("AT+SLEEP = 1");    // Configuring Sleep Mode to 1
  delay(1000);

  Serial1.println("AT+CMGF = 1");
  delay(1000);

  Serial1.println("AT+CSMP  = 17,167,0,0 ");
  delay(1000);

  Serial1.println("AT+CPMS = \"SM\",\"ME\",\"SM\" ");
  delay(1000);

  Serial1.println("AT+SNFS = 2");
  delay(1000);

  Serial1.println("AT+CLVL = 8");
  delay(1000);


  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power. "));
    while (1);
  }
  Serial.println(F("Place your index finger on the sensor with steady pressure."));

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  pinMode(ledPin, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pSensorCharacteristic = pService->createCharacteristic(
                            SENSOR_CHARACTERISTIC_UUID,
                            BLECharacteristic::PROPERTY_READ   |
                            BLECharacteristic::PROPERTY_WRITE  |
                            BLECharacteristic::PROPERTY_NOTIFY |
                            BLECharacteristic::PROPERTY_INDICATE
                          );

  // Create the ON button Characteristic
  pLedCharacteristic = pService->createCharacteristic(
                         LED_CHARACTERISTIC_UUID,
                         BLECharacteristic::PROPERTY_WRITE
                       );

  // Register the callback for the ON button characteristic
  pLedCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pSensorCharacteristic->addDescriptor(new BLE2902());
  pLedCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println(F("Waiting a client connection to notify..."));

}

void loop() {
  // notify changed value
  if (deviceConnected) {

    {
      //listen from GSM Module
      if (Serial1.available())
      {
        char inChar = Serial1.read();

        if (inChar == '\n') {

          //check the state
          if (fromGSM == "SEND LOCATION\r")
          {
            Get_gmap_link(1);  // Send Location with Call
            //          digitalWrite(SLEEP_PIN, HIGH);// Sleep Mode ON

          }

          else if (fromGSM == "RING\r")
          {
            //          digitalWrite(SLEEP_PIN, LOW); // Sleep Mode OFF
            Get_gmap_link(1);
            Serial.println(F("---------ITS RINGING-------"));
            Serial1.println("ATA");
          }

          else if (fromGSM == "NO CARRIER\r")
          {
            Serial.println(F("---------CALL ENDS-------"));
            CALL_END = 1;
            //          digitalWrite(SLEEP_PIN, HIGH);// Sleep Mode ON
          }
          else if (beatThreshhold == beatAvg)
          {
            Get_gmap_link(1);  // Send Location with Call
            //            Serial.println("---------CALL ENDS-------");
            //            CALL_END = 1;
          }


          //write the actual response
          Serial.println(fromGSM);
          //clear the buffer
          fromGSM = "";

        }
        else
        {
          fromGSM += inChar;
        }
        delay(20);
      }

      // read from port 0, send to port 1:
      if (Serial.available()) {
        int inByte = Serial.read();
        Serial1.write(inByte);
      }

    }

    long irValue = particleSensor.getIR();

    if (checkForBeat(irValue) == true)
    {
      //We sensed a beat!
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20)
      {
        rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
        rateSpot %= RATE_SIZE; //Wrap variable

        //Take average of readings
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }

    Serial.print(F("IR="));
    Serial.print(irValue);
    Serial.print(F(", BPM="));
    Serial.print(beatsPerMinute);
    Serial.print(F(", Avg BPM="));
    Serial.print(beatAvg);
    pSensorCharacteristic->setValue(String(beatAvg).c_str());
    pSensorCharacteristic->notify();
    //        value++;
    Serial.print(F("New value notified: "));
    Serial.println(beatAvg);
    //    delay(3000); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms


    if (irValue < 50000)
      Serial.print(F(" No finger?"));

    Serial.println();
  }
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    Serial.println(F("Device disconnected."));
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println(F("Start advertising"));
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
    Serial.println(F("Device Connected"));
  }
}

//---------------------------------------------  Getting Location and making Google Maps link of it. Also making call if needed
void Get_gmap_link(bool makeCall)
{


  //  digitalWrite(SLEEP_PIN, LOW);
  delay(1000);
  Serial1.println("AT+LOCATION = 2");
  Serial.println("AT+LOCATION = 2");

  while (!Serial1.available());
  while (Serial1.available())
  {
    char add = Serial1.read();
    res = res + add;
    delay(1);
  }

  res = res.substring(17, 38);
  response = &res[0];

  Serial.print(F("Recevied Data - ")); Serial.println(response); // printin the String in lower character form
  Serial.println("\n");

  if (strstr(response, "GPS NOT"))
  {
    Serial.println(F("No Location data"));
    //------------------------------------- Sending SMS without any location
    Serial1.println("AT+CMGF=1");
    delay(1000);
    Serial1.println("AT+CMGS=\"" + SOS_NUM + "\"\r");
    delay(1000);

    Serial1.println (F("Unable to fetch location. Please try again"));
    delay(1000);
    Serial1.println((char)26);
    delay(1000);
  }
  else
  {

    int i = 0;
    while (response[i] != ',')
      i++;

    String location = (String)response;
    String lat = location.substring(2, i);
    String longi = location.substring(i + 1);
    Serial.println(lat);
    Serial.println(longi);

    String Gmaps_link = ( "https://www.google.com/maps?q=" + lat + "," + longi); //http://maps.google.com/maps?q=38.9419+-78.3020
    //------------------------------------- Sending SMS with Google Maps Link with our Location
    Serial.println(Gmaps_link);

    Serial1.println("AT+CMGF=1");
    delay(1000);
    Serial1.println("AT+CMGS=\"" + SOS_NUM + "\"\r");
    delay(1000);

    Serial1.println ("I'm here " + Gmaps_link);
    delay(1000);
    Serial1.println((char)26);
    delay(1000);

    Serial1.println("AT+CMGD=1,4"); // delete stored SMS to save memory
    delay(5000);
  }
  response = "";
  res = "";
  if (makeCall)
  {
    Serial.println(F("Calling Now"));
    Serial1.println("ATD" + SOS_NUM);
    CALL_END = 0;
  }
}
