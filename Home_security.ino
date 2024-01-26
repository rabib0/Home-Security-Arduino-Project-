#include <Wire.h>
#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include<SoftwareSerial.h>
SoftwareSerial B(A3,A2);
#define sensorPower 7
#define sensorPin A0

const int buzz = 13; //buzzer to arduino pin 9
const int pumpPin = 4; 
int value = 4;
int inputPin = 12;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
int motion;
int flame=0;
int water=0;
int binary_water=0;
int buzzing=0;


const byte numRows = 4;         //number of rows on the keypad
const byte numCols = 4;         //number of columns on the keypad
const int sensorMin = 0;     //  sensor minimum
const int sensorMax = 1024;  // sensor maximum
char keymap[numRows][numCols] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char keypressed;                 //Where the keys are stored it changes very often
char code[] = {'1', '2', '3', '4'}; //The default code, you can change it or make it a 'n' digits one

char check1[sizeof(code)];  //Where the new key is stored
char check2[sizeof(code)];  //Where the new key is stored again so it's compared to the previous one

short a = 0, i = 0, s = 0, j = 0; //Variables used later

byte rowPins[numRows] = {2, 3, 5, 6}; //Rows 0 to 3 //if you modify your pins you should modify this too
byte colPins[numCols] = {8, 9, 10, 11}; //Columns 0 to 3

LiquidCrystal_I2C lcd(0x27, 16, 2);
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
Servo myservo;
void setup()
{

  pinMode(buzz, OUTPUT); // Set buzzer - pin 9 as an output
  delay(3000);

  pinMode(inputPin, INPUT);     // declare sensor as input


  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);


    // Set D7 as an OUTPUT
  pinMode(sensorPower, OUTPUT);
 
  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);

  B.begin(9600);
  Serial.begin(9600);

  B.begin(9600);
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.begin (16, 2);
  lcd.setCursor(0, 0);
  lcd.print(" ENTER THE PIN");
  lcd.setCursor(1 , 1);

  lcd.print("  Press * ");  
}

 
 
 


int readSensor() {
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  delay(10);              // wait 10 milliseconds
  value = analogRead(sensorPin);    // Read the analog value form sensor
  digitalWrite(sensorPower, LOW);   // Turn the sensor OFF
  return value;             // send current reading
}


void askpin(){
   
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("*ENTER THE CODE*");
  loop();
 
 
  }



void OpenDoor() {            //Lock opening function open for 3s
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Access Granted");
  lcd.setCursor(4, 1);
  lcd.print("WELCOME!!");
  myservo.write(90);


}

void LockDoor() {            //Lock opening function open for 3s
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Door Locked");
  lcd.setCursor(4, 1);
  lcd.print("Secured");
  myservo.write(90);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENTER THE PIN");
  lcd.setCursor(1 , 1);
 
   lcd.print("  Press * ");
   
 }
 

 



void ReadCode() {                 //Getting code sequence
  i = 0;                    //All variables set to 0
  a = 0;
  j = 0;

  while (keypressed != 'A') {                                   //The user press A to confirm the code otherwise he can keep typing
    keypressed = myKeypad.getKey();
    if (keypressed != NO_KEY && keypressed != 'A' ) {     //If the char typed isn't A and neither "nothing"
      lcd.setCursor(j, 1);                                 //This to write "*" on the LCD whenever a key is pressed it's position is controlled by j
      lcd.print("*");
      j++;
      if (keypressed == code[i] && i < sizeof(code)) {       //if the char typed is correct a and i increments to verify the next caracter
        a++;
        i++;
      }
      else
        a--;                                               //if the character typed is wrong a decrements and cannot equal the size of code []
    }
  }
  keypressed = NO_KEY;

}

void ChangeCode() {                     //Change code sequence
  lcd.clear();
  lcd.print("Changing code");
  delay(1000);
  lcd.clear();
  lcd.print("Enter old code");
  ReadCode();                      //verify the old code first so you can change it

  if (a == sizeof(code)) {  //again verifying the a value
    lcd.clear();
    lcd.print("Changing code");
    GetNewCode1();            //Get the new code
    GetNewCode2();            //Get the new code again to confirm it
    s = 0;
    for (i = 0 ; i < sizeof(code) ; i++) { //Compare codes in array 1 and array 2 from two previous functions
      if (check1[i] == check2[i])
        s++;                                //again this how we verifiy, increment s whenever codes are matching
    }
    if (s == sizeof(code)) {        //Correct is always the size of the array

      for (i = 0 ; i < sizeof(code) ; i++) {
        code[i] = check2[i];       //the code array now receives the new code
        EEPROM.put(i, code[i]);        //And stores it in the EEPROM

      }
      lcd.clear();
      lcd.print("Code Changed");
      delay(2000);
    }
    else {                        //In case the new codes aren't matching
      lcd.clear();
      lcd.print("Codes are not");
      lcd.setCursor(0, 1);
      lcd.print("matching !!");
      delay(2000);
    }

  }

  else {                    //In case the old code is wrong you can't change it
    lcd.clear();
    lcd.print("Wrong");
    delay(2000);
  }
}

