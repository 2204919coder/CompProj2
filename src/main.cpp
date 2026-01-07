/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       2204919                                                   */
/*    Created:      10/20/2025, 10:45:29 AM                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"

using namespace vex;

// A global instance of competition
competition Competition;
vex::brain Brain;
controller Controller = controller(primary);
inertial InertialSensor = inertial(PORT19);

// define your global instances of motors and other devices here
motor RightTop = motor(PORT1, ratio18_1, true);
motor RightBottom = motor(PORT2, ratio18_1, false);
motor LeftTop = motor(PORT3, ratio18_1, false); 
motor LeftBottom = motor(PORT4, ratio18_1, true);
motor_group LeftWheel = motor_group(LeftBottom,LeftTop);
motor_group RightWheel = motor_group(RightTop,RightBottom);
smartdrive dTrain = smartdrive(LeftWheel,RightWheel,InertialSensor,320,380,0,mm,60.0/84);

motor BottomChain = motor(PORT20, ratio36_1, true);
motor TopChain = motor(PORT15, ratio36_1, false);

motor Intake = motor(PORT12,false);
motor Lever = motor(PORT13,false);
// motor IntakeLeft = motor(PORT13,false);
// motor_group Intake = motor_group(IntakeLeft,IntakeRight);

motor sorter = motor(PORT14,false); //! Allways use timeout!!!
optical colorSensor = optical(PORT10);
distance distanceSensor = distance(PORT16);


// AI Vision Color Descriptions
aivision::colordesc AIVision18__COLOR1(1, 169, 170, 137, 40, 1);
// AI Vision Code Descriptions
vex::aivision AIVision18(PORT18, AIVision18__COLOR1);

/*
------------------------------------------------------------------------
Where my declared functions go
------------------------------------------------------------------------
*/
//? Intake
void intakeIn() {Intake.spin(forward);}
void intakeOut() {Intake.spin(reverse);}
void intakeStop() {Intake.stop();}

void calibrateLever() {
 Lever.setMaxTorque(100,pct);
 Lever.setVelocity(100,pct);
 Lever.setTimeout(1,sec);
 Lever.spinFor(-100,degrees); //Closes door
 Lever.resetPosition();
 Lever.setMaxTorque(50,pct);
}
bool isLeverDown = false;
void lowerLever() {
  if(isLeverDown) return;
  Lever.setTimeout(1,sec);
  Lever.spinFor(100,degrees);
  isLeverDown = true;
}
void raiseLever() {
  if(!isLeverDown) return;
  Lever.setTimeout(1.5,sec);
  Lever.spinFor(-100,degrees);
  isLeverDown = false;
}

//? Lift System
void spinUp() {
  TopChain.spin(forward);
  BottomChain.spin(forward);
  intakeIn();
}
void spinDown() {
  TopChain.spin(reverse);
  BottomChain.spin(reverse);
  intakeOut();
}
void stop() {
  TopChain.stop();
  BottomChain.stop();
  intakeStop();
}
void removeClog() {
  float moveTime = 0.2f;
  //Move forward and back 
  // if(TopChain.isSpinning() || BottomChain.isSpinning()) {
  //   //Save direction
  //   directionType currentDir = TopChain.direction();
  //   Controller.Screen.clearScreen();
  //   Controller.Screen.print((directionType)currentDir == forward ? "reverse" : "forward");
  //   //Forward and back
  //   TopChain.spin(((directionType)currentDir == forward ? reverse : forward));
  //   BottomChain.spin(((directionType)currentDir == forward ? reverse : forward));
  //   wait(moveTime,sec);
  //   //Restart chain lift
  //   TopChain.spin(currentDir);
  //   TopChain.spin(currentDir);
  // } else {
  TopChain.spin(forward);
  BottomChain.spin(forward);
  Intake.spin(forward);
  wait(moveTime,sec);
  TopChain.spin(reverse);
  BottomChain.spin(reverse);
  Intake.spin(reverse);
  wait(moveTime,sec);
  TopChain.stop();
  BottomChain.stop();
  Intake.stop();
  // }
}

timer t = timer();

void spinUpAdv() {
  //Disable block thing (adv logic) for 5 sec
  //
  
  if(t.value() > 5) {
    t.reset();
  }
  spinUp();
}

void advSpinCheckLogic() {
  if(t.value() > 5) {
    if(distanceSensor.objectDistance(distanceUnits::mm) < 50) {
      // Brain.Screen.clearScreen();
      // Brain.Screen.setCursor(1,1);
      // Brain.Screen.print(distanceSensor.objectDistance(distanceUnits::cm));
      
      TopChain.stop();
    }
  }
}

