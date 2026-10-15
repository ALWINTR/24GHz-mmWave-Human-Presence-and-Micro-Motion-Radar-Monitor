#include <Arduino.h>
#include <ld2410.h>

// ========================================================
// COMPLETE HUMAN-FOLLOWING ROBOT MASTER FIRMWARE
// LD2410C Radar + Dual HC-SR04 Ultrasonics + L298N Motors
// ========================================================

// 1. Radar Serial2 Pins
#define RADAR_RX_PIN        16   // ESP32 RX2 <-- LD2410 TX
#define RADAR_TX_PIN        17   // ESP32 TX2 --> LD2410 RX
#define RADAR_SERIAL        Serial2

// 2. Ultrasonic Sensors
#define LEFT_TRIG_PIN       5    // Left Trig (D5)
#define LEFT_ECHO_PIN       18   // Left Echo (D18)
#define RIGHT_TRIG_PIN      19   // Right Trig (D19)
#define RIGHT_ECHO_PIN      21   // Right Echo (D21)

// 3. L298N Motor Driver Pins
#define MOTOR_LEFT_IN1      25   // D25
#define MOTOR_LEFT_IN2      26   // D26
#define MOTOR_LEFT_PWM      32   // D32 (ENA - Remove jumper)

#define MOTOR_RIGHT_IN3     27   // D27
#define MOTOR_RIGHT_IN4     14   // D14
#define MOTOR_RIGHT_PWM     33   // D33 (ENB - Remove jumper)

// 4. Optional Light / Relay & Indicator
#define LIGHT_RELAY_PIN     4    // Relay / Light output (D4)
#define ONBOARD_BLUE_LED    2    // Built-in Blue LED (D2)

// ==================== TUNING PARAMETERS ====================
// Radar Gating (Active detection zone up to 1.5 meters)
const int RADAR_MAX_DIST_CM       = 150;  
const int RADAR_MIN_ENERGY        = 25;  
const uint32_t PRESENCE_TIMEOUT_MS = 1200; // Presence debounce delay

// Ultrasonic Tracking Limits
const int MIN_STOP_DISTANCE       = 15;  // 15 cm safety stop
const int MAX_TRACK_DISTANCE      = 120; // 120 cm max follow
const int STEER_DEADBAND          = 6;   // 6 cm steering deadband

// High-Torque Motor Speeds (0 - 255)
const int SPEED_FORWARD           = 190;
const int SPEED_TURN              = 190;
const int SPEED_STOP              = 0;

// ESP32 PWM Configuration
#define PWM_FREQ                  5000
#define PWM_RESOLUTION            8
#define PWM_CH_LEFT               0
#define PWM_CH_RIGHT              1

// ==================== STATE MANAGEMENT ====================
enum RobotState {
  STATE_STANDBY,
  STATE_TRACKING
};

RobotState currentState = STATE_STANDBY;
ld2410 radar;

uint32_t lastPresenceTime = 0;
uint32_t lastLoopTime = 0;
uint32_t lastLogTime = 0;

