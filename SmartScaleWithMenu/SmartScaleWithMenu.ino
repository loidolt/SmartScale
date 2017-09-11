#include <LiquidCrystal.h> 
#include <SPI.h>
#include "HX711.h"

#define DOUT  3
#define CLK  2

HX711 scale(DOUT, CLK);

float calibration_factor = 500;

// initialize the interface pins for the LCD
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

//Define Various Menu Variables used throughout

int timer = 0;
byte totalRows = 2;                // total rows of LCD
byte totalCols = 16;               // total columns of LCD
int returndata = 0;                // Used for return of button presses
unsigned long timeoutTime = 0;     // this is set and compared to millis to see when the user last did something.
const int menuTimeout = 20000;     // time to timeout in a menu when user doesn't do anything.
unsigned long lastButtonPressed;   // this is when the last button was pressed. It's used to debounce.
const int debounceTime = 150;      // this is the debounce and hold delay. Otherwise, you will FLY through the menu by touching the button. 

const int buttonUp = 4;            // Set pin for UP Button
const int buttonDown = 5;          // Set pin for DOWN Button
const int buttonSelect = 6;             // Set pin for SLELECT Button
const int buttonTare = 7;             // Set pun for CANCEL Button (No currently used)

int buttonStateUp = 0;             // Initalise ButtonStates
int buttonStateDown = 0;
int buttonState;

int piezoPin = 8;

//Weight and item variables
int itm = 0;
String ingredient;
float currentWeight = 0;
float targetWeight = 0;

// constants for indicating whether cursor should be redrawn
#define MOVECURSOR 1 
// constants for indicating whether cursor should be redrawn
#define MOVELIST 2  


void setup(){

    // set up the LCD's number of columns and rows: 
    lcd.begin(totalCols, totalRows);      
    
    // initialize the serial communications port:
    Serial.begin(9600);
        
    pinMode(buttonUp, INPUT_PULLUP);
    pinMode(buttonDown, INPUT_PULLUP);
    pinMode(buttonSelect, INPUT_PULLUP);
    pinMode(buttonTare, INPUT_PULLUP);
    
    pinMode(piezoPin, OUTPUT);

      // Boot Up Message
    Serial.println("Smart Scale");
    lcd.print("Smart Scale");
    lcd.setCursor(0, 1);
    lcd.print("Starting Up");
    Serial.print("Starting Up");
    delay(300);
    for( int i = 0; i < 6; i++) {
        Serial.print(".");
        lcd.print(".");
        delay(300);
    } 
  
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }

    scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
    scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

    lcd.clear();
  
}  


