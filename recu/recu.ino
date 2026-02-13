#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int soundPin = A0;
int photoPin = 3;

void setup() {
  pinMode(photoPin, INPUT);
  
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED no encontrada");
    while(true);
  }

  display.clearDisplay();
}

void loop() {

  int soundValue = analogRead(soundPin);
  int photoValue = digitalRead(photoPin);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,0);
  display.print("Sonido: ");
  display.println(soundValue);

  display.setCursor(0,20);
  display.print("Interruptor: ");

  if(photoValue == HIGH){
    display.println("Detectado");
  } else {
    display.println("Libre");
  }

  display.display();

  delay(300);
}

