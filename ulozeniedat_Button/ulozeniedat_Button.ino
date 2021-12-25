#include <IRremote.h>
int input_pin = 2; //signalovy pin z IR prijimaca
IRrecv irrecv(input_pin);
decode_results signals;

//premenne
unsigned long int Vol_plus, Vol_minus;  //do tychto uklada hodnoty signalu pre + a -
boolean vol1,vol2 = false; //pomocne premenne, vo while cykle ci uz ma oba signaly alebo nie
const int  buttonPin = A2;    // the pin that the pushbutton is attached to
const int ledPin = 8;       // the pin that the LED is attached to
int buttonPushCounter = -1;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

void setup() {
   pinMode(6,OUTPUT);
  Serial.begin(9600);
  irrecv.enableIRIn(); 
  // initialize the button pin as a input:
  pinMode(buttonPin, INPUT);
  // initialize the LED as an output:
  pinMode(ledPin, OUTPUT);
  // initialize serial communication:
  Serial.begin(9600);
}


void loop() {
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.println("on");
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }


  
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
      Serial.println(" "); 
      Serial.println("ZACIATOK"); 
      Serial.println("================="); 
      Serial.println(buttonPushCounter);
      Serial.println("================="); 
      Serial.println("vol plus");     //kontrola
      Serial.println(Vol_plus);
      Serial.println(Vol_plus, HEX);
      Serial.println("================="); 
      Serial.println("vol minus");    //kontrola
      Serial.println(Vol_minus);
      Serial.println(Vol_minus, HEX);

if(buttonPushCounter == 1){
       Vol_plus=0;
       Vol_minus=0;
       vol1=false;
       vol2=false;
      irrecv.resume(); 
  }

  
if(buttonPushCounter == 1 || buttonPushCounter == 2 || buttonPushCounter == 3){
  digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(50);
  }

  
if (buttonPushCounter == 2 ) {
   Serial.println("stlacte + ");
   calibration_plu();
}

if (buttonPushCounter == 3 ) {
   Serial.println("stlacte - ");
   calibration_min();
}

  // turns on the LED every four button pushes by checking the modulo of the
  // button push counter. the modulo function gives you the remainder of the
  // division of two numbers:
  if (buttonPushCounter == 4) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
     delay(100);
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
     delay(100);
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
     delay(100);
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
  buttonPushCounter=-1;
  }

}


void calibration_plu(){
  while( !(vol1 == true) ){            //program nepojde dalej dokial nedostane 2 signaly pre + a -
    
    if ( (irrecv.decode(&signals))) { // tato podmienka sa vykona prva --> ulozi signal +
      Vol_plus = signals.value;
       signals.value=0;
      Serial.println("vol plus");     //kontrola
      Serial.println(Vol_plus);
      Serial.println(Vol_plus, HEX);
      irrecv.resume();              // priprava na prijatie 2. signalu?
      vol1 = true;             //mam signal1
      digitalWrite(2,LOW);    //ked dostane signal tak zhasne ledka na 1 sek
      delay(1000);
    }  
  }
}



void  calibration_min(){
  while( !(vol2 == true) ){            //program nepojde dalej dokial nedostane 2 signaly pre + a -
    
    if ( (irrecv.decode(&signals)) ) { // tato podmienka sa vykona druha az ked je splnena ta pod nou --> ulozi signal -
      Vol_minus = signals.value;
      signals.value=0;
      Serial.println("vol minus");    //kontrola
      Serial.println(Vol_minus);
      Serial.println(Vol_minus, HEX);
      irrecv.resume(); 
      vol2 = true;             //mam signal2
      digitalWrite(2,LOW);    //ked dostane signal tak zhasne ledka na 1 sek
      delay(1000);
      }
    } 
  }
  
