#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ButtonHandler.h"

#define BUTT1 2 // plus button
#define BUTT2 3 // minus button
#define BUTT3 4 // menu button
#define BUTT4 5 // start button
#define RELAY 6 // relay pin
#define BUTT5 7 // select button

#define STATE_MENU 0 //main menu
#define STATE_STARTING 1 // starting RELAY action
#define STATE_RUNNING 2 // running RELAY
#define STATE_TIMER_REPEAT 3 //main menu

#define MENU_DELAY 0 
#define MENU_DELAY_WITH_STOP 1
#define MENU_MIN MENU_DELAY
#define MENU_MAX MENU_DELAY_WITH_STOP

#define TIME_STEP_MS 100 // time step value
#define STOP_DELAY_MS 1000 // stop delay

// submenu for MENU_DELAY_WITH_STOP menu
#define STOP_DELAY_DELAY_ITEM 0
#define STOP_DELAY_STOPS_ITEM 1
#define STOP_DELAY_STOP_DELAY_ITEM 2
#define STOP_DELAY_MIN STOP_DELAY_DELAY_ITEM
#define STOP_DELAY_MAX STOP_DELAY_STOP_DELAY_ITEM

// timers for relay
unsigned long delayTime = 1000; 
unsigned long prevMils = 0;
// load screen timers
unsigned long screenDelayTime = 195;
unsigned long screenPrevMils = 0;
//delay with stop menu variables
byte stops = 5; // amount of stops per cycle
unsigned long stopDelay = 1000; // stop delay time
unsigned long stopDelayPrev = 0;

// current states
int CURRENT_STATE = STATE_MENU;
int CURRENT_MENU = MENU_DELAY;
int CURRENT_STOP_DELAY_ITEM = STOP_DELAY_DELAY_ITEM;
int CURRENT_STOP = 0;

LiquidCrystal_I2C lcd(0x27,16,2);

void displayDelayScreen(){
    lcd.clear();  
    lcd.setCursor(0, 0);
    lcd.print("D:");
    lcd.print(delayTime / 1000);
    lcd.print(".");
    lcd.print((delayTime % 1000) / 100);
    lcd.setCursor(0, 1);
    lcd.print("Delay");
}

void displayDelayWithStopScreen(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("D:");
    lcd.print(delayTime / 1000);
    lcd.print(".");
    lcd.print((delayTime % 1000) / 100);

    lcd.setCursor(7, 0);
    lcd.print("S:");
    lcd.print(stops);
    lcd.print(" T:");
    lcd.print(stopDelay / 1000);

    lcd.setCursor(0, 1);
    lcd.print("Delay with stop");
}

void displayDelayRunningScreen(unsigned long now){
  Serial.print((now - prevMils) / 1000);
  Serial.print(".");
  Serial.print(((now - prevMils) % 1000) / 100);

  Serial.print(" / ");
  Serial.print(delayTime / 1000);
  Serial.print(".");
  Serial.println(delayTime % 1000);
  
  lcd.setCursor(0, 0);
  lcd.print((now - prevMils) / 1000);
  lcd.print(".");
  lcd.print(((now - prevMils) % 1000) / 100);

  lcd.print(" / ");
  lcd.print(delayTime / 1000);
  lcd.print(".");
  lcd.print(delayTime % 1000);
}

void displayDelayWithStopRunningScreen(unsigned long now){
  lcd.setCursor(0, 0);
  if (CURRENT_STATE == STATE_RUNNING) {
    lcd.print((now - prevMils) / 1000);
    lcd.print(".");
    lcd.print(((now - prevMils) % 1000) / 100);
  } else {
    lcd.print(0);
    lcd.print(".");
    lcd.print(0);
  }

  lcd.print(" / ");
  lcd.print(delayTime / 1000);
  lcd.print(".");
  lcd.print(delayTime % 1000);

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(CURRENT_STOP);
  lcd.print("/");
  lcd.print(stops);
  lcd.print(" T:");

  if (CURRENT_STATE == STATE_TIMER_REPEAT ) {
    lcd.print((now - stopDelayPrev) / 1000);  
    lcd.print(".");
    lcd.print(((now - stopDelayPrev) % 1000) / 100);
  } else {
    lcd.print(0);
    lcd.print(".");
    lcd.print(0);
  }
  lcd.print("/");
  lcd.print(stopDelay / 1000);
}

