#include <Servo.h>

Servo gripper;
Servo base;
Servo elbow;
Servo shoulder;

// --- Positions ---
int stableBase     = 85;   // normal tucked base
int preBaseOdd     = 110;  // extra one-step base for odd-finger case
int userBase       = 180;  // rotate to this to "show" to user

int stableShoulder = 10;
int stableElbow    = 78;
int stableGripper  = 60;

int pickShoulder   = 38;
int pickElbow      = 40;
int pickGripper    = 130;

int liftShoulder   = 20;
int liftElbow      = 65;

// extra lift tuning (visible lift mainly by elbow)
int extraElbowLift = 22; // increase if you need more clearance

// runtime flags
bool taskRequested = false; // set true when serial command arrives
int parity = 0;             // 0 = even, 1 = odd (from mediapipe python)

void setup() {
  Serial.begin(115200);

  gripper.attach(11);
  base.attach(6);
  elbow.attach(9);
  shoulder.attach(10);

  // start in stable pose
  base.write(stableBase);
  shoulder.write(stableShoulder);
  elbow.write(stableElbow);
  gripper.write(stableGripper);

  delay(1500);
  Serial.println("Arduino ready. Send '0' (even) or '1' (odd) to trigger.");
}

void loop() {
  // read serial for a trigger (expect '0' or '1')
  if (Serial.available() > 0) {
  char c = Serial.read();

  // ignore newline / carriage return
  if (c == '\n' || c == '\r') return;

  if (c == '0' || c == '1') {
    parity = (c == '1') ? 1 : 0;
    taskRequested = true;

    Serial.print("Received command: ");
    Serial.println(c);
  }
}


  if (taskRequested) {
    runPickAndPlace(parity);
    taskRequested = false;
    Serial.println("Task completed. Waiting for next command.");
  }
}

// Main pick & place flow. parity==1 -> odd behavior with extra base step
void runPickAndPlace(int parityFlag) {
  // If odd parity: first ramp base 85 -> preBaseOdd
  if (parityFlag == 1) {
    smoothMove(base, preBaseOdd, 40); // ramp up to 110
    delay(200);
  }

  // 1) go to pick position
  smoothMove(shoulder, pickShoulder, 40);
  smoothMove(elbow, pickElbow, 40);
  delay(300);

  // 2) grip
  smoothMove(gripper, pickGripper, 15);
  delay(300);

  // 3) lift (use elbow extra lift; shoulder kept at liftShoulder)
  int higherLiftShoulder = liftShoulder;
  int higherLiftElbow    = liftElbow + extraElbowLift;

  // order: shoulder then elbow (geometry)
  smoothMove(shoulder, higherLiftShoulder, 30);
  smoothMove(elbow, higherLiftElbow, 30);
  delay(300);

  // 4) rotate to user (show)
  smoothMove(base, userBase, 50);
  delay(5000); // show for 5 seconds

  // 5) rotate back:
  if (parityFlag == 1) {
    // odd: go back to preBaseOdd (110) first
    smoothMove(base, preBaseOdd, 50);
    delay(300);
  } else {
    // even: go straight back to stableBase
    smoothMove(base, stableBase, 50);
    delay(300);
  }

  // 6) lower to place (pick position)
  smoothMove(shoulder, pickShoulder, 30);
  smoothMove(elbow, pickElbow, 30);
  delay(300);

  // 7) release
  smoothMove(gripper, stableGripper, 15);
  delay(300);

  // 8) safe retract: lift a bit then go to stable posture
  smoothMove(shoulder, higherLiftShoulder, 30);
  smoothMove(elbow, higherLiftElbow, 30);
  delay(200);

  smoothMove(elbow, stableElbow, 30);
  smoothMove(shoulder, stableShoulder, 30);
  delay(200);

  // Final step for odd parity: move preBaseOdd (110) -> stableBase (85) smoothly
  if (parityFlag == 1) {
    smoothMove(base, stableBase, 40);
    delay(200);
  } else {
    // already at stableBase for even case
    // ensure exact stableBase
    smoothMove(base, stableBase, 20);
  }
}

// smoothMove: moves servo from its current read() to end in 1-degree steps
void smoothMove(Servo &motor, int end, int speed) {
  int start = motor.read(); // current position (last write())
  if (start < 0) {
    // fallback: directly write
    motor.write(end);
    delay(20);
    return;
  }

  if (start < end) {
    for (int i = start; i <= end; i++) {
      motor.write(i);
      delay(speed);
    }
  } else {
    for (int i = start; i >= end; i--) {
      motor.write(i);
      delay(speed);
    }
  }
}
