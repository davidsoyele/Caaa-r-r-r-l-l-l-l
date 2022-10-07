#include "MeMegaPi.h"                               // Put this #include everytime

// Angle of the ultrasonic is 105 deg

MeUltrasonicSensor ultraSensor(PORT_6);             /* Ultrasonic module can ONLY be connected to port 3, 4, 6, 7, 8 of base shield. */
MeLineFollower lineFinder(PORT_5);                  // The line sensor. S1, first blaster, S2, second blaster.
MeMegaPiDCMotor motorR(PORT1B);                     // The 2 motors. PORTXY, where X represents the port and Y represents
MeMegaPiDCMotor motorL(PORT2B);                     // A for stepper and B for DC motor.

float kL = 1.7, kR = 1.7;
int RunSpeed = -115;  //calibrate these numbers first! Should be moderately slow until
int RunSpeedFast = -155;
int maxSpeed = 145;  //things are figured out and working
bool TurnState = false;
float newSpeedL = 0; //this is the time modified speed.
float newSpeedR = 0; //this is the time modified speed.
unsigned long startTime = 0;
//s1 = left
//s2 = right

void setup(){
    // No setup needed
    Serial.begin(9600);
}
void loop(){
    if(ultraSensor.distanceCm()>20.5){
        int sensorState = lineFinder.readSensors();               // Sensor reading
        switch(sensorState) { 
            case S1_OUT_S2_OUT: // On the line. GOTTA GO FAST!
                    TurnState = false;
                    motorR.run(RunSpeed);
                    motorL.run(-RunSpeed);
                
                    break;
            case S1_OUT_S2_IN: //turn left
                    TurnState = true;
                    startTime = millis();
                    newSpeedL = -RunSpeed;
                    newSpeedR = RunSpeed;
                    motorL.run(0);
                    while(lineFinder.readSensors() == S1_OUT_S2_IN){
                        newSpeedR = (float)RunSpeed - kL*(millis()-startTime);
                        if(newSpeedR > maxSpeed) newSpeedR = maxSpeed;
                        if(newSpeedR < -maxSpeed) newSpeedR = -maxSpeed;
                        motorR.run(newSpeedR);
    
                        Serial.print("Right motor speeding up: ");
                        Serial.println(newSpeedR);
                    }
                    motorR.run(RunSpeed);
                    motorL.run(RunSpeed);
                    break;
    
            case S1_IN_S2_OUT: //turn right
                    TurnState = true;
                    startTime = millis();
                    newSpeedL = RunSpeed;
                    //newSpeedR = RunSpeed;
                    motorR.run(0);
                    while(lineFinder.readSensors() == S1_IN_S2_OUT){
                        newSpeedL = (float)((-1)*RunSpeed) + kR*(millis()-startTime);
                        if(newSpeedL > maxSpeed) newSpeedL = maxSpeed;
                        if(newSpeedL < -maxSpeed) newSpeedL = -maxSpeed;
                        motorL.run(newSpeedL);
    
                        Serial.print("Left motor speeding up: ");
                        Serial.println(newSpeedL);
                    }
                    motorR.run(-RunSpeed);
                    motorL.run(-RunSpeed);
                    break;
            case S1_IN_S2_IN:  // Off line, SHIT GO BACK!!                         
                    if(TurnState==false){
                        unsigned long STime = millis();
              
                        int TSpeed = 120;
                        
                        if(lineFinder.readSensors() == S1_IN_S2_IN){
                            STime = millis();
                            while(millis() - STime <= 2000 || lineFinder.readSensors() != S1_IN_S2_IN){
                                motorR.run(TSpeed);
                                motorL.run(TSpeed);
                                if(lineFinder.readSensors() != S1_IN_S2_IN) break;
                            }
                        }
                        if(lineFinder.readSensors() == S1_IN_S2_IN){
                            STime = millis();
                            while(millis() - STime <= 4000 || lineFinder.readSensors() != S1_IN_S2_IN){
                                motorR.run(-TSpeed);
                                motorL.run(-TSpeed);
                                if(lineFinder.readSensors() != S1_IN_S2_IN) break;
                            }
                        }
                    }
                    break;
            default:
                    break;
        }
    }
    else{
        int LowerSpeed = RunSpeed;
        bool valid = true;
        while(LowerSpeed < 0)
        {
            LowerSpeed += 40;
            motorR.run(LowerSpeed);
            motorL.run(-LowerSpeed);
            delay(100);
        }
        for(int t=100; t<=800; t+=100)
        {
            if(ultraSensor.distanceCm()>20.5)
            {
                valid = false;
                break;
            }
            delay(100);
        }
        if(valid==true)
        {
            unsigned long STime = millis();
            while(millis()-STime <= 9000){
                motorR.run(5*RunSpeedFast/8);
                motorL.run(-5*RunSpeedFast/8);
            }
            int TSpeed = 120;
            if(lineFinder.readSensors() == S1_IN_S2_IN){
                STime = millis();
                while(millis() - STime <= 2000 || lineFinder.readSensors() != S1_IN_S2_IN){
                    motorR.run(TSpeed);
                    motorL.run(TSpeed);
                    if(lineFinder.readSensors() != S1_IN_S2_IN) break;
                }
            }
            if(lineFinder.readSensors() == S1_IN_S2_IN){
                STime = millis();
                while(millis() - STime <= 4000 || lineFinder.readSensors() != S1_IN_S2_IN){
                    motorR.run(-TSpeed);
                    motorL.run(-TSpeed);
                    if(lineFinder.readSensors() != S1_IN_S2_IN) break;
                }
            }
        }
    }
}

void gyroCalibration( ) {}