void buttonOneOn(){
  Serial.println("button 1 pressed");
  if (CURRENT_STATE == STATE_MENU) {

    if (CURRENT_MENU == MENU_DELAY) {
      delayTime += TIME_STEP_MS;
      
      displayDelayScreen();
      Serial.println(delayTime);
    } else 
    if (CURRENT_MENU == MENU_DELAY_WITH_STOP) {
      switch (CURRENT_STOP_DELAY_ITEM)
      {
      case STOP_DELAY_DELAY_ITEM:
        delayTime += TIME_STEP_MS;
        displayDelayWithStopScreen();
        break;
      case STOP_DELAY_STOPS_ITEM:
        if (stops > 9) {
          Serial.println("Max stops");
        } else {
          stops += 1;
        }
        displayDelayWithStopScreen();
        break;

      case STOP_DELAY_STOP_DELAY_ITEM:
        stopDelay += STOP_DELAY_MS;
        displayDelayWithStopScreen();
        break;
      default:
        break;
      }
    }
  }
}

void buttonTwoOn(){
  Serial.println("button 2 pressed");
  if (CURRENT_STATE == STATE_MENU) {
    if (CURRENT_MENU == MENU_DELAY) {
      if ((delayTime - TIME_STEP_MS) <= 0) {
        Serial.println("Cannot set negative time");
        return;
      }

      delayTime -= TIME_STEP_MS;

      displayDelayScreen();
      Serial.println(delayTime);
    } else 
    if (CURRENT_MENU == MENU_DELAY_WITH_STOP) {
      switch (CURRENT_STOP_DELAY_ITEM)
      {
      case STOP_DELAY_DELAY_ITEM:
        if ((delayTime - TIME_STEP_MS) <= 0) {
          Serial.println("Cannot set negative time");
          return;
        }
        delayTime -= TIME_STEP_MS;
        displayDelayWithStopScreen();
        break;
      case STOP_DELAY_STOPS_ITEM:
        if (stops <= 1) {
          Serial.println("Min stops");
          return;
        } 
        stops -= 1;
        displayDelayWithStopScreen();
        break;

      case STOP_DELAY_STOP_DELAY_ITEM:
        if ((stopDelay - STOP_DELAY_MS) <= 0) {
          Serial.println("Cannot set negative stop time");
          return;
        } 
        stopDelay -= STOP_DELAY_MS;
        displayDelayWithStopScreen();
        break;
      default:
        break;
      }
    }

  }
}

void initMenu(int menuIndex){
  lcd.clear();
  switch (menuIndex)
  {
  case 0:
    displayDelayScreen();
    break;
  case 1:
    displayDelayWithStopScreen();
    break;
  default:
    break;
  }
}

void nextMenu(){
  int nextMenu = CURRENT_MENU + 1;

  if(nextMenu > MENU_MAX){
    CURRENT_MENU = MENU_MIN;  
  } else {
    CURRENT_MENU++;
  }

  initMenu(CURRENT_MENU);
}

void nextSubmenu(){
  int nextSubmenu = CURRENT_STOP_DELAY_ITEM + 1;

  if (nextSubmenu > STOP_DELAY_MAX) {
    CURRENT_STOP_DELAY_ITEM = STOP_DELAY_MIN;
  } else {
    CURRENT_STOP_DELAY_ITEM++;
  }
}

void buttonThreeOn(){
  Serial.println("button 3 pressed");
  nextMenu(); 
}

void buttonFourOn(){
  Serial.println("button 4 pressed");
  Serial.println(delayTime);
  if (CURRENT_STATE == STATE_MENU) {
    CURRENT_STATE = STATE_STARTING;
  } 
  
  if (CURRENT_STATE == STATE_RUNNING) {
    // imidiate stop
    Serial.println("stop relay action");
    digitalWrite(RELAY, HIGH);
    CURRENT_STATE = STATE_MENU;
    initMenu(CURRENT_MENU);
  }
}

void buttonSelectOn(){
  if (CURRENT_MENU == MENU_DELAY_WITH_STOP) {
    nextSubmenu();
  }
}


