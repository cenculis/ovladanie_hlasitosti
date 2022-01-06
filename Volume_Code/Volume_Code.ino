#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>
#include <IRremote.h>
#include <EEPROM.h>

int ReadPin=A1;    // mikrofon pin
float hladinaa=A4;   // pomocna premenna pre spodnu hranicu
int cass=A5;       // pomocna premenna pre hornu hranicu 

int input_pin = 2; // signalovy pin z IR prijimaca
int button=4;      // button na spustenie kalibracie 
int soundPlusCounter;
int mic;           // vstup z mikrofonu
int u;             // hladina max hodnot na danych intervaloch
int Umaxval=610;   // pomocna premenna pri urcovani u - nastavena na hodnotu nulovej hladiny   
int hlasitost=600; // pomocna premenna na simulovanie a vypisanie pridavania a uberania hlasitosti
int cas;           // horna hranica ktora sa nastavuje pomocou potenciometra
float hladina;       // spodna hranica ktora sa nastavuje pomocou potenciometra
float maxval,minval; 

// cervena ledka pin 6 
// modra ledka pin 8 
// IR ledka 3 

IRrecv irrecv(input_pin);
decode_results signals;

//premenne pre ovladac
unsigned long int Vol_plus;               //do tychto uklada hodnoty signalu pre + a -
unsigned long int Vol_minus;
boolean vol1,vol2 = false;                //pomocne premenne, vo while cykle ci uz ma oba signaly alebo nie

//premenne pre meranie casu vo funkcii maxvolume
unsigned long startMillis;  
unsigned long currentMillis;
const unsigned long period1 = 100; //dlzka intervalu, na ktorom bude merat maximalnu hodnotu

//premenne pre meranie casu vo funkcii volume
unsigned long startmill; 
unsigned long currentmill;
long period2; // dlzka intervalu odozvy na zmenu hlasitosti

void setup() {
  
  pinMode(ReadPin,INPUT);
  pinMode(hladinaa,INPUT);
  pinMode(cass,INPUT);
  pinMode(button,INPUT);   //tlacitko
  
  pinMode(8,OUTPUT);  //ledky cervena a modra 
  pinMode(6,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);  //nastavenie pinu pre infracervenu diodu
  
  IrSender.begin();  //iniciaizacia prenosu pomocou Infracervenej diody 
  Serial.begin(115200);
  startMillis = millis();
  startmill = millis();
  
  Vol_plus = readValue(10);   //pri spusteni nacita ulozene hodnoty z EEPROM
  Vol_minus = readValue(20);
  
}

void loop() {

    if(digitalRead(button) == HIGH){  
    calibration();
    vol1 = false;
    vol2 = false;
    
    }
    if(soundPlusCounter==15){
      YieldDelay(10000);
      soundPlusCounter=0;
      }
  digitalWrite(8,LOW);
  cas = map(analogRead(cass),0.00,1024.00,100.00,3000.00);  //zmena velkosti premennej pomocou funkcie map
  hladina = map(analogRead(hladinaa),0.0,1024.0,0.0,50.0);          //zmena velkosti premennej pomocou funkcie map
  mic=analogRead(ReadPin)+300;                              // +300 lebo moja nulova hodnota bola 500 a nechcelo sa mi vsetko prepisova :D 
  
  maxvolume(mic);
  volume(u);
  maxval=620+(hladina*2.2);
  minval=620+(hladina*1.2);
  period2= cas;
  
//  Serial.print(hlasitost);
  Serial.print(" ");
  Serial.print(maxval); //horna hranica
  Serial.print(" ");
  Serial.print(minval); //spodna hranica
  Serial.print(" ");
  Serial.print(u);      //hladina max hodnot
  Serial.print(" ");
  Serial.println(mic);  
  
}



//funkcia na získanie delayu bez pouzitia prikazu delay()

void YieldDelay(unsigned long ms)
{
    auto now = millis();
    while (millis() < (now + ms))    //funkcia na blikanie ledky bez delay
    {}
}

