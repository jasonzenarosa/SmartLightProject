#include <WiFi.h>
#include <HttpClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <Adafruit_CAP1188.h>

const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";

const char sunset_API[] = "api.sunrise-sunset.org";
char *sunset_API_path;

const char geolocation_API[] = "ip-api.com";
const char geolocation_API_path[] = "/json";

const char* ntpServer0 = "0.north-america.pool.ntp.org";
const char* ntpServer1 = "1.north-america.pool.ntp.org";
const char* ntpServer2 = "2.north-america.pool.ntp.org";
const long  gmtOffset_sec = -28800;
const int   daylightOffset_sec = 3600;


const char cloudAPI[] = "ec2-3-15-204-116.us-east-2.compute.amazonaws.com";

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30 * 1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

// Declare helper functions
void sendLightRequest(bool lightsOn);
void sendLog(int lightsOn, String method);
uint8_t getTouchCount(uint8_t touched);
void getLocalTime();
String convert24HourTo12Hour(String time24);
void printLocalTime();

// Declare global variables
String latitude;
String longitude;
String timezone;
String sunsetTime;
String sunriseTime;
char localTime[8];
bool lightsOn = false;

// Special Classes for Wifi, HTTP, & Sensor 
WiFiClient wifi_client;
HttpClient http_client(wifi_client);
Adafruit_CAP1188 cap = Adafruit_CAP1188();

void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // Use a geolocation service to get the latitude and longitude
  int err = http_client.get(geolocation_API, geolocation_API_path);

  if (err == 0)
  {
    Serial.println("started request ok");

    err = http_client.responseStatusCode();
    if (err < 0)
    {
      Serial.print("HTTP request failed with error ");
      Serial.println(err);
    }
    else
    {
      http_client.skipResponseHeaders();
      String payload = http_client.readString();
      //Serial.println("Payload:\n" + payload);
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, payload);

      String temp_latitude = doc["lat"];
      String temp_longitude = doc["lon"];
      String temp_timezone = doc["timezone"];

      Serial.println("Latitude: " + temp_latitude);
      Serial.println("Longitude: " + temp_longitude);
      Serial.println("Timezone: " + temp_timezone);

      latitude = temp_latitude;
      longitude = temp_longitude;
      timezone = temp_timezone;

      String temp = "/json?lat=" + latitude + "&lng=" + longitude + "&date=today&tzid=" + timezone;
      //Serial.println(temp);
      sunset_API_path = strdup(temp.c_str());
    }
  }
  else
  {
    Serial.println("Error on HTTP request #1");
  }
  http_client.stop();

  int err2 = http_client.get(sunset_API, sunset_API_path);
  if (err2 == 0)
  {
    Serial.println("started request #2 ok");
    err2 = http_client.responseStatusCode();
    if (err2 < 0)
    {
      Serial.print("HTTP request failed with error ");
      Serial.println(err2); 
    }
    else
    {
      
      http_client.skipResponseHeaders();
      String payload = http_client.readString();

      // Clean payload from the HTTP response & Parse the JSON response
      int first_bracket = payload.indexOf("{");
      int last_bracket = payload.lastIndexOf("}");

      String clean_payload;
      DynamicJsonDocument doc(2048);

      if(first_bracket != -1 && last_bracket != -1) {
        String cleanPayload = payload.substring(first_bracket, last_bracket + 1);
        //Serial.println("Payload:\n" + cleanPayload);

        DeserializationError error = deserializeJson(doc, cleanPayload);

        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
      }
     

      String sunsetTemp = doc["results"]["sunset"];
      sunsetTime = sunsetTemp;
      String sunriseTemp = doc["results"]["sunrise"];
      sunriseTime = sunriseTemp;


      Serial.println("Sunset at: " + sunsetTime);
      Serial.println("Sunrise at: " + sunriseTime);
    }
  }
  else{
    Serial.println("Error on HTTP request #2");
  }
  http_client.stop();

  if (!cap.begin()) {
    Serial.println("CAP1188 not found");
  }
  Serial.println("CAP1188 found!");
}

