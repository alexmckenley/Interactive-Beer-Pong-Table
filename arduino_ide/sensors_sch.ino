#include <Shifter.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"

#define SER_Pin 8 //SER_IN
#define RCLK_Pin 9 //L_CLOCK
#define SRCLK_Pin 10 //CLOCK

#define NUM_REGISTERS 5 //how many registers are in the chain


//initaize shifter using the Shifter library
Shifter shifter(SER_Pin, RCLK_Pin, SRCLK_Pin, NUM_REGISTERS); 
//initalize MCP23018
Adafruit_MCP23017 mcp;

const int OE_Pin = 11;
const int eyeR = 5;
const int eyeG = 6;
const int eyeB = 7;
const char colors[6] = {'R', 'G', 'B', 'Y', 'P','W'};
const int rotate[6] = {0, 1, 3, 4, 5, 2};

int side1[6] = {1,1,1,1,1,1};
int side2[6] = {1,1,1,1,1,1};
int x = 0;
int y = 0;
int z = 0;
int allHigh1 = 0;
int allLow1 = 0;
int allHigh2 = 0;
int allLow2 = 0;
int side1Done = 0;
int side2Done = 0;
char side1Color = 'R';
char side2Color = 'W';
char side1Rand = 'W';
char side2Rand = 'R';
int chooseColor1 = 0;
int chooseColor2 = 0;
int flicker = 100;
int flickerCount = 0;
int redWing[3][6] = {{1,0,0,0,0,0},{1,1,1,0,0,0},{1,1,1,1,1,1}}; 
int knightRider[3][6] = {{0,0,0,1,1,1},{0,1,1,0,0,0},{1,0,0,0,0,0}};

void setup(){
  //Init MCP23018
  mcp.begin();      // use default address 0
  mcp.pinMode(0, INPUT);
  mcp.pinMode(1, INPUT);
  mcp.pinMode(2, INPUT);
  mcp.pinMode(3, INPUT);
  mcp.pinMode(4, INPUT);
  mcp.pinMode(5, INPUT);
  mcp.pinMode(6, INPUT);
  mcp.pinMode(7, INPUT);
  mcp.pinMode(8, INPUT);
  mcp.pinMode(9, INPUT);
  mcp.pinMode(10, INPUT);
  mcp.pinMode(11, INPUT);
  
  //Init IE Pin and set to low to enable shift registers
  pinMode(OE_Pin, OUTPUT);
  pinMode(eyeR, OUTPUT);
  pinMode(eyeG, OUTPUT);
  pinMode(eyeB, OUTPUT);
  digitalWrite(OE_Pin, LOW);
  digitalWrite(eyeR, LOW);
  digitalWrite(eyeG, HIGH);
  digitalWrite(eyeB, HIGH);
  
  pinMode(13, OUTPUT);
  
  //Make the random numbers different on each execution of the program
  //randomSeed(analogRead(0));
  
  //Spin the colors to get everybody in the mood.
  
  startup();
  colorSpin();

}

void loop(){
  
  
  side1Done = 0;
  side2Done = 0;
  side1[0] = mcp.digitalRead(0);
  side1[1] = mcp.digitalRead(1);
  side1[2] = mcp.digitalRead(2);
  side1[3] = mcp.digitalRead(3);
  side1[4] = mcp.digitalRead(4);
  side1[5] = mcp.digitalRead(5);
  side2[0] = mcp.digitalRead(6);
  side2[1] = mcp.digitalRead(7);
  side2[2] = mcp.digitalRead(8);
  side2[3] = mcp.digitalRead(9);
  side2[4] = mcp.digitalRead(10);
  side2[5] = mcp.digitalRead(11);
  
//  if (flickerCount == flicker){
//    digitalWrite(eyeR, !digitalRead(eyeR));
//    flicker = random(1000, 500000);
//    flickerCount = 0;
//  }
//  else
//    flickerCount++;

  
  //Check if all cups are placed on either side
  for (x = 0; x < 6; x++){
    if (side1[x] == 0)
      allLow1 = 1;
    else{
      allLow1 = 0;
      break;
    }
  }
  for (x = 0; x < 6; x++){
    if (side2[x] == 0)
      allLow2 = 1;
    else{
      allLow2 = 0;
      break;
    }
  }
  
  //Run code for a full set of cups here
  
  //Check if all cups are missing
  if( side1[0] && side1[1] && side1[2] && side1[3] && side1[4] && side1[5] )
    allHigh1 = 1;
  else
    allHigh1 = 0;
    
  if( side2[0] && side2[1] && side2[2] && side2[3] && side2[4] && side2[5] )
    allHigh2 = 1;
  else
    allHigh2 = 0;
    
    
  //Go to colorSelect, it will run the code if allHigh is true on either side.
  colorSelect();
  
  
  //Send update function, if side1Done or side2Done is 0, update the side, else do nothing
  update();
}






