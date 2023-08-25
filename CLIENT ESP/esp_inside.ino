#include "esp32-hal-ledc.h" // библиотека ШИМа
#include <ESP32Servo.h>
#include <WiFi.h>

#define FAN_COLLECTOR_PIN 16
#define SERVO_PIN 17
#define TERM_PIN 34

//частота ШИМ
#define SERVO_FREQUENCY 50 // стандартная частота для серво
#define FAN_FREQUENCY 30000
//канал, на который назначим в дальнейшем ШИМ (от 0 до 16) (или до 15)
#define PWM_FAN_CHANNEL 10
//разрешение ШИМа (в битах)    
#define RESOLUTION 8 // при 8 2^8 = 256 (ШИМ сигнал от 0 до 256)

const char* ssid     = "rasppi";
const char* password = "";
const char* host = "10.42.0.1"; //"192.168.88.219"
const int port = 6969;

Servo myservo;  // create servo object to control a servo
float prev_temp = 21.34;



void changeFanSpeed(int power);
float getTemp();


void setup() {
  // Для того, чтобы ШИМ использовали разные таймеры
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  Serial.begin(115200);
  
  ledcSetup(PWM_FAN_CHANNEL, FAN_FREQUENCY, RESOLUTION);
  ledcAttachPin(FAN_COLLECTOR_PIN, PWM_FAN_CHANNEL);

  myservo.setPeriodHertz(SERVO_FREQUENCY); 
  myservo.attach(SERVO_PIN); // attaches the servo on pin 18 to the servo object

  ///////////////////////////////////////////////////////////////////
  Serial.print("Connecting to ");
  Serial.println(ssid);
  /* connect to your WiFi */
  WiFi.begin(ssid, password);
  /* wait until ESP32 connect to WiFi*/
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected with IP address: ");
  Serial.println(WiFi.localIP());  
  ////////////////////////////////////////////////////////////////////
  //changeFanSpeed(100);
  myservo.write(0);
}

void loop() {
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(1000);
    return;
  }

  client.print(String(getTemp()));

  String inp = client.readString();
  Serial.println(inp);
  inp.trim();
  if (inp[1] == '0') {
    myservo.write(0);
  } else if (inp[1] == '1') {
    myservo.write(90);
  }
  changeFanSpeed(inp.substring(4).toInt());

  //delay(500);
  
  client.stop();
}



void changeFanSpeed(int power) { // Должна получить значение силы в процентах
  if (power == 0) {
    ledcWrite(PWM_FAN_CHANNEL, 0);
  } else {
    int max_power = pow(2, RESOLUTION) - 1;
    ledcWrite(PWM_FAN_CHANNEL, max_power); // максимальная мощность для запуска
    delay(400);
    ledcWrite(PWM_FAN_CHANNEL, map(power, 1, 100, max_power / 1.8, max_power)); // для более корректной работы
  }
}

float getTemp() {
  float sum = 0;
  int count = 0;
  while (count <= 20) {
    float temp = analogRead(TERM_PIN);
    temp = temp/14;
    temp -= 244;
    if ((temp < prev_temp + 3) and (temp > prev_temp - 3)) {
      sum += temp;
      count++;
    }
    delay(2); 
  }
  float now_temp = sum/count;
  prev_temp = now_temp;
  Serial.println(now_temp);
  Serial.println(prev_temp);
  return prev_temp;
  // if (now_temp > prev_temp) {
  //   if (round(prev_temp) < 30) {      
  //     prev_temp = min(round(now_temp * 100), round(float(prev_temp + 0.63) * 100)) / 100;
  //     Serial.print("1: ");
  //     Serial.println(prev_temp);
  //     return prev_temp;
  //   }
  // } else {      
  //   prev_temp = max(round(now_temp * 100), round(float(prev_temp - 0.63) * 100)) / 100;
  //   Serial.println(prev_temp);
  //   return prev_temp;
  // }
}