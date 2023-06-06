/* ************************************************************************************************
      TX EMG for Stroke Study Application Dual Streams - One to Dashboard other to SD Card Board
 ************************************************************************************************

  This program provides the functions needed to collect data from the KTR2 EMG pre amp
  saves burst raw emg data to SD card and rectified EMG and IMU to transmit wirelessless on XBee.

  Code written for Feather Adalogger M0 and KTR2 sensor

  Based on code written initially for the Hilbert Huang Project by MFP
  This version now integrates Xbee to transmit the rectified data in real time
  and the raw data stored to the SD card.  The movement of the data acquision system is
  recorded using a BNO055 IMU while the EMG electrodes are located on the muscle of interest
  such as biceps.  
  
  There are hardware sensors such as gFORCE armband system for gaming http://www.oymotion.com/en/product32/149
  Other EMG sensors such as SEN 13723 (Digikey and Sparkfun)
  this low cost OYMotion sensor: 
  https://www.robotshop.com/ca/en/gravity-analog-emg-sensor.html?gclid=EAIaIQobChMIioi08J727QIVMBitBh1IPgHlEAQYASABEgJNwPD_BwE
  use gel electrodes and require additional electronics anyway.
  And the Grove:
  https://www.robotshop.com/ca/en/seeedstudio-grove-emg-detector.html
  
  These electrodes use the gel electrode button but function as dry surface electrodes:
  https://www.robotshop.com/ca/en/non-gelled-reusable-electrodes-10pk.html

  For future applications all of these hardware sensors have potential, but the key is the user interface to the PC dashboard
  and the synchronized IMU. The whole system is constrained by commmunications bandwidth if a low cost, simple to use 
  real time solution is required, as is true in this application where it will be used remotely by a non-specialist.

  
  Martin Ferguson-Pell Started March 29 2019  VER 1.0
  Revised to Add a Force Channel  June 24 2019
  Revised to Add IMU and XBee December 29 2020
  Revised February 15 2021 to complete SD Card saving function
  Revised April 2023 to communicate with a wireless data saving board simulaneously with Dashboard


 ************************************************************************************************

  WHAT THIS PROGRAM DOES:

  The BNO055 IMU continuously senses the movement of the lower arm. The ulitmate goal is to use the sensor
  to measure the angular velocity of the limb segment during the assessment. Additionally the IMU is used
  to measure the orientation of the arm by looking at the x,y,z orientation of the IMU sensor. However due to the complexity of
  calculating accurately in the boady frame of reference elbow flexion, for now we are using an electrogonometer.  In addition
  there is a force sensor and an AMG sensor.
  A switch on the electronics package starts the measurement sesson.
  The sensors' data is stored continuously to the SD card and also streamed out to a real time dashboard on a PC
  until the switch is pressed again. XBee S1 peer to peer in API mode is used to acheive adequate
  sampling rate of this data stream. The switch is an on-off press button switch (yellow).

  A two-electrode EMG sensor is atttached to the electronics package.  The data from the sensor is RAW EMG
  and so this program stores the raw data to the remote SD card and also rectifies it to send out to dashboard using XBee.
  The dashboard cannot acquire data fast enough for raw EMG and that is why we use the remote SD card data acquisition board.

  The datastream from AD2 is the raw EMG. We are sampling this at about 1kHz.  The program detects at the start the baseline level 
  and the noise level as a standard deviation.
  
  For rectified EMG we only need the equivalent of 100 Hz.  To use a moving window average or a low pass filter 
  would not reduce the sampling rate that is output by the XBee and would overwhelm the PC.  Therefore we adopt a simple average of the
  abs values of 10 samples of raw EMG then 10 more not as a moving average,even when there is no contraction. 
  This is sent to the XBee along with the IMU values which are also sample at 100 Hz. 

  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                       INCLUDE THE DEFINITIONS LIBRARIES AND VARIABLE DECLARATIONS
  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/*  MAKE SURE ALL  LIBRARY FILES ARE SAVED IN THE LIBRARY DIRECECTORY OF C:/ PROGRAM FILES (X86) ARDUINO .... 
OR IN MAC DOCUMENTS ARDUINO LIBRARY */

#include <SD.h>                         // include the SD library:
#include <SPI.h>                        // include the SPI library for SD card management
#include <elapsedMillis.h>              // To estimate the time elapsed since start of test
#include <math.h>
#include <Average.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <XBee.h>
//#include <MovingAverage.h>
#define ERROR_NOERROR        (0x00)
#define cardSelect 4                    // For Logger SD card
#define SAMPLE_PERIOD_US     (1000 * 10)
#define VBATPIN A7                      // To monitor as the battery voltage
#define STATE_IDLE           (0x00)
#define STATE_ACTIVE         (0x01)


// General public variables declarations *******************