//This function is ran if there is at least one cup on a side, and its not full(to be added functionally later).
void update(){
  if (side1Done == 0){
   for (x = 0; x < 6; x++){
     if (side1[x] == 0){
       if(chooseColor1){
         side1Color = colors[x];
         side1Rand = colors[random(6)];
         if(side1Rand == side1Color)
           side1Rand = colors[x+1];
         chooseColor1 = 0;
       }
       setCupWait( x, side1Color);
     }
     else
       setCupWait(x, side1Rand);
   }
   shifter.write();
  }
  if (side2Done == 0){
   for (x = 0; x < 6; x++){
     if (side2[x] == 0){
       if(chooseColor2){
         side2Color = colors[x];
         side2Rand = colors[random(6)];
         if(side2Rand == side2Color)
           side2Rand = colors[x+1];
         chooseColor2 = 0;
       }
       setCupWait( x+6, side2Color);
     }  
     else
       setCupWait(x+6, side2Rand);
   }    
   shifter.write();
  }
}

//This function changes each cup to a different color when there are no cups placed.
void colorSelect(){
  if(allHigh1){
    for(x = 0; x < 6; x++){
      setCup(x, colors[x]); 
    }
    side1Done = 1;
    chooseColor1 = 1;
  } 
  if(allHigh2){
    for(x = 0; x < 6; x++){
      setCup(x+6, colors[x]);
    } 
    side2Done = 1;
    chooseColor2 = 1;
  }
}

//This function does a spinning light show when the table is first turned on
void colorSpin(){
  for (x = 0; x < 6; x++){
    for (y = 0; y < 6; y++){
      setCup(rotate[y], colors[x]);
      setCup(rotate[y]+6, colors[x]);
      delay(50);
    } 
  }
}

void startup(){
  //Start program bright white for a few seconds
//  shifter.setAll(HIGH);
//  shifter.write();
  
  //This code transitions from red to white 10 times(faster each time)
  for(z=0; z<10; z++){
    for(x = 0; x < 3; x++){
      for (y=0; y<6; y++){
        if(redWing[x][y]){
          setCup(y, 'R');
          setCup(y+6, 'R');
        }
      }
//      delay(100 - (z*5));
        delay(60);
    }
      for(x = 0; x < 3; x++){
      for (y=0; y<6; y++){
        if(redWing[x][y]){
          setCup(y, 'W');
          setCup(y+6, 'W');
        }
      }
//      delay(100 - (z*5));
        delay(60);
    }
  }
  
  //Flash red and white 
//  for (x=0; x <5; x++){
//    for(y=0; y<6; y++){
//      setCupWait(y, 'R');
//      setCupWait(y+6, 'R');
//    }
//    shifter.write();
//    delay(50);
//    shifter.setAll(HIGH);
//    shifter.write();
//    delay(50);   
//  }
  
  knightRide(5);
  
}

