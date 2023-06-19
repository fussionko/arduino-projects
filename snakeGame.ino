#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> 

#define TFT_CS        10
#define TFT_RST        9 
#define TFT_DC         8

Adafruit_ST7735 tft= 
Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int VRx=A0;
int VRy=A1;
int SW=A2;

int xPosition;
int yPosition;
int SW_state;
int mapX;
int mapY;

int smer_j;

int stTel;

int game;

int x;

int koliko;

//Igralno polje
int xp = 41;
int yp = 9;
int sv = 112;

struct Kord
{
  int x, y;
  Kord *nasl;
}*zacetek, *rep;

struct Glava
{
  int x, y;
} glava;


struct Sadje
{
  int x, y;
} sadje;


void setup() {
  Serial.begin(9600);
  posodobi();
}


//Nakljucno spawnanje sadja
void randSpawn(){
  Kord *tmp;
  bool x = 0;
  int x1, y1;
  while(x == 0){
    x1 = random(xp+1, sv);
    y1 = random(yp+1, sv);
    if(x1 != glava.x && y1 != glava.y)
      if((y1-yp-1)%4 == 0 && (x1-xp-1)%4 == 0){
        if(stTel != 0){
          for(tmp = zacetek; tmp != NULL; tmp=tmp->nasl)
            if(x1 != tmp->x && y1 != tmp->y){
              x = 1;
              break;
          }
        }
        else{
          x = 1;
        }
      }
  }
  if(x = 1){
    kvadrat(x1, y1, ST7735_RED);
    sadje.x = x1;
    sadje.y = y1;
  }
}


//Bere pozicijo joysticka
void readP(){
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = analogRead(SW);
  mapX = map(xPosition, 0, 1023, -512, 512);
  mapY = map(yPosition, 0, 1023, -512, 512);
}


//Najde smer
void smer(){ 
  int kam=smer_j;
  if(mapX>=-5 && mapX<=30 && mapY<=-480 && mapY>=-512 && kam!=2)
    smer_j = 1; 
    
  if(mapX>=-5 && mapX<=30 && mapY<=512 && mapY>=480 && kam!=1)
    smer_j = 2; 
    
  if(mapX>=-512 && mapX<=-480 && mapY<=30 && mapY>=-5 && kam!=4)
    smer_j = 3;

  if(mapX>=480 && mapX<=512 && mapY<=30 && mapY>=-5 && kam!=3)
    smer_j = 4;
}


//Zazna ce se zabijes v steno ali v kaco
void zabit(){
  Kord *tmp;
  if(glava.x<xp || glava.x>xp+sv || glava.y<yp || glava.y>yp+sv)
    game=1;
  else{
    if(x==1)
      for(tmp=zacetek; tmp!=NULL; tmp=tmp->nasl){
        Serial.print(glava.x);
        Serial.print("   ");
        Serial.print(glava.y);
        Serial.print("    ");
        Serial.print(tmp->x);
        Serial.print("   ");
        Serial.println(tmp->y);
        if(glava.x==tmp->x && glava.y==tmp->y){
          Serial.println("brisi");
          game=1;
          break;
        }
      }
    else
      x=1;     
  }
}


void gameOver(){
  if(koliko==0){
    //Zbrise zadnji del kace
    kvadrat(glava.x, glava.y, ST7735_WHITE);

    //Zbrise zaslon
    tft.fillRect(xp-2, yp-2, sv+4, sv+4, ST7735_WHITE);
    tft.drawRect(xp, yp, sv, sv, ST7735_BLACK);

    tft.setTextSize(2);
    tft.setCursor(xp+3, yp+sv/2-6);   
    tft.setTextColor(ST77XX_RED);
    tft.println("GAME OVER");
  }
  else
    koliko=1;

  while(SW_state!=0){
    SW_state = analogRead(SW);
    if(SW_state<=800){
      posodobi();
      break;
    }
  }
  
}


//Narise del kace
void kvadrat(int x1, int y1, int color){
  tft.drawRect(x1, y1, 4, 4, color);
  tft.fillRect(x1, y1, 4, 4, color);
}


//Vstavi telo na prvo mesto
void vstaviTelo(int x, int y){
  Kord *tmp=new Kord;
  tmp->x=x;
  tmp->y=y;
  tmp->nasl=zacetek;
  zacetek=tmp;
  if(tmp->nasl==NULL)
    rep=tmp;

}


//Brise zadnje telo
void brisiTelo(){
  int i=0;
  Kord *tmp, *prej;
  tmp=zacetek;
  prej=tmp;
  while(tmp!=rep){
    prej=tmp;
    tmp=tmp->nasl;
    i++;
  }
  rep=prej;
  prej->nasl=NULL;
  delete tmp;
}

//Napise besedo Score
void scoreNapis(){
  tft.setTextSize(1);
  tft.setCursor(2, 2);   
  tft.setTextColor(ST77XX_BLACK);
  tft.println("SCORE");
}

void posodobi(){
  //Postavi rep in zacetek na NULL
  zacetek=rep=NULL;
  smer_j=0;
  stTel=0;
  game=0;
  x=0;
  xPosition=0;
  yPosition=0;
  SW_state=0;
  mapX=0;
  mapY=0;
  koliko=0;

  randomSeed(analogRead(3));
 
  //Inicializacija vhodov
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT); 
  pinMode(SW, INPUT_PULLUP);

  //Inicializacija zaslona na belo barvo
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3); //obrne zaslon na pravo stran
  tft.fillScreen(ST7735_WHITE);

  
  //Narise igralno polje
  tft.drawRect(xp, yp, sv, sv, ST7735_BLACK); // x, y, sirina, visina, barva

  //Napise besedo Score
  scoreNapis();

  //Postavi kaco na sredino
  kvadrat(sv/2+xp+1, sv/2+yp+1, ST7735_BLACK); 
  
  //Dodamo kordinate v strukturo
  glava.x = sv/2+xp+1;
  glava.y = sv/2+yp+1;

  //Vstavi glavo kace v telo
  vstaviTelo(glava.x, glava.y);
  
  //Zacetno sadje
  randSpawn();

  rezultat();
}

//Telo kace
void telo(){
  zabit();
  kvadrat(glava.x, glava.y, ST7735_BLACK);
  if(glava.x==sadje.x && glava.y==sadje.y){
    x=0;
    stTel++;
    rezultat();
    vstaviTelo(glava.x, glava.y);
    randSpawn();
  }
  else{
      vstaviTelo(glava.x, glava.y);
      kvadrat(rep->x, rep->y, ST7735_WHITE);
      brisiTelo();
  }
}

void rezultat(){
  tft.setCursor(8, 13);
  tft.fillRect(8, 13, 25, 25, ST7735_WHITE);
  tft.setTextSize(2);
  tft.print(stTel);
}

//Premik kace s doloceno smer
void premik(){
  switch(smer_j){
    //GOR
    case(1): glava.y -=4;
             telo();
             break;
    //DOL
    case(2): glava.y +=4;
             telo();
             break;
    //LEVO
    case(3): glava.x -=4;
             telo();
             break;
    //DESNO
    case(4): glava.x +=4;
             telo();
             break;
  }
}


void loop() {
  if(game==0){
    readP();
    smer();
    premik();
  }
  else{
    gameOver();
  }
  delay(200);
}
