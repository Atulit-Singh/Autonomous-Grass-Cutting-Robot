#include <Wire.h>
#include <Servo.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

Servo servo;
TinyGPSPlus gps;
HardwareSerial GPS(2); // GPS on Serial2

// Pin definitions
#define TRIG_PIN 5
#define ECHO_PIN 15
#define MOTOR1_IN1 25
#define MOTOR1_IN2 26
#define MOTOR1_PWM 13
#define MOTOR2_IN1 27
#define MOTOR2_IN2 14
#define MOTOR2_PWM 12
#define MOTOR3_IN1 23
#define MOTOR3_IN2 22
#define MOTOR3_PWM 32
#define MOTOR4_IN1 21
#define MOTOR4_IN2 19
#define MOTOR4_PWM 33
#define BLDC_PIN 4

// Boundary
const float LAT_MIN = 12.971598; // Replace with your boundary values
const float LAT_MAX = 12.975000;
const float LON_MIN = 77.594566;
const float LON_MAX = 77.598000;

void setup() {
  Serial.begin(115200);
  GPS.begin(9600, SERIAL_8N1, 16, 17); // GPS TX to GPIO16, RX to GPIO17

  // Motor pins
  pinMode(MOTOR1_IN1, OUTPUT);
  pinMode(MOTOR1_IN2, OUTPUT);
  pinMode(MOTOR2_IN1, OUTPUT);
  pinMode(MOTOR2_IN2, OUTPUT);
  pinMode(MOTOR3_IN1, OUTPUT);
  pinMode(MOTOR3_IN2, OUTPUT);
  pinMode(MOTOR4_IN1, OUTPUT);
  pinMode(MOTOR4_IN2, OUTPUT);

  // Ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Servo
  servo.attach(18); // PWM pin
}

void loop() {
  trackGPS();
  measureDistance();
}

// Track GPS data and enforce boundary
void trackGPS() {
  while (GPS.available()) {
    if (gps.encode(GPS.read())) {
      float latitude = gps.location.lat();
      float longitude = gps.location.lng();
      Serial.print("Lat: ");
      Serial.print(latitude);
      Serial.print(", Lon: ");
      Serial.println(longitude);

      if (latitude < LAT_MIN || latitude > LAT_MAX || longitude < LON_MIN || longitude > LON_MAX) {
        stopMotors();
        Serial.println("Boundary exceeded! Stopping robot.");
      } else {
        moveForward();
      }
    }
  }
}

// Ultrasonic sensor for obstacle detection
void measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  float duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2; // cm
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  if (distance < 10) {
    stopMotors();
    Serial.println("Obstacle detected! Stopping.");
  }
}

// Motor control
void moveForward() {
  analogWrite(MOTOR1_PWM, 255); // Full speed
  digitalWrite(MOTOR1_IN1, HIGH);
  digitalWrite(MOTOR1_IN2, LOW);
  digitalWrite(MOTOR2_IN1, HIGH);
  digitalWrite(MOTOR2_IN2, LOW);
  analogWrite(MOTOR3_PWM, 255);
  digitalWrite(MOTOR3_IN1, HIGH);
  digitalWrite(MOTOR3_IN2, LOW);
  digitalWrite(MOTOR4_IN1, HIGH);
  digitalWrite(MOTOR4_IN2, LOW);
}

void stopMotors() {
  digitalWrite(MOTOR1_IN1, LOW);
  digitalWrite(MOTOR1_IN2, LOW);
  digitalWrite(MOTOR2_IN1, LOW);
  digitalWrite(MOTOR2_IN2, LOW);
  digitalWrite(MOTOR3_IN1, LOW);
  digitalWrite(MOTOR3_IN2, LOW);
  digitalWrite(MOTOR4_IN1, LOW);
  digitalWrite(MOTOR4_IN2, LOW);
}