/* NOTE BASIC RULE OF C++ IS THAT THE FOLLOWING VARIABLES ARE DECLARED GLOBAL AND ARE "KNOWN" AND CAN BE CHANGED 
     THROUGHOUT THE PROGRAM. TRY TO USE PASS BY REFERENCE FOR VARIABLES THAT ARE SPECIFIC TO CERTAIN FUNCTIONS
     IF A VARIABLE IS DECLARED WITHIN { } IT IS A LOCAL VARIABLE.  THIS INCLUDES WITHING IF OR FOR LOOPS.  
     TO REFERENCE A LOCAL VARIABLE OUT OF A FUNCTION PLACE & IN FRONT OF THE VARIABLE IN THE LIST OF VARIABLES 
     PASSED TO THE FUNCTION (THIS IS NOT THE SAME USE OF & AS YOU WOULD USE IT FOR GETTING THE ADDRESS OF 
     A VARIABLE WHICH WOULD ONLY HAPPEN IN THE BODY OF THE FUNCTION.*/


elapsedMicros timeElapsed;              //declare global if you don't want it reset every time loop runs
uint32_t lastTime;                      // unsigned 32 bit integer measures number of microseconds in loop
uint32_t currentTime;                   // Current time in micro secs since start of program
long runTime;                           // elapsed time
uint32_t startTime;                     // Time at start of loop in microseconds
uint32_t procTime;                      // time spent saving and processing
uint32_t currentTimegon = micros();     // Current time in micro secs of the goniometer measurements
uint32_t runTimegon ;                   // gives a measure of sampling rate in micros
uint32_t lastTimegon;

// COUNTERS
long i = 0;                              // Large Counter
long j = 0;                              // Loop counter used for spefic purpose on RMS header
int r = 0;                              // Return state of file save in Loop
int k = 0;                              // Counter used to empty the array to sd card
int contraction=0;                      // Is there a contraction (=1) or not (0) Start assuming there is to avoid blank saves in loop
int ii;                                 // Counter used for goniometer
// DATA ARRAYS
char inData[20];                        // Allocate some space for string
char inChar = -1;                       // Where to store the character read
bool on = false;                        // Use for LED p13 monitor

// **************************************** Adjustables *****************************************
const long contrnumReadings = 800;
const int numReadings = 10;
const int precontrnumReadings = 250;    // Number of readings to put in front and behind the contraction
float sdthresh = 10;                                // Set the standard deviation threshold to avoid noise triggering a burst
// **********************************************************************************************

int val = 0;                            // General purpose variable
int contrest = 0;                       // The value to trigger sd card reading
Average<int> ave1(200);                 // Defines the array averaged for Analog Input CH4
Average<long> avesamp(200);             // Defines the array for runtime to give estimated sampling rate
static int dataset[contrnumReadings];   // Array for raw data 6000 rows, 1 cols @ 1000Hz = 6s per contraction
int *pntrdataset = &dataset[0];         // The compiler will only accept max of ~6000 values in this array
                                        // We therefore have the capacity to store 6K of raw int value
int readings1[numReadings];             // Sets up an array for the circular array
int scratch1[numReadings];              // scratch array for smoothed data
static int contrscratch[precontrnumReadings];  // scratch array for pre-post contraction trace
int *pntcontrscratch = &contrscratch[0];  // Associated pointer
static int contrreadings[precontrnumReadings]; // Sets up array for the pre contraction circular array
int *pntcontrreadings = &contrreadings[precontrnumReadings];    // Associated pointer
int readIndex = 0;                      // The index of the current reading
long total1 = 0;                        // The running total
long total2 = 0;                        // The running total
int avevalue1 = 1610;                   // Average defining the baseline, start with 1650/1024 which is roughly half ref value of Feather
int averagecom = 20;                    // Average for threshold that defines contraction with baseline removed start nominal
int average1;
int average2;
int average3;
int aboutzerovalue1 = 0;                // The EMG value with baseline removed
int absvalue1 = 0;                      // Absolute value of EMG signal
float sdval1;                           // Standard deviation of baseline data
String sideofbody_str = "Left";

float sdvalcom;

// LOOP COUNTING
int loopcount = 0;                      // Loop counter must be an integer for Switch to work properly
uint32_t looplong = 0;                  // Loop counter to count number of loops for contraction
int loopforave = 0;                     // Loop counter to determine when to send sampled values to XBee

const uint8_t headerch = 0x7F;          // This is the headercharacter used by the Dashboard to identify start of packet
const uint8_t headerchend = 0x7E;         // This stops the receiver
int run;                                // Controls start or stop of loop
uint8_t errno;                          // error number for sd card
char filename[25];                      // Sets up a character array to enable filename to be incremented
float estsamplingRate;                  // Frequency during baseline measurement (runtime for first 200 samples / 200)
long estsamplingInterval;               // Interval that includes the delay in case 1

float measuredvbat = analogRead(VBATPIN);   // Variable for the battery voltage

float JointAnglemeas=analogRead(2);
float JointAngle;                          // Measure the voltage from the potentiometer of the goniometer starting with zeros
int JointAnglePacket;                     // Convert to int so that it can be put in packet
float JointAngleVelocity;
float JointAngleArray[30]; 
int JointAngleVelocityPacket;             // Convert to int so that it can be put in packet

// *********** Assign pins *********************************

const int buttonPin = 10;                // Stop and Start data collection ... active low  hardware pin 5
                                        // Since we are powered from BAT we need a pull up resistor (10k)