void loop() {
  
  byte topItemDisplayed = 0;  // stores menu item displayed at top of LCD screen
  byte cursorPosition = 0;  // where cursor is on screen, from 0 --> totalRows. 

  // redraw = 0  - don't redraw
  // redraw = 1 - redraw cursor
  // redraw = 2 - redraw list
  byte redraw = MOVELIST;  // triggers whether menu is redrawn after cursor move.
  byte i=0; // temp variable for loops.
  byte totalMenuItems = 0;  //a while loop below will set this to the # of menu items.

// Create list of Menu Items
  char* menuItems[]={
    "Basic Scale", 
    "Lip Balm", 
    "Hand Salve",
    "CB Conditioner", 
    "L Conditioner", 
    "Beard Balm",
    "",
  };

// count how many items are in list.

  while (menuItems[totalMenuItems] != ""){
    totalMenuItems++;  
  }
  
  //subtract 1 so we know total items in array.
  totalMenuItems--;  
  

  lcd.clear();  // clear the screen so we can paint the menu.

  boolean stillSelecting = true;  // set because user is still selecting.

  timeoutTime = millis() + menuTimeout; // set initial timeout limit. 

  do   // Run a loop while waiting for user to select menu option.
  {

// Call any other setup actions required and/or process anything else required whilst in setup mode as opposed to things setup regardless of setup mode


    
// Call read buttons routine which analyzes buttons and gets a response. Default response is 0.  
switch(read_buttons())
    {  

   
      // Case responses depending on what is returned from read buttons routine
      
      case 1:  // 'UP' BUTTON PUSHED

      timeoutTime = millis()+menuTimeout;  // reset timeout timer
      //  if cursor is at top and menu is NOT at top
      //  move menu up one.
      if(cursorPosition == 0 && topItemDisplayed > 0)  //  Cursor is at top of LCD, and there are higher menu items still to be displayed.
      {
        topItemDisplayed--;  // move top menu item displayed up one. 
        redraw = MOVELIST;  // redraw the entire menu
      }

      // if cursor not at top, move it up one.
      if(cursorPosition>0)
      {
        cursorPosition--;  // move cursor up one.
        redraw = MOVECURSOR;  // redraw just cursor.
      }
      break;

   
   
   
      case 2:    // 'DOWN' BUTTON PUSHED

      timeoutTime = millis()+menuTimeout;  // reset timeout timer
      // this sees if there are menu items below the bottom of the LCD screen & sees if cursor is at bottom of LCD 
      if((topItemDisplayed + (totalRows-1)) < totalMenuItems && cursorPosition == (totalRows-1))
      {
        topItemDisplayed++;  // move menu down one
        redraw = MOVELIST;  // redraw entire menu
      }
      if(cursorPosition<(totalRows-1))  // cursor is not at bottom of LCD, so move it down one.
      {
        cursorPosition++;  // move cursor down one
        redraw = MOVECURSOR;  // redraw just cursor.
      }
      break;

      
      
      
      
      
      case 4:  // SELECT BUTTON PUSHED

      timeoutTime = millis()+menuTimeout;  // reset timeout timer
      switch(topItemDisplayed + cursorPosition) // adding these values together = where on menuItems cursor is.
      {
      case 0:  // menu item 1 selected          

        Serial.println("Basic Scale");
        lcd.clear();
        basicScale();
        stillSelecting = false;
        
       break;

      case 1:  // menu item 2 selected
        Serial.println("Lip Balm");
        lcd.clear();
        LipBalm();
        
        break;

      case 2:  // menu item 3 selected
        lcd.clear();
        lcd.print("Run Item3 code");
        lcd.setCursor(0,1);
        lcd.print("from here");Serial.print("Menu item ");
        Serial.print(topItemDisplayed + cursorPosition);
        Serial.print(" selected - ");
        Serial.println(menuItems[topItemDisplayed + cursorPosition]);
        break;

      case 3:  // menu item 4 selected
        lcd.clear();
        lcd.print("Run Item4 code");
        lcd.setCursor(0,1);
        lcd.print("from here");Serial.print("Menu item ");       Serial.print("Menu item ");
        Serial.print(topItemDisplayed + cursorPosition);
        Serial.print(" selected - ");
        Serial.println(menuItems[topItemDisplayed + cursorPosition]);
        break;

      case 4:  // menu item 5 selected
        lcd.clear();
        lcd.print("Run Item5 code");
        lcd.setCursor(0,1);
        lcd.print("from here");Serial.print("Menu item ");Serial.print("Menu item ");
        Serial.print(topItemDisplayed + cursorPosition);
        Serial.print(" selected - ");
        Serial.println(menuItems[topItemDisplayed + cursorPosition]);
        break;

      case 5:  // menu item 6 selected
        lcd.clear();
        lcd.print("Run Item6 code");
        lcd.setCursor(0,1);
        lcd.print("from here");Serial.print("Menu item ");        Serial.print("Menu item ");
        Serial.print(topItemDisplayed + cursorPosition);
        Serial.print(" selected - ");
        Serial.println(menuItems[topItemDisplayed + cursorPosition]);
        break;
       
      }
      break;
      
    case 8:  //  TARE BUTTON PUSHED
      Serial.println("Tare");
      scale.tare();
      break;


}

    switch(redraw){  //  checks if menu should be redrawn at all.
    case MOVECURSOR:  // Only the cursor needs to be moved.
      redraw = false;  // reset flag.
      if (cursorPosition > totalMenuItems) // keeps cursor from moving beyond menu items.
        cursorPosition = totalMenuItems;
      for(i = 0; i < (totalRows); i++){  // loop through all of the lines on the LCD
        lcd.setCursor(0,i);
        lcd.print(" ");                      // and erase the previously displayed cursor
        lcd.setCursor((totalCols-1), i);
        lcd.print(" ");
      }
      lcd.setCursor(0,cursorPosition);      // go to LCD line where new cursor should be & display it.
      lcd.print(">");
      lcd.setCursor((totalCols-1), cursorPosition);
      lcd.print("<");
      break;  // MOVECURSOR break.

    case MOVELIST:  // the entire menu needs to be redrawn
      redraw=MOVECURSOR;  // redraw cursor after clearing LCD and printing menu.
      lcd.clear(); // clear screen so it can be repainted.
      if(totalMenuItems>((totalRows-1))){  // if there are more menu items than LCD rows, then cycle through menu items.
        for (i = 0; i < (totalRows); i++){
          lcd.setCursor(1,i);
          lcd.print(menuItems[topItemDisplayed + i]);
        }
      }
      else{  // if menu has less items than LCD rows, display all available menu items.
        for (i = 0; i < totalMenuItems+1; i++){
          lcd.setCursor(1,i);
          lcd.print(menuItems[topItemDisplayed + i]);
        }
      }
   
      break;  // MOVELIST break
      
    }

    if (timeoutTime<millis()){  // user hasn't done anything in awhile
      stillSelecting = false;  // tell loop to bail out.
     
    }
  } 


  while (stillSelecting == true);  //
  
//End of Start Setup mode if

lcd.clear();

}

