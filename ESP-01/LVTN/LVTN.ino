#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

const char* ssid = "Dino-IP";
const char* password = "iloveyou";

//Your Domain name with URL path or IP address with path
String serverName = "http://172.20.10.14:8000/api/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (2000)
unsigned long timerDelay = 2000;

// Status = 0 --> Chưa có code & password
// Status = 1 --> Có code & password, chưa login (chưa token)
// Status = 2 --> Đã login (có token)
char Status;
// Rx_data --> Lấy data từ uart
String Rx_data;
// Token --> Token access
String Token;
// code & password --> tài khoản và mật khẩu đăng nhập
char code[10];
char pass[10];
//char tem[5];
//char vol[5];
//char cur[5];
//char pf[5];
String tem;
String vol;
String cur;
String pf;
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  WiFiClient client;
  HTTPClient http;
  Serial.println("Waiting for me!");
}

void loop() {
  // Status = Chưa có code&password, lấy code&password qua Serial
  if(Status == 0){
    if(Serial.available()){
      while(Serial.available()){
        Rx_data = Serial.readString();
        for(int i=0; i<7; i++){
          code[i] = Rx_data[i];
        }
        for(int i=8; i<Rx_data.length(); i++){
          pass[i-8] = Rx_data[i];
        }
        Status = 1;
      }
    }
  }
  // Status = Đã có code&password, thực hiện post lấy token
  if(Status == 1){
    WiFiClient client;
    HTTPClient http;
    
    String serverPath = serverName + "token/";
    http.begin(client, serverPath);
    http.addHeader("Content-Type", "application/json");
    String httpRequestData = "{\"password\":\""+String(pass)+"\",\"code\":\""+String(code)+"\"}";
    int httpResponseCode = http.POST(httpRequestData);
    //Check Response Status & get json data
    String payload = {};
    if(httpResponseCode == 200){
      payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      
      auto access = doc["access"].as<String>();
      Token = access;
      // httpCode will be negative on error
      if(Token != "null" && Token != ""){
        Serial.write("L");
        Status = 2;
      }
    }
    http.end();
  }
  //
  if(Status == 2){
    tem.remove(0);
    vol.remove(0);
    cur.remove(0);
    pf.remove(0);
    if(Serial.available()){
      while(Serial.available()){
        Rx_data = Serial.readString();
        char state = 0;
        char index = 0;
        for(int i=0; i<Rx_data.length(); i++){
          if(Rx_data[i] == '&'){
            state++;
            index = i+1;
          }
          else {
            switch(state) {
              case 0:
                tem += Rx_data[i];
                break;
              case 1:
                vol += Rx_data[i];
                break;
              case 2:
                cur += Rx_data[i];
                break;
              case 3:
                pf += Rx_data[i];
                break;
            }
          }
        }
        // Status = Đã login và timer 2s
        if((millis() - lastTime) > timerDelay){
          WiFiClient client;
          HTTPClient http;
          String serverPath = serverName + "update-monitoring";
          http.begin(client, serverPath);
          http.addHeader("Authorization", "Bearer " + Token);
          http.addHeader("Content-Type", "application/json");

          String httpRequestData = "{\"temperature\":\""+tem+"\",\"voltage\":\""+vol+"\",\"current\":\""+cur+"\",\"PF\":\""+pf+"\",\"vibration\":\"0\",\"device_id\":\"81f1aa0f-8fe3-4f01-b878-f4566e8d647e\"}";
          int httpCode = http.POST(httpRequestData);
          http.end();
          lastTime = millis();
        }
      }
    }
  }
}
