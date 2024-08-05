#include <util.h>

void setup() {
  Serial.begin(115200);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  topServo.attach(topServoPin);
  bottServo.attach(bottServoPin);

  topServo.write(topHomePos);
  bottServo.write(bottHomePos);
}

void loop() {
  if(person_detect()){
    
  take_trash();
  delay(1000);
  Serial.println("Trash Taken");

  bool moist;
  moist = measure_moisture();
  delay(2000);
  Serial.println(moist ? "Wet" : "Dry");

  move_container(moist);
  delay(2000);

  dump_trash();
  delay(4000);

  move_container(!moist);
  delay(1000);
  Serial.println("Cycle Complete....");
  }

  delay(1000);
    
}
  