void knightRide(int rounds){
    //Knight Rider Pattern
  for(x=0;x< rounds ;x++){
    shifter.setAll(HIGH);
    //First Three cups
    for(y=0; y<3; y++){
      for(z=0; z<6; z++){
        if(knightRider[y][z]){
          setCupWait(z, 'R');
        }
        else
          setCupWait(z,'W');
      }
      shifter.write();
      delay(70);
    }
    
    //Next three cups
    shifter.setAll(HIGH);
    for(y=2; y>=0; y--){
      for(z=0; z<6; z++){
        if(knightRider[y][z]){
          setCupWait(z+6, 'R');
        }
        else
          setCupWait(z+6, 'W'); 
      }
      shifter.write();
      delay(70);
    }
    
    //Begin Reverse Order
    shifter.setAll(HIGH);
    for(y=0; y<3; y++){
      for(z=0; z<6; z++){
        if(knightRider[y][z]){
          setCupWait(z+6, 'R');
        }
        else
          setCupWait(z+6,'W');
      }
      shifter.write();
      delay(70);
    }
    
    shifter.setAll(HIGH);
    for(y=2; y>=0; y--){
      for(z=0; z<6; z++){
        if(knightRider[y][z]){
          setCupWait(z, 'R');
        }
        else
          setCupWait(z, 'W'); 
      }
      shifter.write();
      delay(70);
    }
    
  }
}

/* This function sets the cup to the desired color */
void setCup(int cup, char color){
  cup = cup*3;
  
  switch(color){
    case 'R':
      shifter.setPin(39-cup, HIGH);
      shifter.setPin(39-(cup+1), LOW);
      shifter.setPin(39-(cup+2), LOW);
      break;
    case 'G':
      shifter.setPin(39-cup, LOW);
      shifter.setPin(39-(cup+1), HIGH);
      shifter.setPin(39-(cup+2), LOW);
      break;
    case 'B':
      shifter.setPin(39-cup, LOW);
      shifter.setPin(39-(cup+1), LOW);
      shifter.setPin(39-(cup+2), HIGH);
      break;
    case 'Y':
      shifter.setPin(39-cup, HIGH);
      shifter.setPin(39-(cup+1), HIGH);
      shifter.setPin(39-(cup+2), LOW);
      break;
    case 'P':
      shifter.setPin(39-cup, HIGH);
      shifter.setPin(39-(cup+1), LOW);
      shifter.setPin(39-(cup+2), HIGH);
      break;
    case 'W':
      shifter.setPin(39-cup, HIGH);
      shifter.setPin(39-(cup+1), HIGH);
      shifter.setPin(39-(cup+2), HIGH);
      break;
    case 'C':
      shifter.setPin(39-cup, LOW);
      shifter.setPin(39-(cup+1), HIGH);
      shifter.setPin(39-(cup+2), HIGH);
      break;
    case 'X':
      shifter.setPin(39-cup, LOW);
      shifter.setPin(39-(cup+1), LOW);
      shifter.setPin(39-(cup+2), LOW);
      break;
  }

  shifter.write(); 
}

/* This function sets the cup to the desired color without writing changes to the shift registers
   later to be written with shifter.write(); */
void setCupWait(int cup, char color){
  cup = cup*3;
  
  switch(color){
    case 'R':
      shifter.setPin(39-cup, HIGH);
      shifter.setPin(39-(cup+1), LOW);
      shifter.setPin(39-(cup+2), LOW);
      break;
    case 'G':
      shifter.setPin(39-cup, LOW);
      shifter.setPin(39-(cup+1), HIGH);
      shifter.setPin(39-(cup+2), LOW);
      break;
    case 'B':
      shifter.setPin(39-cup, LOW);
      shifter.setPin(39-(cup+1), LOW);
      shifter.setPin(39-(cup+2), HIGH);
      break;
    case 'Y':
      shifter.setPin(39-cup, HIGH);
      shifter.setPin(39-(cup+1), HIGH);
      shifter.setPin(39-(cup+2), LOW);
      break;
    case 'P':
      shifter.setPin(39-cup, HIGH);
      shifter.setPin(39-(cup+1), LOW);
      shifter.setPin(39-(cup+2), HIGH);
      break;
    case 'W':
      shifter.setPin(39-cup, HIGH);
      shifter.setPin(39-(cup+1), HIGH);
      shifter.setPin(39-(cup+2), HIGH);
      break;
    case 'C':
      shifter.setPin(39-cup, LOW);
      shifter.setPin(39-(cup+1), HIGH);
      shifter.setPin(39-(cup+2), HIGH);
      break;
    case 'X':
      shifter.setPin(39-cup, LOW);
      shifter.setPin(39-(cup+1), LOW);
      shifter.setPin(39-(cup+2), LOW);
      break;
  }
}