const int greenLED = 12;                 // General purpose green LED normally off
const int redLED = 13;                  // Red Led used for error status
const int bodySIDE = 5;                 // Slider switch to indicate left or right measurement
const int greenBUILTIN=8;                 // Green builtin led next to SD card
const int buzzer = 12;    

String SIDE;
// YELLOW LED IS DIRECTLY WIRED TO INDUCTION COIL OUTPUT TO INDICATE CHARGING
// RED LARGE LED IS DIRECTLY WIRED TO THE POWER SUPPLY OF THE SYSTEM

int runbuttonstatus;                // Stop and Start data collection
int sidebuttonstatus;               // Left or right arm

// General public variables declarations for ADC *******************

int value0; // Force  AD0
int value1; // Not used AD1
int value2; // Goniometer
int value3; // EMG AD2
int value4; // MMG1 Z AD3
int value5; // MMG2 Z AD4
int value6; // Pot for force threshold AD5

int valuetry;
float ZeroForce;
   
/* Note the A-REF pin is set to default 3.3 volts on Feather linked to supply which defines range 0 - 3.3V */

File logfile;                          // Establishes variable filename as type File

/* Setting up the XBee */

/* *********************** COMMUNICATION ************************************** */

XBee xbee_1 = XBee();                                     // Set up XBee 1 = Channel 1
XBeeResponse response_1 = XBeeResponse();
Rx16Response rx16_1 = Rx16Response();


XBee xbee_2 = XBee();
XBeeResponse response_2 = XBeeResponse();
Rx16Response rx16_2 = Rx16Response();


// use this to reply to TX 
uint8_t payload_response1[1];
uint8_t payload_response2[1];


Tx16Request tx_response1 = Tx16Request(0x5555, payload_response1, sizeof(payload_response1));   // Channel 1
Tx16Request tx_response2 = Tx16Request(0x6666, payload_response2, sizeof(payload_response2));   // Channel 2

// create an open request object targeting address 0x5555 on the Rx XBee with the given payload
// (overwrite the payload later to modify it)
//  PAYLOAD SET UP
uint8_t payload_1[12];
uint8_t payload_2[16];

Tx16Request tx_1 = Tx16Request(0x5555, payload_1, sizeof(payload_1)); // To the Dashboard Xbee
Tx16Request tx_2 = Tx16Request(0x6666, payload_2, sizeof(payload_2)); // To the Raw EMG SD Card storage Xbee

// state control
uint8_t state = STATE_ACTIVE;
// If you don't get a reply from the RX XB then the system hangs at this point

void flashLED(uint8_t LED, uint8_t times) {
    while(times > 0) {
        digitalWrite(redLED, HIGH);
        delay(100);
        digitalWrite(redLED, LOW);
        delay(100);

        times--;
    }
}


//****************************************** CREATE THE SETUP FUNCTION *********************************************************

void setup() {

  pinMode(13, OUTPUT);                        // Sets up the Red LED pin next to USB port
  pinMode(13, LOW);
  pinMode(12, OUTPUT);                        // This is the buzzer for the force threshold
  pinMode(12, LOW);
  pinMode(8, OUTPUT);                         // Green LED on Feather next to SD Card
  pinMode(8, LOW);
  pinMode(6, OUTPUT);                         // GREEN LARGE LED 
  pinMode(6, LOW);
  //pinMode(10,INPUT_PULLUP);
 pinMode(10,INPUT);                          // For new sensor with the normally closes pushbutton
 pinMode(5,INPUT_PULLUP);
  i = 0;
  
  Serial.begin(57600); 
    delay(100);
    
/*  ONLY USE IF YOU MUST HAVE THE SERIAL PORT CONNECTION FOR THE APPLICATION TO FUNCTION
 *   OTHERWISE THE PROGRAM WILL HALT WAITING FOR THE SERIAL TO BE CONNECTED
 *   
    {
#ifndef ESP8266
  while (!Serial);     // will pause Feather Adafruit Adalogger M0 until Serial Port opens
#endif
}
*/
  Serial1.begin(57600);                         // Starts the serial channel for XBee  
  
   
  xbee_1.setSerial(Serial1);
  xbee_2.setSerial(Serial1);
  delay(100); 

{
#ifndef ESP8266
  while (!Serial1);     // will pause Feather Adafruit Adalogger M0 until Xbee opens
#endif
}

  
   // Get the battery voltage before changing AD resolution
   
      measuredvbat = analogRead(VBATPIN);
    
     // Serial.print("RAW Battery Voltage "); Serial.println(measuredvbat);
      
      measuredvbat *= 2;  // We divide by 2 in the hardware so multiply back
      measuredvbat *= 3.3; // Multiply by our reference voltage
      measuredvbat /= 1024; // Convert to voltage
      
   Serial.print("Battery Voltage "); Serial.println(measuredvbat); 
    
   analogReadResolution(16);

digitalWrite(greenLED, HIGH);
delay(500);
digitalWrite(greenLED, LOW);

delay(1000);
  
  /******************************** Set all the loop counters, elapsed time and arrays to zero *************************/

  readIndex = 0;                          // Just to make sure that the index in smoothing array starts at zero
  run = 0;                                // Sets the start condition of loop starter
  total1 = 0;
  total2 - 0;
  loopcount = 0;
  lastTime = micros();                    // record the start time just before the end of "setup"

    loopcount = 0;
    looplong = 0;
    loopforave = 0;
    startTime = micros();
    runTime = 0; 

    Serial.println("Starting to sample data:");      //  Only on monitor
    Serial.println("Press Button to Start Sampling:");
    
   digitalWrite(greenLED, HIGH);             // Green LED on lid to indicate starting to transmit 
   digitalWrite(redLED, LOW); // We are going into the loop and green is HIGH red comes on when we save raw EMG

/* *************** ZERO FORCE SENSOR ****************** */
  
  value0 = analogRead(0);

  ZeroForce = 42.67*log(value0) -360.9; // Make sure the force sensor is not loaded

}                                         // END OF SETUP


