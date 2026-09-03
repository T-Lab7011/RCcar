#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// =====================================================
// Wi-Fi
// =====================================================

const char* ssid = "ESP32_RC_CAR";
const char* password = "12345678";

WebServer server(80);


// =====================================================
// DRV8833 / PWM
// =====================================================

const int AIN1 = 25;
const int AIN2 = 33;

const int BIN1 = 27;
const int BIN2 = 14;

const int MOTOR_PWM_FREQUENCY = 20000;
const int MOTOR_PWM_RESOLUTION = 8;
const int MOTOR_PWM_MAX = 255;

const unsigned long DRIVE_TIMEOUT_MS = 400;
unsigned long lastDriveCommandMillis = 0;
bool motorsRunning = false;


// =====================================================
// Servo
// =====================================================

Servo myservo;

const int servoPin = 32;

const int SERVO_CENTER = 90;
const int SERVO_LEFT   = 40;
const int SERVO_RIGHT  = 150;


// =====================================================
// Motor
// =====================================================

void stopMotors() {

  ledcWrite(AIN1, 0);
  ledcWrite(AIN2, 0);

  ledcWrite(BIN1, 0);
  ledcWrite(BIN2, 0);

  motorsRunning = false;

  Serial.println("STOP");
}


void setMotorSpeed(int speedPercent) {

  speedPercent = constrain(speedPercent, -100, 100);

  if (speedPercent == 0) {
    stopMotors();
    return;
  }

  int duty = map(abs(speedPercent), 0, 100, 0, MOTOR_PWM_MAX);

  if (speedPercent > 0) {
    ledcWrite(AIN1, duty);
    ledcWrite(AIN2, 0);
    ledcWrite(BIN1, duty);
    ledcWrite(BIN2, 0);
  } else {
    ledcWrite(AIN1, 0);
    ledcWrite(AIN2, duty);
    ledcWrite(BIN1, 0);
    ledcWrite(BIN2, duty);
  }

  lastDriveCommandMillis = millis();
  motorsRunning = true;

  Serial.print("Motor speed = ");
  Serial.println(speedPercent);
}


// =====================================================
// HTML
// =====================================================
String webpage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no,viewport-fit=cover">
<meta name="theme-color" content="#07111f">
<title>ESP32 RC CAR</title>
<style>
:root {
  --bg: #07111f;
  --panel: #101d2e;
  --panel-border: #263a52;
  --text: #f5f8fc;
  --muted: #9fb0c5;
  --drive: #35d07f;
  --drive-dark: #16834a;
  --steer: #4da3ff;
  --steer-dark: #2466aa;
  --stop: #ff4d57;
  --stop-dark: #a82631;
}

* {
  box-sizing: border-box;
  -webkit-tap-highlight-color: transparent;
}

html, body {
  width: 100%;
  min-height: 100%;
  margin: 0;
  overflow: hidden;
  background: var(--bg);
  color: var(--text);
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
  user-select: none;
  -webkit-user-select: none;
  touch-action: none;
}

body {
  min-height: 100dvh;
  padding: max(10px, env(safe-area-inset-top)) max(12px, env(safe-area-inset-right)) max(10px, env(safe-area-inset-bottom)) max(12px, env(safe-area-inset-left));
  display: grid;
  grid-template-rows: auto 1fr auto;
  gap: 10px;
}

.header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
}

.brand {
  min-width: 0;
}

.title {
  font-size: clamp(18px, 3.5vw, 27px);
  font-weight: 850;
  letter-spacing: .03em;
  line-height: 1.05;
}

.hint {
  margin-top: 4px;
  color: var(--muted);
  font-size: clamp(10px, 1.8vw, 13px);
}

.status {
  flex: none;
  display: flex;
  align-items: center;
  gap: 7px;
  min-height: 34px;
  padding: 7px 12px;
  border: 1px solid #27583f;
  border-radius: 999px;
  background: #102a20;
  color: #88f1b5;
  font-size: clamp(11px, 1.8vw, 14px);
  font-weight: 750;
}

.status::before {
  content: "";
  width: 9px;
  height: 9px;
  border-radius: 50%;
  background: currentColor;
  box-shadow: 0 0 10px currentColor;
}

.status.error {
  border-color: #6a2a31;
  background: #32181d;
  color: #ff9298;
}

