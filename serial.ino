/* String tone = "-... ..- -. .- ";
    

  Morse code - Receiver

  modified on 14 Apr 2019
  by Saeed Hosseini @ Electropeak
  https://electropeak.com/learn

*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN(pini nrf01)
#include <LiquidCrystal.h>
struct record
{
    String mesaj;
    boolean state=1;
};

typedef struct record Record;
// Create an LCD object. Parameters: (RS, E, D4, D5, D6, D7):
//LiquidCrystal lcd = LiquidCrystal(2, 3, 4, 5, 6, 7);
//boolean button_state = 1;
//boolean button_state1 = 1;
const byte addresses [][6] = {"00001", "00002"};  //Setting the two addresses. One for transmitting and one for receiving
String code = "";
int len = 0;
char ch;
int ok;
char new_char;
int i1=0;
int i2=1;
const int but_send=8;
const int but = 5;
const int led = 4;
const int del = 3;
String a="";
unsigned long pres_len = 0, rel_time, pres_time = 0, old_time_len = 0, old_pres = 0, space = 0;
int state = 0;
int unit_delay = 250;
int min_delay = 10;
Record  button_state,button_state1 ;



char MakeString()
{
  if (pres_len < (unit_delay*3) && pres_len > 50)
  {
    return '.';                        //if button press less than 0.6sec, it is a dot
  }
  else if (pres_len > (unit_delay*3))
  {
    return '-';                        //if button press more than 0.6sec, it is a dash
  }
}

void Morse_decod()
{
  static String morse[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....",
                             "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
                             ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "!"
                            };
  int i = 0;
  while (morse[i] != "!")  
  {
    if (morse[i] == code)
    {
      Serial.print(char('A' + i));
      a=a+char('A' + i);
      Serial.print(" ");
      if(i2>=2)
      {
        i1++;
        i2=0;
      }
      else 
      {i2++;
      }
      break;
    }
    i++;
  }
  if (morse[i] == "!")
  {
    Serial.println("");
    Serial.println("This code is not exist!");
  }

  code = "";
}
void sendText()
{
  // Aici trebuie sa fe codul pentru trimiterea efectiva a msj
  if(ok==1){
    Serial.println("A fost transmis de la arduino Iulia:"+ a);
    }
  ok=0;

 delay(200);
}
void setup() {
  Serial.begin(9600);
  pinMode(but, INPUT_PULLUP);
  pinMode(but_send, INPUT_PULLUP);
  pinMode(del, INPUT_PULLUP);

  pinMode(led, OUTPUT);
//  lcd.setCursor(i1, 12);
//  lcd.begin(16, 2);
  radio.begin();                           //Starting the radio communication
  radio.openWritingPipe(addresses[1]);     //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[0]);  //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MIN); //You can set it as minimum or maximum depending on the distance between the transmitter and receiver. 

}
void loop() {  
label:  
  while (digitalRead(but) == HIGH) { 
     
     if (digitalRead(but_send) == LOW)
      {
        ok=1;
        sendText();
      }
      
    delay(5);
    radio.stopListening();                             //This sets the module as transmitter   
    button_state.state = digitalRead(but_send);
    button_state.mesaj=a;
    
    radio.write(&button_state, sizeof(button_state));  //Sending the data
    delay(5);
        
    radio.startListening();                            //This sets the module as receiver
    while(radio.available())
    {                         //Looking for incoming data
    radio.read(&button_state1, sizeof(button_state1)); //Reading the data
    
   
      
    if (button_state1.state == 0)
      {
          Serial.println("Lavinia a trimis mesaj: "+button_state1.mesaj);
//          Serial.println(button_state1);
          //ok=1;
          //sendText();
      }
  
     }
    
    if (digitalRead(del) == LOW && a[0] != '\0' ) {
    Serial.println("Deleted string");
    a = "\0";
    Serial.println(a);
  }
 

  
 }
  old_pres = rel_time;
  pres_time = millis();
  digitalWrite(led, HIGH);
  while (digitalRead(but) == LOW) {}
  rel_time = millis();
  digitalWrite(led, LOW);
  pres_len = rel_time - pres_time;
  space = pres_time - old_pres;
  if (pres_len > min_delay)
  {
    code += MakeString();
  }
  while ((millis() - rel_time) < (unit_delay * 3))
  {
    
    if (digitalRead(but) == LOW)
    {
      goto label;
      
    }
    
  }

  Morse_decod();
  delay(1000);
 

  
}