// ************************************************************ THE LOOP FUNCTION ********************************************************

void loop() {

 
// IMPORTANT NOTE:YOU ARE MUCH BETTER OPENING THE LOGFILE IN SETUP AND LEAVING IT OPEN
// AND THEN FOLLOW UP WITH AN INFINITE FOR LOOP THAT CAN BE BROKEN AND THE FILE CLOSED WITH THE BUTTON
// FEATHER DOES NOT TOLERATE AMBIGUOUS SITUATIONS WHERE FILE COULD BE OPENED AND THEN REOPENED DUE TO COMPLEX
// CONDITIONAL FUNCTIONS SUCH AS THOSE IN THIS PROGRAM. JUST LET IT KEEP ON WRITING.

// *********** Button Press to Start Loop
while(digitalRead(10) == 1){
digitalWrite(LED_BUILTIN, HIGH);
digitalWrite(greenLED, HIGH);
digitalWrite(greenBUILTIN, HIGH);
runbuttonstatus = digitalRead(10);

  delay(300);  // Flashing the LEDs
  digitalWrite(LED_BUILTIN, LOW);
digitalWrite(greenLED, LOW);
digitalWrite(greenBUILTIN, LOW);
delay(300);  // Flashing the LEDs
  
}

delay(2000);                     // Time let go of the push button

/* ######################################################################################################################### */
// ############################################ THIS IS THE START OF THE MAIN LOOP #########################################
/* ######################################################################################################################### */

for(;;){                                            // Infinite loop flashing red LED when button pressed
    
    runbuttonstatus = digitalRead(10);                // Stop and Start data collection status digitalRead(10)
    //Serial.print("Button Status:  "); Serial.println(runbuttonstatus);
    
// ******************    This stops the acquisition on button press
       
if (runbuttonstatus == LOW) {                                  // The button to stop run is on pin 10
  
        //stopsendoutpackets();       // Stops the receiver
        
        Flash_Led();  
        //logfile.println("Button stopped at start of loop");   // Print if the logfile is closed
        //logfile.close();
        digitalWrite(greenLED, LOW);
        Serial.print("Button stopped at end of loop");
      
        for(;;){                                            // Infinite loop flashing red LED when button pressed
          digitalWrite(redLED, LOW);
          
          Flash_Led();
        }
           
      }
      
 // ***************** Now the Acquisition Functions Start

digitalWrite(greenLED, HIGH);
digitalWrite(greenBUILTIN, HIGH);

  if (loopcount <= 200) {
    //logfile.println(loopcount);
    baseline();                               // Collects baseline values as moving average
  }
  
  else if (loopcount == 201) {
    baselineaverage();                      // Calculates the average and sd of baseline
    //baselinedata();                         // Saves the baseline data to SD card and sets up Header for rest of data  

  }

  else if (loopcount > 202) {
    loopcount = 203;                         // We reset loopcount to a fixed value now to stop it getting too large
                                             // Setting it above 202 makes sure we don't repeat the baseline next pass
  
  collect_force_data();                      // collects one sample of FORCE data

  Goniometer();                             // Get joint angle data
  
  collect_emg_data();                       // Collect data from analog channel once every loop
  
  collect_amg_data();                       // Collect data from analog channel once every loop
  
  smoothanalogchan();                      // Each time we go around the loop from now on we smoooth the value
                                             // with the previous (numReadings) reading. This gives us the rectified
                                             // data array that can be sent out in real time by XBee with IMU data 

  sendoutpackets_Xbee1();                         // Send out the payload to XBee receiver and dashboard system
  sendoutpackets_Xbee2();                         // Send out the payload to XBee receiver and dashboard system
    
  }
  contraction=0;                          // If the contraction was saved reset and keep zero if no contraction last time

  loopcount++;                                // This loopcount is just used to determine if we are getting baseline data
  
  loopforave++;                               // This integer loop counter is to determine when 10 values have been collected
  
                                            // and their average needs to be sent out on XBee
                                            
  delay(1);                               // Set sampling rate here to a ojt 1 KHz if possible
  
  } // End of the Main While Loop
  
 //logfile.close();
 
 digitalWrite(greenLED, LOW);

 
// End of sampling
}

/* =========================  END OF THE LOOP ================================ */