.controller {
  min-height: 0;
  display: grid;
  grid-template-columns: minmax(0, 1fr) minmax(0, 1fr);
  gap: 10px;
}

.panel {
  min-width: 0;
  min-height: 0;
  padding: clamp(9px, 2vw, 16px);
  border: 1px solid var(--panel-border);
  border-radius: 20px;
  background: var(--panel);
  display: grid;
  grid-template-rows: auto 1fr;
  gap: 8px;
}

.panelTitle {
  display: flex;
  align-items: baseline;
  justify-content: space-between;
  gap: 8px;
}

.panelTitle strong {
  font-size: clamp(14px, 2.5vw, 19px);
}

.panelTitle span {
  color: var(--muted);
  font-size: clamp(10px, 1.7vw, 12px);
}

.drive, .steering {
  min-height: 0;
  display: grid;
  gap: clamp(8px, 2vw, 14px);
}

.drive {
  grid-template-rows: 1fr 1fr;
}

.steering {
  grid-template-columns: 1fr 1fr;
}

.controlButton, .stopButton {
  position: relative;
  min-width: 0;
  min-height: 54px;
  border: 0;
  color: #06110b;
  font: inherit;
  font-weight: 900;
  cursor: pointer;
  touch-action: none;
  transition: transform 60ms ease, filter 60ms ease, box-shadow 60ms ease;
}

.controlButton {
  border-radius: 18px;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: clamp(8px, 2vw, 16px);
  font-size: clamp(15px, 2.6vw, 22px);
}

.controlButton .icon {
  font-size: clamp(28px, 7vmin, 54px);
  line-height: 1;
}

.driveButton {
  background: var(--drive);
  box-shadow: inset 0 -5px 0 var(--drive-dark);
}

.steerButton {
  background: var(--steer);
  color: #051424;
  box-shadow: inset 0 -5px 0 var(--steer-dark);
}

.controlButton.active,
.controlButton:active {
  transform: translateY(3px) scale(.985);
  filter: brightness(1.15);
  box-shadow: inset 0 -2px 0 rgba(0, 0, 0, .35), 0 0 0 3px rgba(255, 255, 255, .3);
}

.stopButton {
  width: 100%;
  height: clamp(56px, 12vh, 74px);
  border-radius: 18px;
  background: var(--stop);
  color: white;
  box-shadow: inset 0 -5px 0 var(--stop-dark);
  font-size: clamp(18px, 3vw, 24px);
  letter-spacing: .08em;
}

.stopButton:active {
  transform: translateY(3px) scale(.99);
  filter: brightness(1.13);
  box-shadow: inset 0 -2px 0 var(--stop-dark);
}

button:focus-visible {
  outline: 3px solid white;
  outline-offset: 2px;
}

@media (orientation: portrait) {
  body {
    overflow: auto;
  }

  .controller {
    grid-template-columns: 1fr;
    grid-template-rows: minmax(250px, 1fr) minmax(190px, .75fr);
  }

  .drive {
    grid-template-columns: 1fr 1fr;
    grid-template-rows: 1fr;
  }

  .hint::after {
    content: "（横向きがおすすめ）";
  }
}

@media (max-height: 430px) and (orientation: landscape) {
  body {
    gap: 7px;
    padding-top: max(6px, env(safe-area-inset-top));
    padding-bottom: max(6px, env(safe-area-inset-bottom));
  }

  .hint {
    display: none;
  }

  .panel {
    border-radius: 16px;
    padding: 8px;
    gap: 5px;
  }

  .stopButton {
    height: 50px;
  }
}

/* Analog controller layout */
.header {
  position: relative;
  min-height: 48px;
  justify-content: center;
  padding: 6px 150px;
  border: 1px solid var(--panel-border);
  border-radius: 14px;
  background: #091524;
}

.title {
  font-size: clamp(20px, 3.2vw, 32px);
  font-weight: 900;
  letter-spacing: .045em;
  white-space: nowrap;
}

.status {
  position: absolute;
  right: 12px;
  top: 50%;
  transform: translateY(-50%);
}

.controller {
  grid-template-columns: minmax(280px, .9fr) minmax(360px, 1.1fr);
  gap: 8px;
}

.panel {
  padding: clamp(8px, 1.5vw, 14px);
  border-radius: 16px;
  gap: 6px;
}

