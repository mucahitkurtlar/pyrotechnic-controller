//defining in/out pins
#define FIRE_PIN 8

//defining nRF24L01 pins
#define CE 9
#define CS 10

//including libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t code0 = 0xE8E8F0F0E1LL; //defining read-write channel codes
const uint64_t code1 = 0xE8E9F0F0E1LL; //defining read-write channel codes
int data[4]; //an array for rf communication
int ok; //ok message for rf communication
unsigned long plantTime; //the bomb plant time
bool fire = false;

RF24 radio(CE, CS); //creating radio object

void setup() {
  pinMode(FIRE_PIN, OUTPUT); //setting FIRE_PIN as output
  
  Serial.begin(9600); //starting serial communication
  Serial.println("        |\\                   /|\n        | \\                 / |\n        |  \\               /  |\n        |   \\_\\---'''---/_/   |\n        |     ___ \\ / ___     |\n        |    -_|_-   -_|_-    |\n         \\                   /\n        ~~ /~~    ___    ~~\\~~\n     ~~~~~~\\~~    \\ /    ~~/~~~~~~\n        ~~~~\\__  _/ \\_  __/~~~~\n               \"\"\\___/\"\"\n");
  Serial.println("\tRF RECEIVER FOR PYROTECHNIC");
  Serial.println("\t   Made by allesfresser0");
  Serial.println("----------------------------------------------------------");
  radio.begin(); //starting nRF24L01
  radio.openWritingPipe(code0); //setting writing channel
  radio.openReadingPipe(1, code1); //setting reading channel
  radio.startListening(); //starting listening
}

void loop() {
  if(radio.available()){
    Serial.println("Radio available");
    bool done0 = false;
    while(!done0){ //to the end...
      done0 = radio.read(data, sizeof(data)); //...read signal and write to data[] array
    }
    for(int i = 0; i < 4; i++){
      Serial.print(data[i]); //print the values
      Serial.print("\t");
    }
    Serial.println();
    if(data[0] == 42){ //if cover letter is 42 s
      Serial.println("Bomb has been planted");
      fire = true;
      radio.stopListening(); //stop listening
      ok = 42; //assign ok message
      delay(5);
      for(int i = 0; i < 3; i++){//send ok message for three time for better result
        radio.write(&ok, sizeof(ok)); //write ok message
      }
      plantTime = millis(); //save plant time
      radio.startListening(); //start listening
      unsigned long theTime = ((data[1] * 3600000) + (data[2] * 60000) + (data[3] * 1000)); //turn time to milisecond
      while(((millis() - plantTime) < theTime) && !((data[1] == 0)&&(data[2] == 0)&&(data[3] == 0))){
        if(radio.available()){
          Serial.println("Radio available");
          bool done1 = false;
          while(!done1){
            done1 = radio.read(data, sizeof(data));
          }
          if(data[0] == 255){
            defuse();
            break;
          }
          else if(data[0] == 55){
            sendTime(theTime - (millis() - plantTime));
          }
        }
      }
      if(fire){
        fireTime();
      }
    }
  }
}
void sendTime(unsigned long timeLeft){
  radio.stopListening();
  int data[4];
  data[1] = timeLeft / 3600000;
  data[2] = ((timeLeft - (3600000 * data[1])) / 60000);
  data[3] = ((timeLeft - (3600000 * data[1])) % 60000) / 1000;
  for(int i = 0; i < 3; i++){
    radio.write(data, sizeof(data));
  }
  for(int i = 0; i < 4; i++){
    Serial.print(data[i]);
    Serial.print("\t");
  }
  Serial.println();
  Serial.println("The left time has been sent");
  delay(5);
  radio.startListening();
}
void fireTime(){
  digitalWrite(8, HIGH);
  delay(2000);
  digitalWrite(8, LOW);
  Serial.println("Bomb exploded");
}
void defuse(){
  fire = false;
  Serial.println("Bomb has been defused");
  radio.stopListening();
  delay(5);
  ok = 255;
  for(int i = 0; i < 3; i++){
  radio.write(&ok, sizeof(ok));
  }
  radio.startListening();
}
