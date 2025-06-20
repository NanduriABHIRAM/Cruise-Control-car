#define BLYNK_TEMPLATE_ID "TMPL34V4BYxmK"
#define BLYNK_TEMPLATE_NAME "car"
#define BLYNK_AUTH_TOKEN "KbfsEgtu3P8d2r2jFJ__MhfkW1rSIGfa"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Abhiram A23";
char pass[] = "12345678";

// Motor pins
#define IN1 26
#define IN2 27
#define ENA 25
#define IN3 14
#define IN4 12
#define ENB 33

// Ultrasonic sensor pins
#define trigPin 4
#define echoPin 2

bool forwardCmd = false;
bool backwardCmd = false;
bool leftCmd = false;
bool rightCmd = false;
bool cruiseMode = false;

BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  timer.setInterval(100L, driveControl);
}

// Movement functions
void moveForward(int speed = 200) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 200);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  analogWrite(ENA, 200);
  analogWrite(ENB, 0);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

long readDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000); // Timeout after 20ms
  long distance = duration * 0.034 / 2;
  return distance;
}

void driveControl() {
  if (cruiseMode) {
    long distance = readDistanceCM();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance < 15) {
      stopMotors();
      Blynk.virtualWrite(V5, "Obstacle detected - Slow down");
    } else {
      moveForward(180);  // Cruise speed
      Blynk.virtualWrite(V5, "Cruise mode: Moving forward");
    }
  } else if (forwardCmd) {
    moveForward();
  } else if (backwardCmd) {
    moveBackward();
  } else if (leftCmd) {
    turnLeft();
  } else if (rightCmd) {
    turnRight();
  } else {
    stopMotors();
    Blynk.virtualWrite(V5, "Stopped");
  }
}

// Blynk button handlers
BLYNK_WRITE(V0) { forwardCmd = param.asInt(); }
BLYNK_WRITE(V1) { backwardCmd = param.asInt(); }
BLYNK_WRITE(V2) { leftCmd = param.asInt(); }
BLYNK_WRITE(V3) { rightCmd = param.asInt(); }
BLYNK_WRITE(V4) { cruiseMode = param.asInt(); }

void loop() {
  Blynk.run();
  timer.run();
}