//? Sorter
void calibrateDoor() { //* calibrateSortingDoor
  /*
  Set Max torque
  Set Timeout
  Close door
  Reset position to be zero
  Reset Max torque
  */
 sorter.setMaxTorque(20,pct);
 sorter.setTimeout(1,sec);
 sorter.spinFor(143,degrees); //Closes door
 sorter.resetPosition();
 sorter.setMaxTorque(50,pct);
}

bool isDoorAuto = true;
bool isdoorOpen = false;
void openDoor() {
  if(isdoorOpen) return;
  sorter.setTimeout(2,sec);
  sorter.spinFor(-155,degrees);
  isdoorOpen = true;
}
void closeDoor() {
  if(!isdoorOpen) return;
  sorter.setTimeout(2,sec);
  sorter.spinFor(155,degrees);
  isdoorOpen = false;
}

void checkDoor() {
  if(colorSensor.isNearObject()) {
    if(colorSensor.hue() >= 150 && colorSensor.hue() <= 310) { //* blue ish
      openDoor();
    } else { //* red ish
      closeDoor();
    }
  }
}

void toggleDoorAuto() {
  //! print on scree
  isDoorAuto = !isDoorAuto;
}

//! Emergency Stop (Stop all motors)
void stopAll() {
  BottomChain.stop();
  TopChain.stop();
  Intake.stop();
} 


void goDeg(int degIn) {
  LeftWheel.spinFor(degIn, deg,false);
  RightWheel.spinFor(degIn, deg,true);
}
void turnDeg(int degIn) {
  dTrain.turnFor(degIn,deg);
}
void faceYellow() {
  // while(true) {
    AIVision18.takeSnapshot(AIVision18__COLOR1);
    if(abs(AIVision18.objects[0].centerX - 160) > 10) {
      if(AIVision18.objects[0].centerX - 160 < 0) {
        dTrain.turn(left,20,velocityUnits::pct);
      } else {
        dTrain.turn(right,20,velocityUnits::pct);
      }
    }
    while(abs(AIVision18.objects[0].centerX - 160) > 3) {
      // break;
      AIVision18.takeSnapshot(AIVision18__COLOR1);
    }
    dTrain.stop();
  // }
}

/*
----------------------------------------------------------------------
Controller buttons
----------------------------------------------------------------------
*/
int moveSpeedPct = 100;
int turnSpeedPct = 80;
int speedFactor = 20;

void displayInfo() {
  // Controller.Screen.clearScreen();
  Controller.Screen.setCursor(1,1);
  Controller.Screen.print("Move Speed: %d", moveSpeedPct);
  Controller.Screen.setCursor(2,1);
  Controller.Screen.print("Turn Speed: %d", turnSpeedPct);
}

void moveSpeedUp() {
  moveSpeedPct += (moveSpeedPct + speedFactor > 100 ? 0 : speedFactor);
}
void moveSpeedDown() {
  moveSpeedPct -= (moveSpeedPct - speedFactor < 0 ? 0 : speedFactor);
}

void turnSpeedUp() {
  turnSpeedPct += (turnSpeedPct + speedFactor > 100 ? 0 : speedFactor);
}
void turnSpeedDown() {
  turnSpeedPct -= (turnSpeedPct - speedFactor < 0 ? 0 : speedFactor);
}

void printController(char c) {
  Controller.Screen.clearScreen();
  Controller.Screen.setCursor(1,1);
  Controller.Screen.print(c);
}

void fillScreen(color col) { 
  Brain.Screen.setPenColor(col);
  Brain.Screen.setFillColor(col);
  Brain.Screen.drawRectangle(0, 0, 480, 240);
}

void setup() {
  fillScreen(color::yellow);
  BottomChain.setVelocity(100,pct); //75 matchs top speed
  TopChain.setVelocity(100,pct);
  Intake.setVelocity(100,pct);
  sorter.setVelocity(100,pct);
  sorter.setStopping(brake);
  colorSensor.setLightPower(75,pct);
  colorSensor.setLight(ledState::on);
  InertialSensor.calibrate(3);
  wait(3,sec);
  //Set door
  calibrateDoor();
  calibrateLever();
  Lever.setMaxTorque(100,pct);
  Lever.setVelocity(100,pct);
  Lever.setStopping(hold);
  fillScreen(color::orange);
}
void pre_auton(void) {
  
}

