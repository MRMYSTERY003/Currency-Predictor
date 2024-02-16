#include "esp_camera.h"
#include <WiFi.h>
#include "img_converters.h"
#include "Arduino.h"
#include "Base64.h"  // Include the Base64 library
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
#include <HTTPClient.h>
// const char* serverUrl = "https://grabber-f4c9a-default-rtdb.firebaseio.com/.json";
const char* serverUrl = "http://192.168.1.4:5000/upload_image";
// const char* serverUrl = "https://currency-prediction-0zy9.onrender.com/upload_image";


// ... (rest of your code)

const char* ssid = "agustin";
const char* password = "12345678";

int btn = 16;

int led = 4;
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  // pinMode(led, OUTPUT);
  // pinMode(btn, INPUT_PULLUP);
  // digitalWrite(led, LOW);
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  
  camera_config_t config;

 config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Camera init

    esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }


  // Wi-Fi connection



  WiFi.begin(ssid, password);
  Serial.println("Wifi:0");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }


  Serial.println("Wifi:1");



  

  // Capture image

  // capture();
  // Close camera
  // 
}



void makePutRequest(String data) {
  HTTPClient http;

  // Specify the target URL and the method (PUT)
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");  // Adjust content type as needed

  // Your JSON payload for the PUT request
  String jsonPayload = "{\"img\": \"" + data + "\"}";
  // Serial.println(jsonPayload);

  // Send the PUT request with the payload
  int httpCode = http.POST(jsonPayload);

  // Check the HTTP response code
  if (httpCode > 0) {
    // Serial.printf("[HTTP] PUT request status code: %d\n", httpCode);
    delay(100);

    // Read the response
    String response = http.getString();
    Serial.print(response);
  } else {
    Serial.printf("[HTTP] PUT request failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  // Close the connection
  http.end();
}


void capture(){
  // digitalWrite(led, HIGH);

  delay(100);
  for(int i = 0; i < 15; i++){
  camera_fb_t *fb = esp_camera_fb_get();
  esp_camera_fb_return(fb);
  delay(100);

  }
  camera_fb_t *fb = esp_camera_fb_get();

  if (fb) {
    // Encode image data to Base64
    String base64Image = base64::encode(fb->buf, fb->len);

    // // Print Base64 data to Serial Monitor
    // Serial.println("Base64 Encoded Image:");
    // Serial.println(base64Image);
    // digitalWrite(led, LOW);
    makePutRequest(base64Image);

    esp_camera_fb_return(fb);
    // Serial.println("Image captured and Base64 printed.");
  } else {
    Serial.println("Failed to capture image.");
  }
  // esp_camera_deinit();
}


void loop(){
  // if(digitalRead(btn) == 0){
  //   capture();
  // }
    if (Serial.available()) {
      char sendChar = Serial.read();
      // Serial.println(sendChar);
      Serial.flush();
      if(sendChar == 'c'){
        capture();
      }
  }
}
