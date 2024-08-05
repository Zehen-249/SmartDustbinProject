#include <NewPing.h>
#include <ESP32Servo.h>


// Define pins for ultrasonic sensor
#define TRIGGER_PIN  5  // ESP32 pin connected to the Trig pin of the sensor
#define ECHO_PIN     18 // ESP32 pin connected to the Echo pin of the sensor

// Define pins for Stepper
#define STEP_PIN 4
#define DIR_PIN 0
#define STEP_DURATION 5000
#define STEP_DELAY 1000

//Define servo motors
#define topServoPin 32
#define bottServoPin 33
#define topHomePos 90
#define bottHomePos 145
// Define maximum distance we want to ping for (in centimeters)
#define MAX_DISTANCE 200

// Define the number of samples to take
#define NUM_SAMPLES  5

// Created NewPing instance
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//Created Servo instances
Servo topServo;
Servo bottServo;


// Function to get median distance from multiple samples
unsigned int get_median_distance() {
  unsigned int distances[NUM_SAMPLES];
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    distances[i] = sonar.ping_cm();
    delay(10); 
  }
  for (int i = 0; i < NUM_SAMPLES - 1; i++) {
    for (int j = i + 1; j < NUM_SAMPLES; j++) {
      if (distances[i] > distances[j]) {
        unsigned int temp = distances[i];
        distances[i] = distances[j];
        distances[j] = temp;
      }
    }
  }
  return distances[NUM_SAMPLES / 2];
}

// Function to detect person based on distance
bool person_detect() {
  unsigned int distance = get_median_distance();
  return (distance > 0 && distance <= 50)?true:false;
}

void take_trash(){
  for (int pos = bottHomePos; pos >= 90; pos -= 1) {
    bottServo.write(pos);
    delay(15);
  }
  Serial.println("Bottom Flap Closed..");
  delay(1000);

  for (int pos = topHomePos; pos >= 0; pos -= 1) {
    topServo.write(pos);
    delay(15);
  }
  Serial.println(("Top Flap Opened.."));
  while(true){
    if(!person_detect()){
      delay(2000);
      for (int pos = 0; pos <= topHomePos; pos += 1) {
        topServo.write(pos);
        delay(15);
      }
      Serial.println(("Top Flap Closed.."));
      break;
    }
  }
  delay(1000);
}


// Moisture reading
bool measure_moisture(){
   while (Serial.available() > 0) {
    Serial.read();
  }

  while (true) {
    if (Serial.available() > 0) {
      long moist = Serial.parseInt();
      return (moist > 50);
    }
    delay(10); 
  }

}


void move_container(bool moist){

  digitalWrite(DIR_PIN, moist ? HIGH : LOW);
  unsigned long startTime = millis();
  while (millis() - startTime < STEP_DURATION) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(STEP_DELAY);
  }
}

void dump_trash(){
  for (int pos = 90; pos <= bottHomePos; pos += 1) {
    bottServo.write(pos);
    delay(15);
  }
  Serial.println("Bottom flap Opened..");
  delay(1000);
  Serial.println("Trash Dumped..");
  delay(100);
}
