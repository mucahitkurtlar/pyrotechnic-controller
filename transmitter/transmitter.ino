//defining menu settings
#define MAIN_MENU_STEP 128
#define TIME_MENU_STEP 200

//defining buzzer, pot and buttons
//#define DEBUG_PIN 8
#define UP_PIN A1
#define DOWN_PIN A2
#define SELECT_PIN A3
#define BUZZER_PIN A4
#define POT_PIN A0
#define FIRE_PIN A5

//defining lcd pins
#define RS 2
#define EN 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7

//defining nRF24L01 pins
#define CE 9
#define CS 10

//including libraries
#include <LiquidCrystal.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t code0 = 0xE8E8F0F0E1LL;
const uint64_t code1 = 0xE8E9F0F0E1LL;
int data[4];
unsigned int bombHour = 0, bombMinute = 5, bombSecond = 0;
int counter;
boolean stay;

RF24 radio(CE ,CS);
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);


void setup() {
  //pinMode(DEBUG_PIN, INPUT);
  pinMode(UP_PIN, INPUT);
  pinMode(DOWN_PIN, INPUT);
  pinMode(SELECT_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FIRE_PIN, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  radio.begin();
  radio.openWritingPipe(code1);
  radio.openReadingPipe(1, code0);
}

void loop() {
  if((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < MAIN_MENU_STEP){
    buzz();
    lcd.clear();
    lcd.print("1-Vehicle Mode");
    lcd.setCursor(0, 1);
    lcd.print("(Out of service)");
    while((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < MAIN_MENU_STEP){
    }
    delay(50);
  }
  else if(((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= MAIN_MENU_STEP) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 2))){
    stay = true;
    buzz();
    lcd.clear();
    lcd.print("2-Defuse Mode");
    while((((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= MAIN_MENU_STEP) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 2))) && stay){
      if(digitalRead(SELECT_PIN) == HIGH){
        buzz();
        lcd.clear();
        lcd.print("Press fire button");
        lcd.setCursor(8, 1);
        lcd.print("to defuse the bomb");
        while(digitalRead(SELECT_PIN) == HIGH){}
        while((digitalRead(SELECT_PIN) == LOW) && (((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= MAIN_MENU_STEP ) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 2)))){
          if(digitalRead(FIRE_PIN) == HIGH){
            defuse();
            break;
          }
          if((millis() % 400) == 0){
            lcd.scrollDisplayLeft();
          }
        }
        while(digitalRead(SELECT_PIN) == HIGH){}
        stay = false;
      }
    }
    delay(50);
  }
  else if(((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= (MAIN_MENU_STEP * 2)) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 3))){
    stay = true;
    buzz();
    lcd.clear();
    lcd.print("3-Time Bomb Mode");
    while((((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= (MAIN_MENU_STEP * 2)) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 3))) && stay){
      if(digitalRead(SELECT_PIN) == HIGH){
        buzz();
        lcd.clear();
        lcd.print("    Set Time");
        delay(680);
        while(digitalRead(SELECT_PIN) == HIGH){}
        while((digitalRead(SELECT_PIN) == LOW) && stay){
          if((analogRead(POT_PIN) % (TIME_MENU_STEP * 3)) < TIME_MENU_STEP){
            setTime("Set hour", bombHour, 0, 1);
          }
          else if(((analogRead(POT_PIN) % (TIME_MENU_STEP * 3)) >= TIME_MENU_STEP) && ((analogRead(POT_PIN) % (TIME_MENU_STEP * 3)) < (TIME_MENU_STEP * 2))){
            setTime("Set minute", bombMinute, 1, 2);
          }
          else if((analogRead(POT_PIN) % (TIME_MENU_STEP * 3)) >= (TIME_MENU_STEP * 2)){
            setTime("Set second", bombSecond, 2, 3);
          }
        }
        while(digitalRead(SELECT_PIN) == HIGH){}
        stay = false;
      }
    }
    delay(50);
  }
  else if(((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= (MAIN_MENU_STEP * 3)) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 4))){
    stay = true;
    buzz();
    lcd.clear();
    lcd.print("4-QuickFire Mode");
    while((((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= (MAIN_MENU_STEP * 3)) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 4))) && stay){
      if(digitalRead(SELECT_PIN) == HIGH){
        buzz();
        lcd.clear();
        lcd.print("Press fire button");
        lcd.setCursor(8, 1);
        lcd.print("to fire the bomb");
        while(digitalRead(SELECT_PIN) == HIGH){}
        while((digitalRead(SELECT_PIN) == LOW) && (((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= (MAIN_MENU_STEP * 3)) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 4)))){
          if(digitalRead(FIRE_PIN) == HIGH){
            fire(0, 0, 0);
            break;
          }
          if((millis() % 400) == 0){
            lcd.scrollDisplayLeft();
          }
        }
        while(digitalRead(SELECT_PIN) == HIGH){}
        stay = false;
      }
    }
    delay(50);
  }
  else if(((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= (MAIN_MENU_STEP * 4)) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 5))){
    stay = true;
    buzz();
    lcd.clear();
    lcd.print("5-Time Left Mode");
    while((((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= (MAIN_MENU_STEP * 4)) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 5))) && stay){
      if(digitalRead(SELECT_PIN) == HIGH){
        buzz();
        lcd.clear();
        lcd.print("Press fire button");
        lcd.setCursor(8, 1);
        lcd.print("to see the time");
        while(digitalRead(SELECT_PIN) == HIGH){}
        while((digitalRead(SELECT_PIN) == LOW) && (((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= (MAIN_MENU_STEP * 4)) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 5)))){
          if(digitalRead(FIRE_PIN) == HIGH){
            seeTimeLeft();
            break;
          }
          if((millis() % 400) == 0){
            lcd.scrollDisplayLeft();
          }
        }
        while(digitalRead(SELECT_PIN) == HIGH){}
        stay = false;
      }
    }
    delay(50);
  }
}
void printTime(){
  lcd.setCursor(0, 1);
  lcd.print(bombHour);
  lcd.setCursor(2, 1);
  lcd.print("hr");
  lcd.setCursor(5, 1);
  lcd.print(bombMinute);
  lcd.setCursor(7, 1);
  lcd.print("mn");
  lcd.setCursor(10, 1);
  lcd.print(bombSecond);
  lcd.setCursor(12, 1);
  lcd.print("sc");
}
void timeBombUp(unsigned int &bombTime){
  buzz();
  if(bombTime < 59){
    bombTime++;
  }
  lcd.clear();
  lcd.print("Time: ");
  printTime();
  int counter = 0;
  while(digitalRead(UP_PIN) == HIGH){
    while(counter > 4){
      buzz();
      if(bombTime <= 54){
        bombTime += 5;
      }
      else if((bombTime > 54) && (bombTime < 59)){
        bombTime = 59;
      }
      printTime();
      if(digitalRead(UP_PIN) == LOW){
        counter = 0;
      }
      delay(70);
    }
    delay(190);
    counter++;
  }
}
void timeBombDown(unsigned int &bombTime){
  buzz();
  if(bombTime > 0){
    bombTime--;
  }
  lcd.clear();
  lcd.print("Time: ");
  printTime();
  int counter = 0;
  while(digitalRead(DOWN_PIN) == HIGH){
    while(counter > 4){
      buzz();
      if(bombTime >= 5){
        bombTime -= 5;
      }
      else if((bombTime < 5) && (bombTime > 0)){
        bombTime = 0;
      }
      printTime();
      if(digitalRead(DOWN_PIN) == LOW){
        counter = 0;
      }
      delay(70);
    }
    delay(190);
    counter++;
  }
}
void fire(unsigned int hour, unsigned int minute, unsigned int second){
  buzz();
  buzz();
  data[0] = 42;
  data[1] = hour;
  data[2] = minute;
  data[3] = second;
  for(int i = 0; i < 3; i++){
    radio.write(data, sizeof(data));
  }
  delay(5);
  radio.startListening();
  unsigned long listenStartTime = millis();
  while(true){
    if(radio.available()){
      int ok;
      radio.read(&ok, sizeof(ok));
      Serial.println(ok);
      if(ok == 42){
        buzz();
        lcd.clear();
        lcd.print("Bomb has been");
        lcd.setCursor(0, 1);
        lcd.print("planted");
        buzzTill(350);
        radio.stopListening();
        delay(650);
        break;
      }
    }
    else if((millis() - listenStartTime) >= 2000){
      lcd.clear();
      lcd.print("Timed out");
      delay(1000);
      break;
    }
  }
}
void defuse(){
  for(int i = 0; i < 4; i++){
    buzz();
    data[i] = 255;
  }
  for(int i = 0; i < 3; i++){
    radio.write(data, sizeof(data));
  }
  delay(5);
  radio.startListening();
  unsigned long listenStartTime = millis();
  while(true){
    if(radio.available()){
      int ok;
      radio.read(&ok, sizeof(ok));
      Serial.println(ok);
      if(ok == 255){
        buzz();
        lcd.clear();
        lcd.print("Bomb has been");
        lcd.setCursor(0, 1);
        lcd.print("defused");
        buzzTill(350);
        radio.stopListening();
        delay(650);
        break;
      }
    }
    else if((millis() - listenStartTime) >= 2000){
      lcd.clear();
      lcd.print("Timed out");
      delay(1000);
      break;
    }
  }
}
void setTime(String setTimeStr, unsigned int &setTimeInt, int minPotVal, int maxPotVal){
  buzz();
  lcd.clear();
  lcd.print(setTimeStr);
  printTime();
  while(((analogRead(POT_PIN) % (TIME_MENU_STEP * 3)) >= (TIME_MENU_STEP * minPotVal)) && ((analogRead(POT_PIN) % (TIME_MENU_STEP * 3)) < (TIME_MENU_STEP * maxPotVal))){
    if(digitalRead(UP_PIN) == HIGH){
      timeBombUp(setTimeInt);
    }
    else if(digitalRead(DOWN_PIN) == HIGH){
      timeBombDown(setTimeInt);
    }
    else if(digitalRead(FIRE_PIN) == HIGH){
      fire(bombHour, bombMinute, bombSecond);
      while(digitalRead(FIRE_PIN) == HIGH){}
    }
    else if(digitalRead(SELECT_PIN) == HIGH){
      while(digitalRead(SELECT_PIN) == HIGH){}
      stay = false;
      break;
    }
  }
}
void seeTimeLeft(){
  int data[4];
  data[0] = 55;
  for(int i = 0; i < 3; i++){
    radio.write(data, sizeof(data));
  }
  delay(5);
  radio.startListening();
  unsigned long listenStartTime = millis();
  while(true){
    if(radio.available()){
      bool done = false;
      while(!done){
        done = radio.read(data, sizeof(data));
      }
      delay(5);
      radio.stopListening();
      //Serial.println(data[2]);
      showTimeLeft(data[1], data[2], data[3]);
      break;
    }
    else if((millis() - listenStartTime) >= 2000){
      lcd.clear();
      lcd.print("Timed out");
      delay(1000);
      break;
    }
  }
}
void showTimeLeft(int hour, int minute, int second){
  Serial.println("////////////");
  Serial.println(hour);
  Serial.println(minute);
  Serial.println(second);
  Serial.println("////////////");
  bool exploded = true;
  unsigned long startTime = millis();
  while((hour > 0) || (minute > 0) || (second > 0)){
    if(((millis() - startTime) % 1000) == 0){
      lcd.clear();
      lcd.print("Time left:");
      lcd.setCursor(0, 1);
      lcd.print(hour);
      lcd.setCursor(2, 1);
      lcd.print("hr");
      lcd.setCursor(5, 1);
      lcd.print(minute);
      lcd.setCursor(7, 1);
      lcd.print("mn");
      lcd.setCursor(10, 1);
      lcd.print(second);
      lcd.setCursor(12, 1);
      lcd.print("sc");
      buzz();
      if(second > 0){
        second--;
      }
      else if(second == 0){
        second = 59;
        if(minute > 0){
          minute--;
        }
        else if(minute == 0){
          minute = 59;
          hour--;
        }
      }
    }
    if((digitalRead(SELECT_PIN) == HIGH) || !(((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) >= (MAIN_MENU_STEP * 4)) && ((analogRead(POT_PIN) % (MAIN_MENU_STEP * 5)) < (MAIN_MENU_STEP * 5)))){
      exploded = false;
      break;
    }
  }
  if(exploded){
    lcd.clear();
    lcd.print("Bomb exploded");
    buzzTill(350);
    delay(650);
  }
}
void buzz(){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(8);
  digitalWrite(BUZZER_PIN, LOW);
}
void buzzTill(unsigned int till){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(till);
  digitalWrite(BUZZER_PIN, LOW);
}