// ************************************************************************************************************************************
// ****************************************************  FUNCTIONS  *******************************************************************
// ************************************************************************************************************************************


// *************************************************************** BASELINE() **********************************************************
// ***************************************************** FUNCTION TO COLLECT EMG BASELINE **********************************************/
// *************************************************************************************************************************************

void baseline()  {
  //Serial.println("In baseline measuring RAW EMG");
  
  value3 = analogRead(3);                              // Read Channel 3 of Adalogger M0 ADC  Raw EMG
                                                       
  ave1.push(value3);                                   // Calls the average function from math/h
                                                       // and the value is assigned to (value3) bucket
}
// End of function

// ************************************************************  BASELINEAVERAGE() *****************************************************
// *************************************************** FUNCTION TO CALCULATE THE MEAN AND STDEV OF BASELINE DATA ***********************
// *************************************************************************************************************************************

void baselineaverage() {
//Serial.println("In baseline average");

  avevalue1 = ave1.mean();                           // Calculates baseline value and assigns mean and SD
  
  sdval1 = ave1.stddev();                           // SD gives us a measure of the resting activity noise level
   
}
// End of Function


// ********************************************************* COLLECT_FORCE_DATA() *******************************************************
// ************************************  FUNCTION TO COLLECT FORCE DATA EACH PASS OF THE LOOP  ******************************************
// *************************************************************************************************************************************

void collect_force_data(){

// THIS IS THE CALIBRATION APRIL 18 2023

//y = 42.672ln(x) - 360.9  with r^s of 0.98

value0= (analogRead(0));
valuetry = 42.67*log(value0) -360.9 - ZeroForce;

//valuetry = (analogRead(0)*0.0018 + 15.216) - ZeroForce;  // Cal in Newtons

value6= (analogRead(5));

if (value0 - value6 >0) {
  digitalWrite(buzzer,HIGH);
}
Serial.print("Force:  "); Serial.print("\t"); Serial.println(value0);
Serial.print("ForcePot:  "); Serial.print("\t"); Serial.println(value6);

} // End of function

// ******************************************************* COLLECT_EMG_DATA ************************************************************
// ************************************  FUNCTION TO COLLECT EMG DATA EACH PASS OF THE LOOP  *******************************************
// *************************************************************************************************************************************

void collect_emg_data() {                                // Function to collect the emg data
  
  currentTime = micros();                                // Update current time ready for timestamp
  timeElapsed = currentTime - startTime;                 // Time Stamp in microseconds as global in micros now
  procTime = currentTime - lastTime;                     // procTime gives a measure of sampling rate in micros
  lastTime = currentTime;
  runTime = timeElapsed/1000;                            // Time Stamp in milliseconds
  
 // Serial.print("Loop Time micros:   "); Serial.println(procTime); // With serial port running this is about 2ms or 500Hz

  value3 = analogRead(3);                                // Read Channel 3 of Adalogger M0 ADC  Raw EMG
                                                         // NOTE: value3 is used by baseline() to get the average for the first
                                                         // 200 passes before we get to this function. From here value1 is being read
                                                         // just to collect the raw EMG from 201 onwards.  This works OK as we don't
                                                         // go back to baseline() but be aware. value3 is global.
                                                         
  
  aboutzerovalue1 = value3 - (avevalue1);                 // Remove the offset We are leaving aboutzerovalue1 and avevalue1 as 1 not 3

// Make the Raw EMG mV about the baseline  
      aboutzerovalue1 *= 3.3; // Multiply by our reference voltage
      aboutzerovalue1 /= 65536; // Convert to voltage as we set AD converter to 16 bit
      aboutzerovalue1 *=1000; // To make the value mV

  
  //Serial.print("AD CH 3:  "); 
  //Serial.println(aboutzerovalue1); // The raw value
  
}
// End of function

// ******************************************************* COLLECT_AMG_DATA ************************************************************
// ************************************  FUNCTION TO COLLECT EMG DATA EACH PASS OF THE LOOP  *******************************************
// *************************************************************************************************************************************

void collect_amg_data() {                                // Function to collect the emg data
  
  currentTime = micros();                                // Update current time ready for timestamp
  timeElapsed = currentTime - startTime;                 // Time Stamp in microseconds as global in micros now
  procTime = currentTime - lastTime;                     // procTime gives a measure of sampling rate in micros
  lastTime = currentTime;
  runTime = timeElapsed/1000;                            // Time Stamp in milliseconds
  
 // Serial.print("Loop Time micros:   "); Serial.println(procTime); // With serial port running this is about 2ms or 500Hz

  value4 = analogRead(4);                                // Read Channel 4 of Adalogger M0 ADC  Raw AMG
                                                     
}
// End of function


// ********************************************************* SMOOTHANALOGCHAN() ********************************************************
// ************************************************ FUNCTION FOR SMOOTHING IN REAL TIME ************************************************/
// *************************************************************************************************************************************
/* This smoothing function enables us to get a smoothed value to identify and keep running a contraction
    An alternative approach would be to use the rectified signal and set an amplitude threhold on the rectified signal.
*/