void basicScale() {

  int i = 0;
  while ( i != 4 ){
    i = read_buttons();
    
    weight();

    Serial.print("Ounces: ");
    lcd.setCursor(0, 0);
    lcd.print("Ounces:");
    
    Serial.println(currentWeight);
    lcd.setCursor(12, 0);
    lcd.print(currentWeight);

    Serial.print("Grams: ");
    lcd.setCursor(0, 1);
    lcd.print("Grams:");
    
    Serial.println((currentWeight * 28.3495));
    lcd.setCursor(10, 1);
    lcd.print((currentWeight * 28.3495));
  }
  
  lcd.clear();
  
}

void LipBalm (){

  itm = 0;
  int i = 0;
  
  while ( i != 4 ){
    i = read_buttons();
    
    switch(itm){
    case 0: {
    ingredient = "Beeswax";
    targetWeight = 0.5;
    runRecipe();
    tone(piezoPin, 1050, 200);
    ++itm;
    }
    case 1: {
    ingredient = "SheaButter";
    targetWeight = 0.5;
    runRecipe();
    tone(piezoPin, 1050, 200);
    ++itm;
    }
    case 2: {
    ingredient = "Coconut";
    targetWeight = 0.5;
    runRecipe();
    tone(piezoPin, 1050, 200);
    ++itm;
    }
    case 3: {
    ingredient = "VitaminE";
    targetWeight = 0.08;
    runRecipe();
    tone(piezoPin, 1050, 200);
    ++itm;
    }
    case 4: {
    ingredient = "Lanolin";
    targetWeight = 0.08;
    runRecipe();
    tone(piezoPin, 1050, 200);
    ++itm;
    }
    case 5: {
    ingredient = "Pepppermt";
    targetWeight = 0.03;
    runRecipe();
    tone(piezoPin, 1050, 200);
    ++itm;
    }
    case 6: {
    ingredient = "Lavender";
    targetWeight = 0.02;
    runRecipe();
    tone(piezoPin, 1050, 200);
    ++itm;
    }
    case 7: {
    lcd.setCursor(0,0);
    lcd.print("Done!");
    
    lcd.setCursor(0,1);
    lcd.print("Return to menu?");
    break;
    }
    }
  }
  itm = 0;
}

void runRecipe(){

  scale.tare();
  currentWeight = 0;
  weight();

  while ( currentWeight < targetWeight ){
    Serial.println("- Get Weight -");
    weight();

    Serial.print(ingredient);
    Serial.print(" ");
    lcd.setCursor(0, 0);
    lcd.print(ingredient);

    Serial.println(targetWeight);
    lcd.setCursor(12, 0);
    lcd.print(targetWeight);

    Serial.print("Weight oz: ");
    lcd.setCursor(0, 1);
    lcd.print("Weight oz:");

    Serial.println(currentWeight);
    lcd.setCursor(12, 1);
    lcd.print(currentWeight);
  }

    lcd.clear();

}

void weight(){

  currentWeight = currentWeight + 0.005;
  //currentWeight = scale.get_units(), 1;

}

int read_buttons(){  // you may need to swap "void" with "int" or "byte"
  
  int returndata = 0;
 
  if ((lastButtonPressed + debounceTime) < millis()){  // see if it's time to check the buttons again
    
    // read Up button
    buttonState = digitalRead(buttonUp);
   
    if (buttonState == LOW){
      returndata = returndata + 1;
      lastButtonPressed = millis();
      Serial.println("UP");
    }

    // read Down button
    buttonState = digitalRead(buttonDown);
    
    if (buttonState == LOW){
      returndata = returndata + 2;
      lastButtonPressed = millis();
      Serial.println("DOWN");
    }

    //read Select button
    buttonState = digitalRead(buttonSelect);
    
    if (buttonState == LOW){
      returndata = returndata + 4; 
      lastButtonPressed = millis();
      Serial.println("SELECT");
    }

    //read Tare button - Not used at present
    buttonState = digitalRead(buttonTare);
    
    if (buttonState == LOW){
      returndata = returndata + 8;
      lastButtonPressed = millis();
      Serial.println("TARE");
    }
  }
  
  return returndata; // this spits back to the function that calls it the variable returndata.
}

