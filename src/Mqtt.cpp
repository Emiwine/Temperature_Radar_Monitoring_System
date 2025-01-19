#include "Mqtt.h"
#include "eepromFile.h"
#include "variable.h"
#include "deviceControl.h"
#include <PubSubClient.h>
#include <Adafruit_AHTX0.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "otaControl.h"
#include "network.h"

unsigned long timeoutDuration = 1 * 60 * 1000;
unsigned long lastPublishTime = 0;
WiFiClient espClient;
PubSubClient client(espClient);

const char *mqttServer = "epvi-mqtt.com";
const int mqttPort = 1883;
const char *mqttUser = "test_devices";
const char *mqttPassword = "JKDNjkndkndkJNdckJNKAJnajkfnafuneoufnalfkanFLKAFOINOFIn";

const char *deviceTopic = "device/";
const char *alertTopic = "alert/";
const char *dataTopic = "info/";
const char *publishTopic = "publish/";

void initMqtt()
{
    if (isWifiConnected())
    {
        client.setServer(mqttServer, mqttPort);
        client.setCallback(callback);
        client.setKeepAlive(40);
        delay(1000);
    }
}

void reconnect()
{
    unsigned long startTime = millis();
    if (isWifiConnected())
    {
        while (!client.connected())
        {
            Serial.println(F("Attempting MQTT connection..."));
            // Attempt to connect
            String deviceId_str = readStringFromEEPROM(DEVICEID_ADDR);
            String devicemac_string = WiFi.macAddress();
            String subTopic = Subtopic();
            Serial.println("subTopic : " + subTopic);
            if (client.connect(devicemac_string.c_str(), mqttUser, mqttPassword))
            {
                digitalWrite(2, HIGH);              // for turning on the leds
                client.subscribe(subTopic.c_str()); // Subscribe to device topics
                Serial.println(F("connected to MQTT server"));
            }
            else
            {
                Serial.print("failed, rc=");
                Serial.print(client.state());
                Serial.println(" try again in 5 seconds");
                digitalWrite(2, LOW);
                delay(5000); // Wait 5 seconds before retrying
            }
            if (millis() - startTime >= timeoutDuration)
            {
                Serial.println("Max retry attempts reached or timeout exceeded. Exiting retry loop.");
                break;
            }
            delay(500);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{ //// this is my function to listen to our subtopic
    Serial.print("Received [");
    Serial.print(topic);
    Serial.print("]: ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, payload);

    if (!error)
    {
        // Check if the message contains the "deviceid"
        const char *receivedDeviceId = doc["deviceid"];

        // Log whether the message is for the current device
        String Deviceid12 = readStringFromEEPROM(DEVICEID_ADDR);
        if (String(receivedDeviceId) == Deviceid12)
        {
            Serial.println("Data is for me.");

            const char *command = doc["command"];

            if (String(command) == "enquireip")
            {
                StaticJsonDocument<256> jsonDoc;
                jsonDoc["mac_address"] = String(ESP.getEfuseMac(), HEX);
                jsonDoc["wifi"] = readStringFromEEPROM(SSID_ADDR);
                jsonDoc["password"] = readStringFromEEPROM(PASSWORD_ADDR);
                jsonDoc["d_group"] = readStringFromEEPROM(GROUPID_ADDR);
                jsonDoc["deviceid"] = readStringFromEEPROM(DEVICEID_ADDR);
                jsonDoc["mfcode"] = readStringFromEEPROM(MFCODE_ADDR);
                jsonDoc["mqttClientId"] = WiFi.macAddress();
                String response;
                serializeJson(jsonDoc, response);

                String alertTopicFull = String(alertTopic) + readStringFromEEPROM(DEVICEID_ADDR);
                Serial.println("alertTopicFull" + alertTopicFull);

                client.publish(alertTopicFull.c_str(), response.c_str());
                Serial.println("Response sent: " + response);
            }
            else if (String(command) == "restart")
            {
                shouldRestart = true;
            }
            else if (String(command) == "ota")
            {
                const String url = doc["otaurl"];
                checkAndUpdateFirmware(url, currentVersion);
            }
            else if (String(command) == "deviceid")
            {
                String deviceid = doc["deviceid"];
                saveToEEPROM(DEVICEID_ADDR, deviceid);
                delay(100);
                shouldRestart = true;
            }
            else
            {
                Serial.println(F("invalid Mqtt command"));
            }
            // }
        }
        else
        {
            Serial.println("Data is not for me.");
        }
    }
    else
    {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
    }
}

void mqttLoop()
{
    if (!client.connected() && isWifiConnected())
    {
        Serial.println("disconnected try to connecting");
        initMqtt();
        reconnect();
    }
    client.loop();
}

bool publishTemperatureData()
{
    if (!aht.begin())
    {
        Serial.println("AHT sensor initialization failed!");
        return false;
    }

    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

    if (isnan(temp.temperature))
    {
        corruptedDataAlert();
        Serial.println("Sensor data is corrupted.");
        return false;
    }

    String deviceId = readStringFromEEPROM(DEVICEID_ADDR);
    StaticJsonDocument<256> jsonDoc;
    jsonDoc["deviceid"] = deviceId;
    jsonDoc["temperature_C"] = instantTemperature();
    jsonDoc["data"] = "live";

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    String publishTopic = "temperature/" + deviceId;
    bool publishStatus = client.publish(publishTopic.c_str(), jsonString.c_str());
    Serial.println("Temperature data published: " + String(publishStatus));
    return publishStatus;
}

void corruptedDataAlert()
{
    String deviceId_str = readStringFromEEPROM(DEVICEID_ADDR);
    StaticJsonDocument<256> jsonDoc; // Adjust size as needed
    // Add the device ID to the JSON
    jsonDoc["deviceid"] = deviceId_str.c_str(); // Add device ID
    jsonDoc["Temperature C"] = "corrupted Data";
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    String publish = "TemperatureAlert/" + deviceId_str;
    if (client.publish(publish.c_str(), jsonString.c_str()))
    {
        Serial.println("Sensor alert is published ");
    }
}

bool publishPresenceData()
{
    String deviceId = readStringFromEEPROM(DEVICEID_ADDR);
    if (deviceId.isEmpty())
    {
        Serial.println("Error: Device ID is empty!");
        return false;
    }

    StaticJsonDocument<256> jsonDoc;
    jsonDoc["deviceid"] = deviceId;
    jsonDoc["stationary_distance_cm"] = stat_target;
    jsonDoc["moving_distance_cm"] = mov_tgt;
    jsonDoc["data"] = "live";

    String jsonString;
    if (serializeJson(jsonDoc, jsonString) == 0)
    {
        Serial.println("Error: Failed to serialize JSON!");
        return false;
    }

    String publishTopic = "presence/" + deviceId;
    bool publishStatus = client.publish(publishTopic.c_str(), jsonString.c_str());
    if (publishStatus)
    {
        Serial.println("Presence data published successfully to topic: " + publishTopic);
        mov_tgt = 0;
    }
    else
    {
        Serial.println("Error: Failed to publish presence data!");
    }
    return publishStatus;
}

void corruptedPresenceAlert()
{
    String deviceId = readStringFromEEPROM(DEVICEID_ADDR);
    StaticJsonDocument<256> jsonDoc;

    // Add device ID and alert message
    jsonDoc["deviceid"] = deviceId;
    jsonDoc["presence"] = no_target;

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    String publishTopic = "PresenceAlert/" + deviceId;
    if (client.publish(publishTopic.c_str(), jsonString.c_str()))
    {
        Serial.println("Presence alert published: No target detected");
    }
}