//funkcia sluzi na urcovanie hladiny maximalnych hodnot - vystup je u (potom neskor to mozme upravit a spravit lokalne premenne)

void maxvolume(int mic) {
  currentMillis = millis();  
  if (mic>Umaxval) {Umaxval=mic;}
  
  if (currentMillis - startMillis >= period1)  //kontroluje interval
  {
     u=Umaxval;
    startMillis = currentMillis;              // "vynulovanie intervalu"
    Umaxval=610;                              // nastavime max hodnotu na nulovu hladinu
  }
}


//pridavanie a uberanie hlasitosti 
   
void volume(int u){
  if (u < minval){
     currentmill = millis();
         if (currentmill - startmill >= period2*4 && soundPlusCounter<15)   // kontroluje ci dana podmienka bude platit aj po casovom intervale  period2
           {
                  digitalWrite(8, HIGH);           //infoledka aby sme vedeli ze fungujeme 
                  IrSender.sendNEC(0x4,0x2,0); //+ zvuk
                  soundPlusCounter++;
                  YieldDelay(100);
                  digitalWrite(8, LOW);
            // hlasitost=hlasitost+10;
            // if (hlasitost==710) {hlasitost=700;}  //kontrola maximalnej hodnoty
             startmill = currentmill;
           }
          
         
  }
if (u > maxval ){
     currentmill = millis();
         if (currentmill - startmill >= period2)  
           {
                digitalWrite(6, HIGH);          //infoledka aby sme vedeli ze fungujeme 
               IrSender.sendNEC(0x4,0x3,0);  //- zvuk
                YieldDelay(100);
                digitalWrite(6, LOW);
           //  hlasitost=hlasitost-10;
           //  if (hlasitost==590) {hlasitost=600;} //kontrola maximalnej hodnoty
             startmill = currentmill; 
           }
  }
}


void calibration(){

  irrecv.enableIRIn();              //nanovo inicializuje vzdy ked sa zapne tlacitko
  
  while( !((vol1 == true) && (vol2 == true)) ){            //program nepojde dalej dokial nedostane 2 signaly pre + a -
    
    if ( (irrecv.decode(&signals)) && (vol1 == true) ) { // tato podmienka sa vykona druha az ked je splnena ta pod nou --> ulozi signal -
      Vol_minus = signals.value;
      
      Serial.println("vol minus");    //kontrola
      Serial.println(Vol_minus);
      Serial.println(Vol_minus, HEX);
      
      writeValue(20, Vol_minus);    //na adresu 20,21,22,23 zapise vol_minus
      vol2 = true;                //mam signal2
      digitalWrite(8,LOW);        //ked dostane signal tak zhasne ledka na 2 sek
      delay(2000);
    }
  
    if ( (irrecv.decode(&signals)) && (vol1 == false)) { // tato podmienka sa vykona prva --> ulozi signal +
      Vol_plus = signals.value;
      
      Serial.println("vol plus");     //kontrola
      Serial.println(Vol_plus);
      Serial.println(Vol_plus, HEX);
      
      writeValue(10, Vol_plus);     //na adresu 10,11,12,13 zapise vol_plus
      irrecv.resume();              // priprava na prijatie 2. signalu?
      vol1 = true;             //mam signal1
      digitalWrite(8,LOW);    //ked dostane signal tak zhasne ledka na 2 sek
      delay(2000);
    }
    
    digitalWrite(8,HIGH); // dokial nedostane signal tak svieti ledka
  }
  
}

//ukladanie + a - na EEPROM 

void writeValue(int address, long number){
  
  EEPROM.write(address, (number >> 24) & 0xFF);
  EEPROM.write(address + 1, (number >> 16) & 0xFF);
  EEPROM.write(address + 2, (number >> 8) & 0xFF);
  EEPROM.write(address + 3, number & 0xFF);
}

long readValue(int address){
  
  return ((long)EEPROM.read(address) << 24) +
         ((long)EEPROM.read(address + 1) << 16) +
         ((long)EEPROM.read(address + 2) << 8) +
         (long)EEPROM.read(address + 3);
}
         
         
  