void autonomous(void) { //Start Right side
  fillScreen(color::green);
  dTrain.setTurnThreshold(0.7);
  // dTrain.turnFor(180,deg);2300.
  // return;
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
  InertialSensor.setHeading(23,deg);
  RightWheel.setVelocity(60,pct);
  LeftWheel.setVelocity(60,pct);
  dTrain.setTurnVelocityMin(25);
  closeDoor();
  // while (true){
  //   faceYellow();
  //   wait(3,seconds);
  // }
  
  // wait(100,sec);
  spinUp();
  goDeg(1100);
  TopChain.stop();
  removeClog();
  BottomChain.spin(forward);
  intakeIn();
  dTrain.turnFor(100,deg,true);
  goDeg(1500);
  
  dTrain.setTurnVelocityMin(15);
  dTrain.turnToHeading(180,deg);
  
  goDeg(-200);
  lowerLever();
  // dTrain.turnToHeading(180,deg);
  RightWheel.setVelocity(100,pct);
  LeftWheel.setVelocity(100,pct);
  // faceYellow(); //! keep this maybe?
  goDeg(900);
  raiseLever();
  lowerLever();
  wait(1,sec);
  // for(int i = 0; i < 1; i++){
  //   removeClog();
  //   spinUp();
  //   goDeg(-100);
  //   goDeg(200);
  // }    
  RightWheel.setVelocity(60,pct);
  LeftWheel.setVelocity(60,pct);
  dTrain.turnToHeading(180,deg);
  spinUp();
  goDeg(-900);
  // removeClog();
  TopChain.setVelocity(100,pct);
  BottomChain.setVelocity(100,pct);
  spinUp();
  // wait(2,sec);
  spinUp();
  
   
}

//! BUG: When named usercontrol, it seems to run anyways despite being commented out ¯\(ツ)/¯
//! Simply just changed the name
void usercontroler(void) {
  fillScreen(color::orange);
  // Controller.rumble("...---...");
  
  // 1 is top, 2 is bottom
  Controller.ButtonR1.pressed(lowerLever);
  Controller.ButtonR2.pressed(spinUpAdv);
  Controller.ButtonL1.pressed(raiseLever);
  Controller.ButtonL2.pressed(spinDown);
  Controller.ButtonA.pressed(removeClog);
  Controller.ButtonB.pressed(stopAll);
  // Controller.ButtonX.pressed();
  // Controller.ButtonY.pressed();
  Controller.ButtonUp.pressed(moveSpeedUp);
  Controller.ButtonDown.pressed(moveSpeedDown);
  Controller.ButtonLeft.pressed(turnSpeedDown);
  Controller.ButtonRight.pressed(turnSpeedUp);
  LeftWheel.spin(forward);
  RightWheel.spin(forward);
  calibrateLever();
  fillScreen(color::green);
  while (true) {
    // Controller.Screen.clearScreen();
    // Controller.Screen.setCursor(0,0);
    // Controller.Screen.print(colorSensor.hue());
    displayInfo();
    advSpinCheckLogic();
    if(isDoorAuto) {
      
      checkDoor();
    }
    int axis3 = Controller.Axis3.position();
    int axis1 = Controller.Axis1.position();
    if(axis1 >= 80 || axis1 <= -80) {
      LeftWheel.setVelocity(axis1 * turnSpeedPct/100,pct);
      RightWheel.setVelocity(-axis1 * turnSpeedPct/100,pct);
    } else {
      LeftWheel.setVelocity(axis3 * moveSpeedPct/100,pct);
      RightWheel.setVelocity(axis3 * moveSpeedPct/100,pct);
    }
    int axis2 = Controller.Axis2.position();
    // if(axis2 > 10) {
    //   spinUp();
    // } else if (axis2 < -10) {
    //   spinDown();
    // }
    // if(!isDoorAuto) {
    //   int axis4 = Controller.Axis4.position();
    //   if(axis4 > 10) {
    //     openDoor();
    //   } else if (axis4 < -10) {
    //     closeDoor();
    //   }
    // }
    wait(20, msec); // Sleep the task for a short amount of time to prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  setup();
  autonomous();
  fillScreen(color::red);
  wait(10,seconds);
  usercontroler();
  // Set up callbacks for autonomous and driver control periods.
  // Competition.autonomous(autonomous);
  // Competition.drivercontrol(usercontroler);

  // Run the pre-autonomous function.
  // pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