void smoothanalogchan() {                             
  
  // We are using the milli second elapsed time count

// First we will create a scratch array with (numReadings) of raw EMG with baseline removed (aboutzerovalue1)
// We also rectify by taking the abs value
// We nominally set numReadings to 10 which is the data block we are averaging
// First few times around the loop creates a false average (~10ms)

  i = 0;
  for (i = 0; i < numReadings - 1; i++) {             // Shift all the readings up the array by 1
    readings1[i] = scratch1[i + 1];                   // The nth element of scratch is n-1th of readings
  }

  // Put the new value at the bottom of the readings1 array
  
  readings1[numReadings - 1] = abs(aboutzerovalue1); // At this point i is equal to 1-numreadings so we are adding new readings
                                                     // to bottom of array readings1[]

  i = 0;                                                // Now reset i
  
  // Add up all the readings in the scratch array
  for (i = 0; i < numReadings; i++) {
    total1 = (total1 + readings1[i]);                 
  }
  
// total1 is now the sum of readings up to i readings
  
  average1 = total1 / numReadings;
  // To convert Rectified EMG signal to mV  
      average1 *= 3.3; // Multiply by our reference voltage
      average1 /= 65536; // Convert to voltage as we set AD converter to 16 bit
      average1 *=1000; // To make the value mV

//Serial.print("Rect Values:  ");
Serial.print("EMG Averaged"); Serial.print("\t");Serial.println(average1);

//logfile.print("Smoothed Rectified EMG values:  ");logfile.print(",");logfile.println(average1);  
  
  total1 = 0;                                         // Reset total1
  i = 0;

  for (i = 0; i < numReadings; i++) {                 // Set scratch up for next time round loop

    scratch1[i] = readings1[i];                       // The nth element of scratch is now nth of new readings arrays are the same now
  }


//return;
}
// End of function



// ************************************************ SENDOUTPACKETS() TO DASHBOARD ******************************************************
// *************************************************FUNCTION TO TRANSMIT XBEE DATA *****************************************************
// *************************************************************************************************************************************

void sendoutpackets_Xbee1() {        
//Serial.println("In sendout packets");
digitalWrite(greenLED, HIGH);

// Note the rectified EMG values are averaged but the rest are not


// Make sure your Tx and Rx XBees are in API=1 or dashboard will not work due to spurious characters in header. 
// NOTE:  There are four special characters used by XBee. They are 7E 7D 0x11 and 0x13.  So when you send for example 0x11 the receiving XBee
// will add a flag (7D) and then XOR the value with 0x20 so what you see transmitted appears to be 7D 31
// To resolve this when you have received the data you will need code to look for the 7D flag and then OR the byte after it to revert the
// value back to what was transmitted.  So...  0x11 is seen on the Rx side as 7D 31 . When you see the 7D ignore that flag and then XR the next
// byte with 0x20 and you will get back to 0x11 again as the inverse of XOR is XOR.  The operator for XOR is ^ in c and in python.

//value0 = analogRead(0); // FORCE
//value1 = analogRead(1); // NOT USED
//value2 = analogRead(2); // Goniometer
//value3 = analogRead(3); // EMG But we want to send the averaged data not raw
//value4 = analogRead(4); // AMG1
//value5 = analogRead(4); // AMG2

//Serial.print("FORCE:");Serial.println(value0);
//Serial.print("NOT USED:");Serial.println(value1);
//Serial.print("ANGLE:");Serial.println(value2);
//Serial.println(average1); // The EMG rectified signal
//Serial.print("AMG1:");Serial.println(value4);
//Serial.print("AMG2:");Serial.println(value5);

                // convert the timestamp (long)
                
                payload_1[ 0] = (headerch     ) & 0xff;     // The header character used by Dashboard to identify start of packet
                payload_1[ 1] = (headerch     ) & 0xff;
                payload_1[ 2] = (runTime >> 24) & 0xff;
                payload_1[ 3] = (runTime >> 16) & 0xff;
                payload_1[ 4] = (runTime >>  8) & 0xff;
                payload_1[ 5] = (runTime      ) & 0xff;

                payload_1[ 6] = (average1 >> 8) & 0xff;     // EMG  (H byte) (would normally be EMG) now in mV
                payload_1[ 7] = (average1     ) & 0xff;     // EMG  (L byte)
                payload_1[ 8] = (valuetry >>   8) & 0xff;   // Force Sensor Calibrated Value  (H byte)
                payload_1[ 9] = (valuetry       ) & 0xff;   // Force Sensor Calibrated Value (L byte)
                payload_1[ 10] = (value2 >>   8) & 0xff;   // Elbow Angle (H byte)
                payload_1[ 11] = (value2       ) & 0xff;   // Elbow Angle (L byte)
                // send it along
                xbee_1.send(tx_1);
                
       digitalWrite(greenLED, LOW);
    //return;         
            
 }   // End of Function
 

// ************************************************ SENDOUTPACKETS() FOR RAW EMG TO SD CARD RECEIVER ***********************************
// *************************************************FUNCTION TO TRANSMIT XBEE DATA *****************************************************
// *************************************************************************************************************************************