.panelTitle {
  display: block;
  text-align: center;
  font-size: clamp(17px, 2.4vw, 25px);
  font-weight: 900;
  letter-spacing: .025em;
}

.throttleBody {
  min-height: 0;
  display: grid;
  grid-template-columns: minmax(72px, .75fr) 72px minmax(110px, 1fr);
  align-items: stretch;
  gap: clamp(8px, 1.5vw, 18px);
}

.directionLabels {
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  padding: 8px 0;
  color: #68de38;
  font-size: clamp(10px, 1.5vw, 14px);
  font-weight: 850;
}

.directionLabels div {
  display: grid;
  gap: 2px;
  text-align: center;
}

.directionLabels .arrow {
  font-size: clamp(22px, 5vmin, 40px);
  line-height: 1;
}

.directionLabels .neutral { color: var(--text); }

.verticalTrack, .horizontalTrack {
  position: relative;
  border: 3px solid #788595;
  background: #050a10;
  box-shadow: inset 0 0 12px rgba(0, 0, 0, .9);
  touch-action: none;
}

.verticalTrack {
  width: 62px;
  height: 100%;
  min-height: 180px;
  margin: auto;
  border-radius: 28px;
  background: linear-gradient(to bottom, rgba(104, 222, 56, .7), #0d2211 47%, #050a10 50%, #0d2211 53%, rgba(104, 222, 56, .55));
}

.verticalTrack::after, .horizontalTrack::after {
  content: "";
  position: absolute;
  background: white;
  opacity: .9;
  pointer-events: none;
}

.verticalTrack::after {
  left: -14px;
  right: -14px;
  top: 50%;
  height: 2px;
}

.thumb {
  position: absolute;
  z-index: 2;
  display: flex;
  align-items: center;
  justify-content: center;
  border: 2px solid rgba(255, 255, 255, .45);
  box-shadow: 0 4px 0 rgba(0, 0, 0, .45), 0 0 14px currentColor;
  transition: top 70ms linear, left 70ms linear;
  pointer-events: none;
}

.verticalThumb {
  left: 50%;
  top: 50%;
  width: 82px;
  height: 64px;
  transform: translate(-50%, -50%);
  border-radius: 18px;
  background: #68de38;
  color: #68de38;
}

.grip {
  width: 34px;
  height: 18px;
  border-top: 3px solid rgba(0, 0, 0, .45);
  border-bottom: 3px solid rgba(0, 0, 0, .45);
}

.speedInfo {
  min-width: 0;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 10px;
}

.readout {
  min-width: 110px;
  padding: 8px 12px;
  border: 2px solid currentColor;
  border-radius: 14px;
  background: #07111f;
  text-align: center;
  font-size: clamp(28px, 6vmin, 52px);
  font-weight: 900;
  line-height: 1;
}

.speedReadout { color: #68de38; }
.steerReadout { color: #2e86ff; }

.instruction {
  color: var(--muted);
  text-align: center;
  font-size: clamp(9px, 1.35vw, 12px);
  font-weight: 700;
}

.steeringBody {
  min-height: 0;
  display: flex;
  flex-direction: column;
  justify-content: center;
  gap: clamp(10px, 3vh, 22px);
  padding: 0 clamp(6px, 2vw, 22px);
}

.steeringLabels {
  display: flex;
  justify-content: space-between;
  color: #2e86ff;
  font-weight: 850;
  font-size: clamp(11px, 1.8vw, 15px);
}

.steeringLabels span:nth-child(2) { color: var(--muted); }

.horizontalTrack {
  width: 100%;
  height: 62px;
  border-radius: 30px;
  background: linear-gradient(to right, rgba(46, 134, 255, .7), #07111f 47%, #050a10 50%, #07111f 53%, rgba(46, 134, 255, .7));
}

.horizontalTrack::after {
  top: -14px;
  bottom: -14px;
  left: 50%;
  width: 2px;
}

.horizontalThumb {
  left: 50%;
  top: 50%;
  width: 72px;
  height: 84px;
  transform: translate(-50%, -50%);
  border-radius: 18px;
  background: #2e86ff;
  color: #2e86ff;
}

.steeringScale {
  display: flex;
  justify-content: space-between;
  margin-top: -8px;
  color: var(--muted);
  font-size: clamp(9px, 1.4vw, 12px);
}

.steeringInfo {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 6px;
}

.stopButton {
  height: clamp(52px, 11vh, 72px);
  border: 2px solid #ff5555;
  border-radius: 16px;
  background: #ef2929;
  box-shadow: inset 0 -5px 0 #9e1515;
  font-size: clamp(19px, 3.4vw, 29px);
}

@media (orientation: portrait) {
  .header { padding: 7px 105px 7px 8px; justify-content: flex-start; }
  .title { font-size: 16px; }
  .controller {
    grid-template-columns: 1fr;
    grid-template-rows: minmax(360px, 1fr) minmax(310px, .85fr);
  }
  .verticalTrack { min-height: 245px; }
}

@media (max-height: 430px) and (orientation: landscape) {
  .header { min-height: 38px; padding-top: 3px; padding-bottom: 3px; }
  .panelTitle { font-size: 17px; }
  .horizontalThumb { height: 68px; }
  .readout { font-size: 30px; }
  .stopButton { height: 48px; }
}
</style>
</head>
<body>
  <header class="header">
    <div class="title">ESP32 RC CAR</div>
    <div class="status" id="status" role="status" aria-live="polite">CONNECTED</div>
  </header>

  <main class="controller">
    <section class="panel" aria-labelledby="speedTitle">
      <div class="panelTitle" id="speedTitle">MOTOR SPEED</div>
      <div class="throttleBody">
        <div class="directionLabels" aria-hidden="true">
          <div><span class="arrow">▲</span><span>FORWARD</span></div>
          <div class="neutral"><span>STOP</span></div>
          <div><span>REVERSE</span><span class="arrow">▼</span></div>
        </div>
        <div class="verticalTrack" id="throttle" role="slider" aria-label="Motor speed" aria-valuemin="-100" aria-valuemax="100" aria-valuenow="0" tabindex="0">
          <div class="thumb verticalThumb" id="throttleThumb"><span class="grip"></span></div>
        </div>
        <div class="speedInfo">
          <div class="readout speedReadout" id="speedReadout">0%</div>
          <div class="instruction" id="driveState">STOPPED</div>
          <div class="instruction">HOLD &amp; DRAG<br>RELEASE TO STOP</div>
        </div>
      </div>
    </section>

    <section class="panel" aria-labelledby="steerTitle">
      <div class="panelTitle" id="steerTitle">STEERING</div>
      <div class="steeringBody">
        <div class="steeringLabels" aria-hidden="true"><span>◀ LEFT</span><span>CENTER</span><span>RIGHT ▶</span></div>
        <div class="horizontalTrack" id="steering" role="slider" aria-label="Steering angle" aria-valuemin="-45" aria-valuemax="45" aria-valuenow="0" tabindex="0">
          <div class="thumb horizontalThumb" id="steeringThumb"><span class="grip"></span></div>
        </div>
        <div class="steeringScale" aria-hidden="true"><span>-45°</span><span>-30°</span><span>-15°</span><span>0°</span><span>15°</span><span>30°</span><span>45°</span></div>
        <div class="steeringInfo">
          <div class="readout steerReadout" id="steerReadout">0°</div>
          <div class="instruction" id="steerState">CENTERED</div>
          <div class="instruction">HOLD &amp; DRAG · RELEASE TO CENTER</div>
        </div>
      </div>
    </section>
  </main>

  <button class="stopButton" id="stop" aria-label="Emergency stop">⚠ EMERGENCY STOP</button>

<script>
const statusElement = document.getElementById("status");
const driveState = document.getElementById("driveState");
const steerState = document.getElementById("steerState");
const throttle = document.getElementById("throttle");
const throttleThumb = document.getElementById("throttleThumb");
const speedReadout = document.getElementById("speedReadout");
const steering = document.getElementById("steering");
const steeringThumb = document.getElementById("steeringThumb");
const steerReadout = document.getElementById("steerReadout");

let driveSpeed = 0;
let steeringAngle = 0;
let driveHeartbeat = null;
let lastDriveSentAt = 0;
let lastSteeringSentAt = 0;

function setConnection(ok) {
  statusElement.textContent = ok ? "CONNECTED" : "CONNECTION ERROR";
  statusElement.classList.toggle("error", !ok);
}

function send(url) {
  return fetch(url, { cache: "no-store" })
    .then(function(response) {
      if (!response.ok) throw new Error("HTTP " + response.status);
      setConnection(true);
    })
    .catch(function() {
      setConnection(false);
    });
}

function vibrate(duration) {
  if (navigator.vibrate) navigator.vibrate(duration || 20);
}

function clamp(value, minimum, maximum) {
  return Math.min(maximum, Math.max(minimum, value));
}

function sendDrive(force) {
  const now = Date.now();
  if (!force && now - lastDriveSentAt < 50) return;
  lastDriveSentAt = now;
  send("/drive?speed=" + driveSpeed);
}

function sendSteering(force) {
  const now = Date.now();
  if (!force && now - lastSteeringSentAt < 50) return;
  lastSteeringSentAt = now;
  send("/steer?angle=" + steeringAngle);
}

function updateThrottle(event) {
  const rect = throttle.getBoundingClientRect();
  const ratio = clamp((event.clientY - rect.top) / rect.height, 0, 1);
  let value = Math.round((0.5 - ratio) * 200);

  if (Math.abs(value) < 5) value = 0;

  driveSpeed = value;
  throttleThumb.style.top = (50 - value * 0.42) + "%";
  throttle.setAttribute("aria-valuenow", value);
  speedReadout.textContent = Math.abs(value) + "%";
  driveState.textContent = value > 0 ? "FORWARD" : value < 0 ? "REVERSE" : "STOPPED";
  sendDrive(false);
}

function resetThrottle() {
  driveSpeed = 0;
  throttleThumb.style.top = "50%";
  throttle.setAttribute("aria-valuenow", "0");
  speedReadout.textContent = "0%";
  driveState.textContent = "STOPPED";

  if (driveHeartbeat) clearInterval(driveHeartbeat);
  driveHeartbeat = null;
  sendDrive(true);
}

function updateSteering(event) {
  const rect = steering.getBoundingClientRect();
  const ratio = clamp((event.clientX - rect.left) / rect.width, 0, 1);
  let value = Math.round((ratio - 0.5) * 90);

  if (Math.abs(value) < 2) value = 0;

  steeringAngle = value;
  steeringThumb.style.left = (50 + value * 0.9333) + "%";
  steering.setAttribute("aria-valuenow", value);
  steerReadout.textContent = (value > 0 ? "+" : "") + value + "°";
  steerState.textContent = value < 0 ? "TURNING LEFT" : value > 0 ? "TURNING RIGHT" : "CENTERED";
  sendSteering(false);
}

function resetSteering() {
  steeringAngle = 0;
  steeringThumb.style.left = "50%";
  steering.setAttribute("aria-valuenow", "0");
  steerReadout.textContent = "0°";
  steerState.textContent = "CENTERED";
  sendSteering(true);
}

function analogControl(element, onMove, onRelease, heartbeat) {
  let activePointer = null;

  function release(event) {
    if (activePointer === null) return;
    if (event && event.pointerId !== undefined && event.pointerId !== activePointer) return;
    activePointer = null;
    onRelease();
  }

  element.addEventListener("pointerdown", function(event) {
    if (activePointer !== null) return;
    event.preventDefault();
    activePointer = event.pointerId;
    element.setPointerCapture(event.pointerId);
    vibrate(18);
    onMove(event);

    if (heartbeat) {
      if (driveHeartbeat) clearInterval(driveHeartbeat);
      driveHeartbeat = setInterval(function() {
        sendDrive(true);
      }, 120);
    }
  });

  element.addEventListener("pointermove", function(event) {
    if (event.pointerId === activePointer) onMove(event);
  });
  element.addEventListener("pointerup", release);
  element.addEventListener("pointercancel", release);
  element.addEventListener("lostpointercapture", release);
}

analogControl(throttle, updateThrottle, resetThrottle, true);
analogControl(steering, updateSteering, resetSteering, false);

function emergencyStop() {
  vibrate(45);
  resetThrottle();
  resetSteering();
  driveState.textContent = "EMERGENCY STOP";
}

document.getElementById("stop").addEventListener("click", emergencyStop);

function safeStop() {
  resetThrottle();
  resetSteering();
}

document.addEventListener("visibilitychange", function() {
  if (document.hidden) safeStop();
});
window.addEventListener("blur", safeStop);
document.addEventListener("contextmenu", function(event) {
  event.preventDefault();
});
</script>
</body>
</html>
)rawliteral";



// =====================================================
// setup
// =====================================================

void setup() {

  Serial.begin(115200);


  // ===================================================
  // 1. モータを最優先で停止状態へ
  // ===================================================

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);


  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);


  // ===================================================
  // 2. Servo
  //
  // 正常動作した単体テストと同じ初期化
  // ===================================================

  // サーボ1台分だけ予約し、残りのLEDCタイマーをモーターPWMへ残す
  ESP32PWM::allocateTimer(0);


  myservo.setPeriodHertz(50);


  myservo.attach(
    servoPin,
    1000,
    2000
  );


  // ★ attach直後に90度
  myservo.write(SERVO_CENTER);




  // サーボを安定させる
  delay(500);


  // ===================================================
  // 3. Motor PWM
  // サーボ用LEDCチャンネルとの競合を避けるため4～7を使用
  // ===================================================

  ledcAttachChannel(AIN1, MOTOR_PWM_FREQUENCY, MOTOR_PWM_RESOLUTION, 4);
  ledcAttachChannel(AIN2, MOTOR_PWM_FREQUENCY, MOTOR_PWM_RESOLUTION, 5);
  ledcAttachChannel(BIN1, MOTOR_PWM_FREQUENCY, MOTOR_PWM_RESOLUTION, 6);
  ledcAttachChannel(BIN2, MOTOR_PWM_FREQUENCY, MOTOR_PWM_RESOLUTION, 7);

  stopMotors();


  // ===================================================
  // 4. Wi-Fi
  // ===================================================

  Serial.println("Starting WiFi...");


  WiFi.mode(WIFI_AP);


  WiFi.softAP(
    ssid,
    password
  );


  Serial.println("WiFi started");


  Serial.print("IP address: ");

  Serial.println(
    WiFi.softAPIP()
  );


  // Wi-Fi起動後も念のため90度を再指令
  myservo.write(SERVO_CENTER);


  // ===================================================
  // 5. Web Server
  // ===================================================

  server.on("/", []() {

    server.send(
      200,
      "text/html",
      webpage
    );

  });


  server.on("/forward", []() {

    setMotorSpeed(100);

    server.send(
      200,
      "text/plain",
      "OK"
    );

  });


  server.on("/backward", []() {

    setMotorSpeed(-100);

    server.send(
      200,
      "text/plain",
      "OK"
    );

  });


  server.on("/stop", []() {

    stopMotors();

    server.send(
      200,
      "text/plain",
      "OK"
    );

  });


  server.on("/drive", []() {

    if (!server.hasArg("speed")) {
      server.send(400, "text/plain", "Missing speed");
      return;
    }

    int speedPercent =
      constrain(
        server.arg("speed").toInt(),
        -100,
        100
      );

    setMotorSpeed(speedPercent);

    server.send(
      200,
      "text/plain",
      "OK"
    );

  });


  server.on("/steer", []() {

    if (!server.hasArg("angle")) {
      server.send(400, "text/plain", "Missing angle");
      return;
    }

    int displayAngle =
      constrain(
        server.arg("angle").toInt(),
        -45,
        45
      );

    int servoAngle =
      map(
        displayAngle,
        -45,
        45,
        SERVO_LEFT,
        SERVO_RIGHT
      );

    myservo.write(servoAngle);

    Serial.print("Steering = ");
    Serial.print(displayAngle);
    Serial.print(" degrees, servo = ");
    Serial.println(servoAngle);

    server.send(
      200,
      "text/plain",
      "OK"
    );

  });


  server.on("/servo", []() {

    if (server.hasArg("angle")) {

      int angle =
        server.arg("angle").toInt();


      // サーボの可動範囲（40～150度）に制限する
      angle =
        constrain(
          angle,
          SERVO_LEFT,
          SERVO_RIGHT
        );


      myservo.write(angle);


      Serial.print("Servo = ");

      Serial.println(angle);

    }


    server.send(
      200,
      "text/plain",
      "OK"
    );

  });


  server.begin();


  Serial.println(
    "Web server started"
  );

}


// =====================================================
// loop
// =====================================================

void loop() {

  server.handleClient();

  if (
    motorsRunning &&
    millis() - lastDriveCommandMillis > DRIVE_TIMEOUT_MS
  ) {
    Serial.println("Drive watchdog timeout");
    stopMotors();
  }

}
