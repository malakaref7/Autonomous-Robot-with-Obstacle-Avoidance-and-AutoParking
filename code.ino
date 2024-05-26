#include <Servo.h>
#include <SoftwareSerial.h>

// Motor Pins
const int in1 = 2;
const int in2 = 3;
const int in3 = 4;
const int in4 = 5;

// Ultrasonic Sensor Pins
const int trigFront = 7;
const int echoFront = 6;
const int trigBack = 13;
const int echoBack = 8;
const int trigRight = 12;
const int echoRight = 11;

// Servo Motor Pin
const int servoPin = 9;
Servo myServo;

// Switch Pin
const int switchPin = 10;

// Variables
long duration, distance;
const long ULTRASONIC_TIMEOUT = 30000L; // Timeout in microseconds (30ms)
const float SOUND_SPEED = 0.0343 / 2;   // Speed of sound in cm/us (divided by 2 for one-way travel)

// Flag to track if the system is on or off
bool systemOn = false;

// Flag to track the previous state of the switch
bool previousSwitchState = true; // Assuming the switch is initially in the OFF state
char reading; // Data type used to store a character value.

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigBack, OUTPUT);
  pinMode(echoBack, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);

  pinMode(switchPin, INPUT_PULLUP); // Ensure the switch pin is properly configured

  myServo.attach(servoPin);
  myServo.write(135);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    reading = Serial.read();
    Serial.println(reading);

     if (reading == 'A') {
      while (digitalRead(switchPin) == LOW) { // Switch pressed (assuming pull-up configuration)
        Serial.println("Switch pressed, starting parking sequence");

        // Move forward until space is detected
        if (getDistance(trigFront, echoFront) > 30 && getDistance(trigBack, echoBack) > 10 && getDistance(trigRight, echoRight) > 20) {
          moveForward();
        } else if (getDistance(trigFront, echoFront) < 30 && getDistance(trigBack, echoBack) > 10 && getDistance(trigRight, echoRight) < 20) {
          stopCar();
          delay(200);

          // Start parking sequence
          Serial.println("Parking space detected, starting parking maneuver");

          // Move backward diagonally
          moveBackwardRight();
          delay(2150); // Adjust time for the desired angle and position
          moveBackward();
          delay(500);
        } else if (getDistance(trigBack, echoBack) > 10 && getDistance(trigRight,echoRight)<20) {
          moveBackward();
        } else {
          stopCar();
        }
        break;

      }
    } else if (reading == 'O') {
      bool currentSwitchState = digitalRead(switchPin) == LOW; // LOW when pressed

      // If the switch state has changed
      if (currentSwitchState != previousSwitchState) {
        // Toggle the system state
        systemOn = !systemOn;

        // Update the previous switch state
        previousSwitchState = currentSwitchState;

        // Add a short delay to debounce the switch
        delay(50);
      }

      // If the system is off, stop motors and return from the loop
      if (!systemOn) {
        stopCar();
        return;
      }

      // Measure distances
      int distanceFront = getDistance(trigFront, echoFront);
      int distanceBack = getDistance(trigBack, echoBack);
      int distanceRight = getDistance(trigRight, echoRight);

      // Obstacle avoidance logic
      if (distanceFront <= 30) {
        stopCar();
        delay(500); // Delay to stop the robot

        // Decide whether to turn left or right based on side distances
        if (distanceRight > 30) {
          // Turn right to avoid obstacle
          turnRight();
          delay(1000); // Adjust delay for turning right
        } else {
          // Turn left to avoid obstacle
          turnLeft();
          delay(1000); // Adjust delay for turning left
        }
      } else if (distanceBack <= 10) {
        stopCar();
        delay(500); // Delay to stop the robot

        // If there's an obstacle close behind, back up and turn left
        moveBackwardLeft();
        delay(1000); // Adjust delay for backward left
      } else {
        // If no obstacle detected, move forward
        moveForward();
      }
    } else {
      stopCar();
    }
  }
}

void moveForward() {
  Serial.println("Moving forward");
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void moveBackward() {
  Serial.println("Moving backward");
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void moveBackwardRight() {
  Serial.println("Moving backward right");
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void moveBackwardLeft() {
  Serial.println("Moving backward left");
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void stopCar() {
  Serial.println("Stopping car");
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

void turnLeft() {
  Serial.println("Turning left");
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void turnRight() {
  Serial.println("Turning right");
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}