void GetNewCode1() {
  i = 0;
  j = 0;
  lcd.clear();
  lcd.print("Enter new code");   //tell the user to enter the new code and press A
  lcd.setCursor(0, 1);
  lcd.print("and press A");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("and press A");     //Press A keep showing while the top row print ***

  while (keypressed != 'A') {          //A to confirm and quits the loop
    keypressed = myKeypad.getKey();
    if (keypressed != NO_KEY && keypressed != 'A' ) {
      lcd.setCursor(j, 0);
      lcd.print("*");               //On the new code you can show * as I did or change it to keypressed to show the keys
      check1[i] = keypressed;   //Store caracters in the array
      i++;
      j++;
    }
  }
  keypressed = NO_KEY;
}

void GetNewCode2() {                        //This is exactly like the GetNewCode1 function but this time the code is stored in another array
  i = 0;
  j = 0;

  lcd.clear();
  lcd.print("Confirm code");
  lcd.setCursor(0, 1);
  lcd.print("and press A");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("and press A");

  while (keypressed != 'A') {
    keypressed = myKeypad.getKey();
    if (keypressed != NO_KEY && keypressed != 'A' ) {
      lcd.setCursor(j, 0);
      lcd.print("*");
      check2[i] = keypressed;
      i++;
      j++;
    }
  }
  keypressed = NO_KEY;
}


 
 
void loop(){



  val = digitalRead(inputPin);
  motion=val;
  B.print(motion);
  B.print(",");
  // read input value
  if (val == HIGH) {            // check if the input is HIGH

    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {

    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }

   
  int level = readSensor();
  water=level;
  B.print(water);
  B.print(",");
  if(water>400){
    binary_water=1;
    digitalWrite(buzz, HIGH);
    digitalWrite(pumpPin, LOW);
    buzzing=1;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Emergency");
  lcd.setCursor(4, 1);
  lcd.print("Exit!!");
  myservo.write(90);
   
    }
  else{
    binary_water=0;
    if (flame==0){
  
      digitalWrite(pumpPin, HIGH);
      buzzing=0;

    }
  }
   



  // read the sensor on analog A0:
  int sensorReading  = analogRead(A1);
  // map the sensor range (four options):
  // ex: 'long  int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading,  sensorMin, sensorMax, 0, 3);
 
  // range value:
  switch (range) {
  case 0:    // A fire closer than 1.5 feet away.
    flame=1;
    buzzing=1;
    digitalWrite(buzz, HIGH);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Emergency");
  lcd.setCursor(4, 1);
  lcd.print("Exit!!");
  myservo.write(90);
    break;
  case 2:    // No fire detected.
    flame=0;
    if (binary_water==0){
      buzzing=0;
      digitalWrite(buzz, LOW);
    }
    break;
  }

  
  B.print(flame);
  B.print(",");
  B.print(buzzing);
  B.print(";");




 keypressed = myKeypad.getKey();               //Constantly waiting for a key to be pressed
  if (keypressed == '*') {                    // * to open the lock
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("ENTER THE CODE");            //Message to show
    ReadCode();                          //Getting code function
    if (a == sizeof(code))        //The ReadCode function assign a value to a (it's correct when it has the size of the code array)
      OpenDoor();                   //Open lock function if code is correct
    else {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("CODE"); //Message to print when the code is wrong
      lcd.setCursor(6, 0);
      lcd.print("INCORRECT");
      lcd.setCursor(15, 1);
      lcd.print(" ");
      lcd.setCursor(4, 1);
      lcd.print("GET AWAY!!!");
    }
    
  }

  if (keypressed == '#') {                //To change the code it calls the changecode function
    ChangeCode();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ENTER THE CODE");
    lcd.setCursor(1 , 1);

    lcd.print("TO / (Door)!!");                 //When done it returns to standby mode
  }

  if (keypressed == 'D') {                //To change the code it calls the changecode function
    LockDoor();
    loop();
      
  }

delay(1000);




 
}