// ==================== FUNCTION DECLARATIONS ====================
bool checkHumanPresence();
float readUltrasonicCM(int trigPin, int echoPin);
void setMotors(int leftSpeed, int rightSpeed, bool leftFwd, bool rightFwd);
void moveForward();
void turnLeft();
void turnRight();
void stopMotors();

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n========================================================");
  Serial.println("  ESP32 AUTONOMOUS ROBOT: RADAR + ULTRASONIC STEERING   ");
  Serial.println("========================================================");

  // Setup Light / Relay & LED
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  pinMode(ONBOARD_BLUE_LED, OUTPUT);
  digitalWrite(LIGHT_RELAY_PIN, HIGH); // Light OFF
  digitalWrite(ONBOARD_BLUE_LED, LOW);

  // Setup Ultrasonics
  pinMode(LEFT_TRIG_PIN, OUTPUT);
  pinMode(LEFT_ECHO_PIN, INPUT);
  pinMode(RIGHT_TRIG_PIN, OUTPUT);
  pinMode(RIGHT_ECHO_PIN, INPUT);

  // Setup Motors
  pinMode(MOTOR_LEFT_IN1, OUTPUT);
  pinMode(MOTOR_LEFT_IN2, OUTPUT);
  pinMode(MOTOR_RIGHT_IN3, OUTPUT);
  pinMode(MOTOR_RIGHT_IN4, OUTPUT);

  // Configure Hardware PWM for speed
  ledcSetup(PWM_CH_LEFT, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(MOTOR_LEFT_PWM, PWM_CH_LEFT);
  ledcSetup(PWM_CH_RIGHT, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(MOTOR_RIGHT_PWM, PWM_CH_RIGHT);

  stopMotors();

  // Initialize Radar
  RADAR_SERIAL.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
  if (radar.begin(RADAR_SERIAL)) {
    Serial.println("[RADAR] LD2410C Online and Scanning!");
  } else {
    Serial.println("[RADAR] Initializing radar stream...");
  }
}

void loop() {
  radar.read();

  if (millis() - lastLoopTime >= 50) {
    lastLoopTime = millis();

    // 1. Primary Gate: Verify human presence with LD2410C radar
    bool humanDetected = checkHumanPresence();
    if (humanDetected) {
      lastPresenceTime = millis();
    }

    bool activePresence = (millis() - lastPresenceTime < PRESENCE_TIMEOUT_MS);

    // =========================================================
    // STAGE 1: NO HUMAN DETECTED -> STANDBY (MOTORS LOCKED OFF)
    // =========================================================
    if (!activePresence) {
      if (currentState != STATE_STANDBY) {
        currentState = STATE_STANDBY;
        stopMotors();
        digitalWrite(LIGHT_RELAY_PIN, HIGH); // Light OFF
        digitalWrite(ONBOARD_BLUE_LED, LOW);
      }

      if (millis() - lastLogTime >= 1000) {
        lastLogTime = millis();
        Serial.println("[STANDBY] No human in 1.5m zone -> Motors: STOPPED");
      }
      return;
    }

    // =========================================================
    // STAGE 2: HUMAN CONFIRMED -> ULTRASONIC NAVIGATION
    // =========================================================
    if (currentState != STATE_TRACKING) {
      currentState = STATE_TRACKING;
      digitalWrite(LIGHT_RELAY_PIN, LOW); // Light ON
      digitalWrite(ONBOARD_BLUE_LED, HIGH);
      Serial.println("\n💡 >>> [HUMAN DETECTED] Gateway Cleared -> Ultrasonic Steering Active! <<<");
    }

    float distLeft  = readUltrasonicCM(LEFT_TRIG_PIN, LEFT_ECHO_PIN);
    float distRight = readUltrasonicCM(RIGHT_TRIG_PIN, RIGHT_ECHO_PIN);

    bool shouldPrint = (millis() - lastLogTime >= 250);
    if (shouldPrint) {
      lastLogTime = millis();
      Serial.printf("[TRACKING] L: %5.1f cm | R: %5.1f cm ===> Action: ", distLeft, distRight);
    }

    // --- Decision Priority ---
    // A. Safety Obstacle Stop (< 15 cm)
    if ((distLeft > 0 && distLeft < MIN_STOP_DISTANCE) ||
        (distRight > 0 && distRight < MIN_STOP_DISTANCE)) {
      stopMotors();
      if (shouldPrint) Serial.println("SAFETY STOP (Too close!)");
    }
    // B. Both out of follow range
    else if ((distLeft > MAX_TRACK_DISTANCE || distLeft <= 0) &&
             (distRight > MAX_TRACK_DISTANCE || distRight <= 0)) {
      stopMotors();
      if (shouldPrint) Serial.println("HOLD (Searching...)");
    }
    // C. Person closer on Left -> Turn Left
    else if (distLeft > 0 && (distLeft + STEER_DEADBAND < distRight || distRight <= 0)) {
      turnLeft();
      if (shouldPrint) Serial.println("TURN LEFT");
    }
    // D. Person closer on Right -> Turn Right
    else if (distRight > 0 && (distRight + STEER_DEADBAND < distLeft || distLeft <= 0)) {
      turnRight();
      if (shouldPrint) Serial.println("TURN RIGHT");
    }
    // E. Centered -> Move Forward
    else {
      moveForward();
      if (shouldPrint) Serial.println("FORWARD");
    }
  }
}

// ==================== RADAR FILTER ====================
bool checkHumanPresence() {
  if (!radar.isConnected()) return false;

  bool moving = radar.movingTargetDetected() &&
                (radar.movingTargetDistance() > 0 && radar.movingTargetDistance() <= RADAR_MAX_DIST_CM) &&
                (radar.movingTargetEnergy() >= RADAR_MIN_ENERGY);

  bool stat = radar.stationaryTargetDetected() &&
              (radar.stationaryTargetDistance() > 0 && radar.stationaryTargetDistance() <= RADAR_MAX_DIST_CM) &&
              (radar.stationaryTargetEnergy() >= RADAR_MIN_ENERGY);

  return (moving || stat);
}

// ==================== ULTRASONIC SENSOR ====================
float readUltrasonicCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000);
  if (duration == 0) return -1.0;

  return (duration * 0.0343) / 2.0;
}

// ==================== HIGH-TORQUE MOTOR CONTROLS ====================
void setMotors(int leftSpeed, int rightSpeed, bool leftFwd, bool rightFwd) {
  // Left Motor
  digitalWrite(MOTOR_LEFT_IN1, leftFwd ? HIGH : LOW);
  digitalWrite(MOTOR_LEFT_IN2, leftFwd ? LOW : HIGH);
  ledcWrite(PWM_CH_LEFT, leftSpeed);

  // Right Motor
  digitalWrite(MOTOR_RIGHT_IN3, rightFwd ? HIGH : LOW);
  digitalWrite(MOTOR_RIGHT_IN4, rightFwd ? LOW : HIGH);
  ledcWrite(PWM_CH_RIGHT, rightSpeed);
}

void moveForward() {
  setMotors(SPEED_FORWARD, SPEED_FORWARD, true, true);
}

void turnLeft() {
  // Pivot left (Left motor 0, Right motor forward)
  setMotors(0, SPEED_TURN, false, true);
}

void turnRight() {
  // Pivot right (Left motor forward, Right motor 0)
  setMotors(SPEED_TURN, 0, true, false);
}

void stopMotors() {
  setMotors(SPEED_STOP, SPEED_STOP, true, true);
}
