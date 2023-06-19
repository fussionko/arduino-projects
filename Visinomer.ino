#include <Adafruit_BME280.h>

#include <Adafruit_ST7735.h> 

#define SEALEVELPRESSURE_HPA (1013.25)

#define TFT_CS        10
#define TFT_RST        9 
#define TFT_DC         8

Adafruit_BME280 bme;

Adafruit_ST7735 tft=Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int SW=A2;
int SW_state;

float tocka_a;
float tocka_b;
int x=0;

float cas=0;
float teza;

void setup() {
  Serial.begin(9600);  
  bme.begin(0x76);

  pinMode(SW, INPUT_PULLUP);

  //Inicializacija zaslona na belo barvo
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3); //obrne zaslon na pravo stran
  tft.background(0, 0, 255);
  
  reset();
  
  //Vnos teze v kg;
  teza=81;
}

bool klik() {
  SW_state = analogRead(SW);
  if(SW_state<=50)
    return 1;
  return 0;
}

float vpis() {
  x++;
  return bme.readAltitude(SEALEVELPRESSURE_HPA);
}



float pokKal() {
  cas/=3600;
  if((tocka_b-tocka_a)>0)
    return 6*cas*teza;
  return 5*cas*teza;
}

void reset() {
  tft.fillScreen(ST7735_WHITE);
  cas=0;
  x=0;

  tft.setTextSize(1.8);
  tft.setTextColor(ST7735_BLUE);
  tft.setCursor(2, 10);   
  tft.print(1);
  tft.setCursor(2, 40);   
  tft.print(2);
  tft.drawLine(2, 65, 40, 65, ST7735_BLACK); 
  tft.setCursor(2, 70);  
  tft.print("Razlika");
  tft.setCursor(60, 70); 
  tft.print("Cas");
  tft.setCursor(76, 30); 
  tft.print("Kalorije");
}

void loop() {
}