void sendoutpackets_Xbee2() {        
//Serial.println("In sendout packets");
digitalWrite(greenLED, HIGH);

// Note the rectified EMG values are averaged but the rest are not


// Make sure your Tx and Rx XBees are in API=1 or dashboard will not work due to spurious characters in header. 
// NOTE:  There are four special characters used by XBee. They are 7E 7D 0x11 and 0x13.  So when you send for example 0x11 the receiving XBee
// will add a flag (7D) and then XOR the value with 0x20 so what you see transmitted appears to be 7D 31
// To resolve this when you have received the data you will need code to look for the 7D flag and then OR the byte after it to revert the
// value back to what was transmitted.  So...  0x11 is seen on the Rx side as 7D 31 . When you see the 7D ignore that flag and then XR the next
// byte with 0x20 and you will get back to 0x11 again as the inverse of XOR is XOR.  The operator for XOR is ^ in c and in python.

//value0 = analogRead(0); // FORCE
//value1 = analogRead(1); // NOT USED
//value2 = analogRead(2); // Goniometer
//value3 = analogRead(3); // EMG But we also send out average1 which is the rectified EMG envelop
//value4 = analogRead(4); // AMG1
//value5 = analogRead(4); // AMG2

//Serial.print("FORCE:");Serial.println(value0);
//Serial.print("NOT USED:");Serial.println(value1);
//Serial.print("ANGLE:");Serial.println(value2);
//Serial.println(average1); // The EMG rectified signal
//Serial.print("AMG1:");Serial.println(value4);
//Serial.print("AMG2:");Serial.println(value5);

                // convert the timestamp (long)
                
                payload_2[ 0] = (headerch     ) & 0xff;     // The header character used by Dashboard to identify start of packet
                payload_2[ 1] = (headerch     ) & 0xff;
                payload_2[ 2] = (runTime >> 24) & 0xff;
                payload_2[ 3] = (runTime >> 16) & 0xff;
                payload_2[ 4] = (runTime >>  8) & 0xff;
                payload_2[ 5] = (runTime      ) & 0xff;

                payload_2[ 6] = (average1 >> 8) & 0xff;     // EMG  (H byte) (would normally be EMG) now in mV
                payload_2[ 7] = (average1     ) & 0xff;     // EMG  (L byte)
                payload_2[ 8] = (value0 >>   8) & 0xff;   // Force Sensor  (H byte)
                payload_2[ 9] = (value0       ) & 0xff;   // Force Sensor  (L byte)
                payload_2[ 10] = (value2 >>   8) & 0xff;   // Elbow Angle (H byte)
                payload_2[ 11] = (value2       ) & 0xff;   // Elbow Angle (L byte)
                payload_2[ 12] = (value3 >>   8) & 0xff;   // RAW EMG now in mV
                payload_2[ 13] = (value3       ) & 0xff;   // RAW EMG  (L byte)
                payload_2[ 14] = (value4 >>   8) & 0xff;   // AMG (H byte) now in mV
                payload_2[ 15] = (value4       ) & 0xff;   // AMG (L byte)
                // send it along
                xbee_2.send(tx_2);
                
       digitalWrite(greenLED, LOW);
    //return;         
            
 }   // End of Function

 
/*
void stopsendoutpackets() {        
//Serial.println("In sendout packets");
digitalWrite(greenLED, HIGH);

// Note the rectified EMG values are averaged but the rest are not


// Make sure your Tx and Rx XBees are in API=1 or dashboard will not work due to spurious characters in header. 
// NOTE:  There are four special characters used by XBee. They are 7E 7D 0x11 and 0x13.  So when you send for example 0x11 the receiving XBee
// will add a flag (7D) and then XOR the value with 0x20 so what you see transmitted appears to be 7D 31
// To resolve this when you have received the data you will need code to look for the 7D flag and then OR the byte after it to revert the
// value back to what was transmitted.  So...  0x11 is seen on the Rx side as 7D 31 . When you see the 7D ignore that flag and then XR the next
// byte with 0x20 and you will get back to 0x11 again as the inverse of XOR is XOR.  The operator for XOR is ^ in c and in python.


//value0 = analogRead(0); // FORCE
//value1 = analogRead(1); // NOT USED
//value2 = analogRead(2); // Goniometer
//value3 = analogRead(3); // EMG But we want to send the averaged data not raw
//value4 = analogRead(4); // AMG1
//value5 = analogRead(4); // AMG2

//Serial.print("FORCE:");Serial.println(value0);
//Serial.print("NOT USED:");Serial.println(value1);
//Serial.print("ANGLE:");Serial.println(value2);
//Serial.println(average1); // The EMG rectified signal
//Serial.print("AMG1:");Serial.println(value4);
//Serial.print("AMG2:");Serial.println(value5);

                // convert the timestamp (long)          
                payload_2[ 0] = (headerchend     ) & 0xff;     // The header character used by Dashboard to identify start of packet
                payload_2[ 1] = (headerchend     ) & 0xff;
                payload_2[ 2] = (runTime >> 24) & 0xff;
                payload_2[ 3] = (runTime >> 16) & 0xff;
                payload_2[ 4] = (runTime >>  8) & 0xff;
                payload_2[ 5] = (runTime      ) & 0xff;

                payload_2[ 6] = (average1 >> 8) & 0xff;     // EMG  (H byte) (would normally be EMG)
                payload_2[ 7] = (average1     ) & 0xff;     // EMG  (L byte)
                payload_2[ 8] = (value0 >>   8) & 0xff;   // Force Sensor  (H byte)
                payload_2[ 9] = (value0       ) & 0xff;   // Force Sensor  (L byte)
                payload_2[ 10] = (value2 >>   8) & 0xff;   // Elbow Angle (H byte)
                payload_2[ 11] = (value2       ) & 0xff;   // Elbow Angle (L byte)
                // send it along
                xbee_1.send(tx_1);
                
       digitalWrite(greenLED, LOW);
    //return;         
            
 }   // End of Function
*/
// *************************************************************************************************************************************
// ******************************** FUNCTION TO READ THE POTENTIOMETER GONIOMETER ******************************************************
// *************************************************************************************************************************************

