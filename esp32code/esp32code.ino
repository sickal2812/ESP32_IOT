#include <AWS_IOT.h>
#include <stdio.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
AWS_IOT hornbill;
int is_changed = 0;

#define timeSeconds 3

int times = 0;
int pre_t = 0;
int del = 5000;

const char* ssid = "KT_GiGA_2G_B7E4"; // ###########
const char* password = "7dhe6ex175"; // ###########

const int trigPin = 22;
const int echoPin = 23;

int ledpin1 = 18;
int ledpin2 = 19;
int buttonPin = 15;
int ledPin3 = 16;
int buttonState = 0;
int ledPin4 = 17;
const int motionSensor = 36;

unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;

void IRAM_ATTR detectsMovement() {
  Serial.println("MOTION DETECTED!!!");
  digitalWrite(ledPin4, HIGH);
  startTimer = true;
  lastTrigger = millis();
}

char HOST_ADDRESS[] = "awskey-ats.iot.ap-northeast-2.amazonaws.com";

char CLIENT_ID[]= "smart_trash_bucket1"; // ## 1 = 안일훈 2 = 이지원 3 = 전예림 4 = 이효중 ##############
char sTOPIC_NAME[]= "$aws/things/smart_trash_bucket/shadow/update/delta"; // subscribe topic name
char pTOPIC_NAME[]= "$aws/things/smart_trash_bucket/shadow/update"; // publish topic name
int status = WL_IDLE_STATUS;
int msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];
unsigned long preMil = 0;

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload,payLoad,payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void setup() {
  bool status;
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin,INPUT);
  pinMode(ledPin3,OUTPUT);
  
  pinMode(motionSensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
  pinMode(ledPin4,OUTPUT);
  
  //++choi This is here to force the ESP32 to reset the WiFi and initialise correctly.
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode()); //++choi
  WiFi.begin(ssid, password);

  pinMode(ledpin1, OUTPUT);
  pinMode(ledpin2, OUTPUT);

  digitalWrite(ledpin1, LOW);
  digitalWrite(ledpin2, LOW);


  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to wifi");
  
  if(hornbill.connect(HOST_ADDRESS,CLIENT_ID)== 0) {
      Serial.println("Connected to AWS");
      delay(1000);
    if(0==hornbill.subscribe(sTOPIC_NAME,mySubCallBackHandler)) {
      Serial.println("Subscribe Successfull");
    } else {
    Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
    while(1);
    }
  }
  
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while(1);
  }

}

void loop() {
  buttonState = digitalRead(buttonPin);
  if(buttonState == HIGH) {
    digitalWrite(ledPin3,HIGH);
  } else {
    digitalWrite(ledPin3,LOW);
  }

  now = millis();
  if (startTimer && (now - lastTrigger > (timeSeconds*1000))) {
    Serial.println("Motion stopped...");
    digitalWrite(ledPin4, LOW);
    startTimer = false;
  }
  
  long duration, distance;
  digitalWrite(trigPin, LOW); // trig low for 2us
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // trig high for 10us
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  //getting duration for echo pulse
  duration = pulseIn(echoPin, HIGH);
  //sound speed = 340 m/s = 34/1000 cm/us
  //distance = duration * 34/1000 * 1/2
  distance = duration * 17 / 1000;
  //Serial.print(distance);
  //Serial.print("cm");
  //Serial.println();
  
  if(msgReceived == 1) {
  }

    times = millis();
  if(times - pre_t >= del) {
    pre_t = times;
    Serial.print(distance);
    Serial.println();
  
  if(distance>10) {
    digitalWrite(ledpin1, LOW);
    digitalWrite(ledpin2, HIGH);
    if(hornbill.publish(pTOPIC_NAME,payload) == 0) {
      Serial.print("Publish Message:");
      sprintf(payload, " {\"state\":{\"reported\":{\"1_state\": \"1notfull\", \"1_latitude\": \"37.701653\", \"1_longitude\": \"126.900892\", \"id\" : \"1\"}}}"); // ###########
      //sprintf(payload, " {\"state\":{\"reported\":{\"2_state\": \"2notfull\", \"2_latitude\": \"37.701653\", \"2_longitude\": \"126.900892\", \"id\" : \"2\"}}}");
      //sprintf(payload, " {\"state\":{\"reported\":{\"3_state\": \"3notfull\", \"3_latitude\": \"37.701653\", \"3_longitude\": \"126.900892\", \"id\" : \"3\"}}}");
      //sprintf(payload, " {\"state\":{\"reported\":{\"4_state\": \"4notfull\", \"4_latitude\": \"37.701653\", \"4_longitude\": \"126.900892\", \"id\" : \"4\"}}}");
      // 1 = 안일훈, 2 = 이지원 3 = 전예림 4 = 이효중
      // state#
      Serial.println(payload);
      // {"temp":temp, "humid" : humi, "press" : pres}
      //hornbill.publish(pTOPIC_NAME,payload);
    } else {
      Serial.println("Publish failed");
    }
  } else {
    digitalWrite(ledpin1, HIGH);
    digitalWrite(ledpin2, LOW);
    if(hornbill.publish(pTOPIC_NAME,payload) == 0) {
      Serial.print("Publish Message:");
      sprintf(payload, " {\"state\":{\"reported\":{\"1_state\": \"1full\", \"1_latitude\": \"37.701653\", \"1_longitude\": \"126.900892\", \"id\" : \"1\"}}}"); // ###########
      //sprintf(payload, " {\"state\":{\"reported\":{\"2_state\": \"2full\", \"2_latitude\": \"37.701653\", \"2_longitude\": \"126.900892\", \"id\" : \"2\"}}}");
      //sprintf(payload, " {\"state\":{\"reported\":{\"3_state\": \"3full\", \"3_latitude\": \"37.701653\", \"3_longitude\": \"126.900892\", \"id\" : \"3\"}}}");
      //sprintf(payload, " {\"state\":{\"reported\":{\"4_state\": \"4full\", \"4_latitude\": \"37.701653\", \"4_longitude\": \"126.900892\", \"id\" : \"4\"}}}");
      // 1 = 안일훈, 2 = 이지원 3 = 전예림 4 = 이효중  위도,경도 설 
      // state#
      Serial.println(payload);
      // {"temp":temp, "humid" : humi, "press" : pres}
      //hornbill.publish(pTOPIC_NAME,payload);
    } else {
      Serial.println("Publish failed");
    }
  }
  } else {
    
  }

}