void loop()
{
  // Obtain the current time every 1 second
  getLocalTime(); //places into local time variable 
  String localTimeStr = localTime;

  // Sensor Reading
  uint8_t touched = cap.touched();
  uint8_t touchCount = getTouchCount(touched);

  // Lights Start at the Off State & Turns on at Sunset & Turns off at Sunrise
  if(localTimeStr >= sunsetTime && lightsOn == false){
    Serial.println("Turning on Lights at Sunset");
    sendLightRequest(true);
    lightsOn = true;
  }
  else if(localTimeStr >= sunriseTime && lightsOn == true){
    Serial.println("Turning off Lights at Sunrise");
    sendLightRequest(false);
    sendLog(0, "sunrise");
    lightsOn = false;
  }
  else if(touchCount >= 2 && touchCount <= 3){
    Serial.println("Turning on Lights via Touch");
    sendLightRequest(true);
    sendLog(1, "touch");
    lightsOn = true;
  }
  else if(touchCount >= 4){
    Serial.println("Turning off Lights via Touch");
    sendLightRequest(false);
    sendLog(0, "touch");
    lightsOn = false;
  }  
  
}

void sendLightRequest(bool lightsOn){
  if(lightsOn){
    http_client.get("13.52.247.21", 5000, "lights-on");
    http_client.stop();
  }
  else{
    http_client.get("13.52.247.21", 5000, "lights-off");
    http_client.stop();
  }
}


void sendLog(int lightsOn, String method){
  int err;
  if(method == "sunset"){
    err = http_client.post("13.52.247.21", 5000, "/create-log?light-on=1&method=sunset");
  }
  else if (method == "sunrise")
  {
    err = http_client.post("13.52.247.21", 5000, "/create-log?light-on=0&method=sunrise");
  }
  else if(method == "touch"){
    if(lightsOn == 1){
      err = http_client.post("13.52.247.21", 5000, "/create-log?light-on=1&method=touch");
    }
    else{
      err = http_client.post("13.52.247.21", 5000, "/create-log?light-on=0&method=touch");
      
    }
  }
    if(err == 0){
        Serial.println("Log Created ");
        Serial.println(http_client.responseStatusCode());
      }
      else{
        Serial.println("Error on Log Creation");
      }
    http_client.stop();
}

uint8_t getTouchCount(uint8_t touched){
  uint8_t touchCount = 0;
  for (uint8_t i=0; i<8; i++) {
    if (touched & (1 << i)) {
      ++touchCount; 
      Serial.print("C"); Serial.print(i+1);Serial.println("");
    }
  }
  delay(50);
  return touchCount;
}

void getLocalTime(){
  
  // Config Time via NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer0,ntpServer1, ntpServer2);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  sprintf(localTime, "%d:%d:%d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  sleep(1);
}


void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");
}


// Function to convert 24-hour format string to 12-hour format string with AM/PM
String convert24HourTo12Hour(String time24) {
    int hour24, minute;
    int hour12;
    String period;
    String timeString;

    // Parse the input string to get hours and minutes
    hour24 = time24.substring(0, 2).toInt();
    minute = time24.substring(3, 5).toInt();

    // Determine if it's AM or PM
    if (hour24 >= 12) {
        period = "PM";
        if (hour24 > 12) {
            hour12 = hour24 - 12;
        } else {
            hour12 = hour24;
        }
    } else {
        period = "AM";
        if (hour24 == 0) {
            hour12 = 12;
        } else {
            hour12 = hour24;
        }
    }

    // Format the time string
    if (minute < 10) {
        timeString = String(hour12) + ":0" + String(minute) + " " + period;
    } else {
        timeString = String(hour12) + ":" + String(minute) + " " + period;
    }

    return timeString;
}

