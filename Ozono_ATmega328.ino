/*

  Progetto:    Ozonatore ambiente
  Versione:    V.2.0
  Data:        16/04/2020
  Autore:      L.M
  
*/

// Parametri per display oled 0.96"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     -1 
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// porte I/O
#define led_v 2
#define led_r 3
#define start_stop 4
#define ozono 5
#define ventola 6

// sistema
uint8_t stato = 0;          // stato della macchina
uint8_t max_stato = 4;
#define durata_ventola 10
#define durata_ozono 4
char buf[20];
#define Sec " Sec."
#define Min " Min."

// Timer
uint16_t tempo_base = 1000;       // tempo confronto di 1 secondo
unsigned long int start_time;
unsigned long int end_time;
boolean blinka = false;
boolean timer_sec = false;
uint8_t secondo = 0;        // per conteggiare i secondi
uint8_t timer_ventola = 0; // durata pre/post ventilazione
uint8_t timer_ozono = 0;   // durata ozonizzazione

// inizializzazione del sistema
void setup (){
  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  oled.clearDisplay();
  oled.setTextSize(1);oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.print("Inizializzazione");
  oled.display();
// inizializzo pin I/O
  pinMode(led_v,OUTPUT);
  pinMode(led_r,OUTPUT);
  pinMode(start_stop,INPUT_PULLUP);
  pinMode(13,OUTPUT);
  oled.setCursor(0, 7);
  oled.print("Init I/O");
  oled.display();delay(2000);
  // init ventola
  pinMode(ventola,OUTPUT);digitalWrite(ventola,HIGH);
  oled.setCursor(0, 16);
  oled.print("Init Ventola");
  oled.display();delay(2000);
  pinMode(ventola,OUTPUT);digitalWrite(ventola,LOW);
  // init ozono
  pinMode(ozono,OUTPUT);digitalWrite(ozono,HIGH);
  oled.setCursor(0, 25);
  oled.print("Init Ozono");
  oled.display();delay(2000);
  pinMode(ozono,OUTPUT);digitalWrite(ozono,LOW);
  // attivazione del timer
  start_time = millis();
}

void loop (){
  gest_timer();
  gest_tasto();
  switch (stato){
    case 0:
      digitalWrite(led_v,HIGH);
      digitalWrite(led_r,LOW);
    break;
    case 1:
      digitalWrite(ventola,HIGH);
      if(timer_ventola == 0){
        timer_ventola = durata_ventola;
        secondo = 0;
      }
      if(timer_sec == true){
        digitalWrite(led_v,blinka);
        digitalWrite(led_r,LOW);
      }
    break;
    case 2:
      digitalWrite(ozono,HIGH);digitalWrite(ventola,HIGH);
      if(timer_ozono == 0){
        timer_ozono = durata_ozono;
        secondo = 0;
      }if(timer_sec == true){
        digitalWrite(led_v,LOW);
        digitalWrite(led_r,blinka);
      }
    break;
    case 3:
      digitalWrite(ozono,LOW);digitalWrite(ventola,HIGH);
      if(timer_ventola == 0){
        timer_ventola = durata_ventola * 2;
        secondo = 0;
      }
      if(timer_sec == true){
        digitalWrite(led_v,blinka);
        digitalWrite(led_r,!blinka);
      }
    break;
    case 4:
      digitalWrite(ozono,LOW);digitalWrite(ventola,LOW);
      timer_ozono = 0;timer_ventola = 0;
      stato = 0;
    break;
  }
  if(timer_sec == true)timer_sec = false;
}

// gestione timer
void gest_timer(){
  end_time = millis();
  if((end_time-start_time) >= tempo_base){
    start_time = end_time;
    timer_sec = true;
    blinka = !blinka;
    if(timer_ventola != 0){
      timer_ventola-=1;
      if(timer_ventola == 0){
        digitalWrite(ventola,LOW);stato+=1;
      }
    }
    digitalWrite(13, !digitalRead(13));
    gest_visual();
    secondo+=1;
    if (secondo >= 60 ){
      secondo = 0;
      if(timer_ozono != 0){
        timer_ozono-=1;
        if(timer_ozono == 0){
          digitalWrite(ozono,LOW);stato+=1;
        }
      }
    }
  }
}

// gestione del pulsante
void gest_tasto(){
  if (digitalRead(start_stop) == LOW ){
    delay (200);
    while(digitalRead(start_stop) == LOW){}
    if(stato == 0){
      stato = 1;
    }else{
      stato = 3;
    }
  }
}

// visualizzazione dei dati su display Oled
void gest_visual(){
  // azzeramento area visibile del display
  oled.clearDisplay();oled.setCursor(0, 0);
  oled.setTextSize(1);oled.setTextColor(WHITE);
  switch (stato){
    case 0:
      oled.print("Stato Riposo");
      oled.setCursor(0, 7);
      oled.print("ozono off");
      oled.setCursor(0, 16);
      oled.print("ventola off");
    break;
    case 1:
      oled.print("Stato Preparazione");
      oled.setCursor(0, 7);
      oled.print("ozono off");
      oled.setCursor(0, 16);
      oled.print("ventola on");
      oled.setCursor(0, 25);
      oled.print("secondi ");
      sprintf(buf, "%0.2hd" Sec , timer_ventola);
      oled.print(buf);
    break;
    case 2:
      oled.print("Stato Ozonizzo");
      oled.setCursor(0, 7);
      oled.print("ozono on");
      oled.setCursor(0, 16);
      oled.print("ventola on");
      oled.setCursor(0, 25);
      oled.print("minuti ");
      sprintf(buf, "%0.2hd" Min , timer_ozono);
      oled.print(buf);
    break;
    case 3:
      oled.print("Stato Diffusione");
      oled.setCursor(0, 7);
      oled.print("ozono off");
      oled.setCursor(0, 16);
      oled.print("ventola on");
      oled.setCursor(0, 25);
      oled.print("secondi ");
      sprintf(buf, "%0.2hd" Sec , timer_ventola);
      oled.print(buf);
    break;
  }
  oled.display();
}