void noActionHandler()
{ /** no action **/
}

ButtonHandler butth1(BUTT1, 20, &buttonOneOn, &noActionHandler);
ButtonHandler butth2(BUTT2, 20, &buttonTwoOn, &noActionHandler);
ButtonHandler butth3(BUTT3, 20, &buttonThreeOn, &noActionHandler);
ButtonHandler butth4(BUTT4, 20, &buttonFourOn, &noActionHandler);
ButtonHandler butth5(BUTT5, 20, &buttonSelectOn, &noActionHandler);

void relayActionUpdate(unsigned long now) {
  if (delayTime <= 0) {
    return;
  }

  if (CURRENT_STATE == STATE_STARTING && now - prevMils >= delayTime) {

    if (CURRENT_MENU == MENU_DELAY) {
      Serial.println("Starting relay action");
      CURRENT_STATE = STATE_RUNNING;  
      digitalWrite(RELAY, LOW);
      prevMils = now; 
      lcd.clear();
    } else if (CURRENT_MENU == MENU_DELAY_WITH_STOP) {
      if (CURRENT_STOP < stops) {
          CURRENT_STOP++;
          CURRENT_STATE = STATE_RUNNING; 
          digitalWrite(RELAY, LOW); 
          prevMils = now; 
          lcd.clear();
      }
    }
  }
  
  if (CURRENT_STATE == STATE_RUNNING && now - prevMils >= delayTime) {     
    // delay task
    if (CURRENT_MENU == MENU_DELAY) {
      CURRENT_STATE = STATE_MENU;
      digitalWrite(RELAY, HIGH);
      prevMils = now; 
      lcd.clear();
      initMenu(CURRENT_MENU);
    } else 
    if (CURRENT_MENU == MENU_DELAY_WITH_STOP) {
    // delay with stops task
      if (CURRENT_STOP < stops) {
        CURRENT_STATE = STATE_TIMER_REPEAT;
        digitalWrite(RELAY, HIGH);
        prevMils = now; 
        stopDelayPrev = millis();
      } else {
        CURRENT_STATE = STATE_MENU;
        CURRENT_STOP = 0;
        digitalWrite(RELAY, HIGH);
        prevMils = now; 
        lcd.clear();
        initMenu(CURRENT_MENU);
      }
    }
  }

  if (CURRENT_MENU == MENU_DELAY_WITH_STOP && CURRENT_STATE == STATE_TIMER_REPEAT && now - stopDelayPrev >= stopDelay) {
      // waiting for stopDelay timer completion
      stopDelayPrev = now;
      CURRENT_STATE = STATE_STARTING;
  }
  
  // handle running screen
  if (CURRENT_STATE == STATE_RUNNING && now - screenPrevMils > screenDelayTime) {
    Serial.println("Handle relay screen");
    if ( CURRENT_MENU == MENU_DELAY) {
      displayDelayRunningScreen(now);
    } else if (CURRENT_MENU == MENU_DELAY_WITH_STOP) {
      displayDelayWithStopRunningScreen(now);
    }

    screenPrevMils = now;
  }

  if (CURRENT_STATE == STATE_TIMER_REPEAT && CURRENT_MENU == MENU_DELAY_WITH_STOP && now - screenPrevMils > screenDelayTime) {
    displayDelayWithStopRunningScreen(now);
  }
}

void setup() {
  pinMode(BUTT1, INPUT);
  pinMode(BUTT2, INPUT);
  pinMode(BUTT3, INPUT);
  pinMode(BUTT4, INPUT);
  pinMode(BUTT5, INPUT);
  pinMode(RELAY, OUTPUT);

  digitalWrite(RELAY, HIGH);

  Serial.begin(115200); 

  lcd.init();
  lcd.backlight();
  lcd.print("Time relay");
  lcd.setCursor(0, 1);
  lcd.print("LCD 1602");

  delay(1000);
  lcd.clear();
  displayDelayScreen();
}

void loop() {
  unsigned long currentMillis = millis();
  
  butth1.Update(currentMillis);
  butth2.Update(currentMillis);
  butth3.Update(currentMillis);
  butth4.Update(currentMillis);
  butth5.Update(currentMillis);
  relayActionUpdate(currentMillis);
}