void Goniometer() {
//Serial.println("In goniometer");

// Establish Left or Right Arm

sidebuttonstatus=digitalRead(bodySIDE);

// Calibration April 1 2023
// LEFT: Degrees = -0.0043* Computer Units + 141.62

// RIGHT: Degrees = 0.0038* Computer Units - 124.6

if (sidebuttonstatus == LOW) {
SIDE = "LEFT";
JointAngle = ((analogRead(2))*-0.0043 + 141.62);  // The value is sent to XBee
}
else
{
SIDE = "RIGHT";
JointAngle = ((analogRead(2))*0.0038 - 124.6);  // The value is sent to XBee
}
//Serial.print("Which Body Side?  "); Serial.println(SIDE);
value2= JointAngle;     // Comment out to get the raw values for calibration
//value2=analogRead(2);    // Use this to get raw values for calibration then comment out
Serial.print("Joint Angle"); Serial.print("\t"); Serial.println(value2);

// ********************** To measure the joint angle from goniometer *******************************

  JointAnglemeas =  analogRead(2);                // Measure the voltage from the potentiometer of the goniometer
  
  // Measure the goniometer voltage and provide value in volts
      
      JointAnglemeas *= 3.3; // Multiply by our reference volatage  *= and /= is shorthand way to multiply/divide one variable by another
      JointAnglemeas /= 1024; // Convert to voltage
      

 /* OLD CALIBRATION DAN's DEVICE     
  // CHECK THIS CALIBRATION
  JointAngle =  0.0787*JointAnglemeas*1000 - 52.418;    // Convert volts to degrees  degrees where 0 degrees = 180 (clinical protocol)
  JointAnglePacket =  JointAngle;  // Convert to integer to transmit
  */

ii++;  // increments each time we go around the main while loop

// ****************** To calculate joint angle velocity *********************************
if (ii<30) {
  JointAngleArray[ii]=JointAngle;
  
}

 else {
  ii=0; 
  }
 
if (ii==29) {

  currentTimegon = micros();                   // Current time in micro secs since start of program
  runTimegon = currentTimegon - lastTimegon;                     // runTime gives a measure of sampling rate in micros
  lastTimegon = currentTimegon;

  JointAngleVelocity = (abs(JointAngleArray[29] - JointAngleArray[1])/runTimegon)*1000000;    // To convert degrees per micor second to per second. NOTE UNSIGNED INTEGER FOR THE ANGLE MEASURES
  JointAngleVelocityPacket = JointAngleVelocity;
  
  //Serial.print("Joint Angle Goniometer 1:   "); Serial.print(JointAngleArray[29]);Serial.print("    Joint Angle Goniometer 2:   "); Serial.print(JointAngleArray[1]);Serial.print("    Joint Angle Packet   "); Serial.print(JointAnglePacket);Serial.print("     Joint Angle Velocity Packet Goniometer:   "); Serial.print(JointAngleVelocityPacket);Serial.print("    Elapsed Time:   "); Serial.print(runTimegon);Serial.print("    Counter:   "); Serial.println(ii);
}
  
  
  //                              return;
}
            
// *************************************************************************************************************************************
// ********************************  FUNCTION TO FLASH RED LED TO INDICATE ERROR OR TERMINATION OF PROGAM ******************************
// *************************************************************************************************************************************
void Flash_Led() {
  digitalWrite(redLED, HIGH);
  delay(200);
  digitalWrite(redLED, LOW);
  delay(200);
}
// End of function

// *************************************************************************************************************************************
// ********************************  SD CARD ERROR STATUS ******************************************************************************
// *************************************************************************************************************************************

// blink out an error code for SD card status
void error(uint8_t errno) {
  while (1) {
    uint8_t i;
    for (i = 0; i < errno; i++) {
      digitalWrite(13, HIGH);
      delay(150);
      digitalWrite(13, LOW);
      delay(150);
    }
    for (i = errno; i < 10; i++) {
      delay(100); 
    }
  }
}           
// *************************************************************************************************************************************       
// ************************************************************* END OF FUNCTIONS SECTION **********************************************
// *************************************************************************************************************************************
