#include <SPI.h>
#include <mcp_can.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <math.h>

#define CAN_CS 10
#define CAN_INT 2
#define TERMISTOR A0

// ===== Parámetros NTC =====
#define R_REF 10000.0
#define BETA 3950.0
#define T0 298.15

MCP_CAN CAN(CAN_CS);
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);

  // ===== Inicializar CAN =====
  while (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) != CAN_OK) {
    Serial.println("Error CAN...");
    delay(500);
  }

  CAN.setMode(MCP_NORMAL);
  pinMode(CAN_INT, INPUT);

  // ===== Inicializar BMP180 =====
  if (!bmp.begin()) {
    Serial.println("BMP180 NO detectado");
    while (1);
  }

  Serial.println("EMISOR CAN LISTO");
}

void loop() {

  // ===== TERMISTOR =====
  int adc = analogRead(TERMISTOR);
  float v = adc * (5.0 / 1023.0);

  float r_ntc = R_REF * (5.0 / v - 1.0);

  float tempNTC = (1.0 / (log(r_ntc / R_REF) / BETA + 1.0 / T0)) - 273.15;

  // ===== BMP180 =====
  float tempBMP = bmp.readTemperature();
  int32_t presionPa = bmp.readPressure();   // Pascales

  // ===== TRAMA CAN =====
  byte data[8];

  data[0] = (int8_t)tempNTC;   // Temperatura NTC (°C)
  data[1] = (int8_t)tempBMP;   // Temperatura BMP (°C)

  // Presión en int32 (Pa)
  data[2] = (presionPa >> 24) & 0xFF;
  data[3] = (presionPa >> 16) & 0xFF;
  data[4] = (presionPa >> 8)  & 0xFF;
  data[5] = presionPa & 0xFF;

  data[6] = 0;
  data[7] = 0;

  CAN.sendMsgBuf(0x100, 0, 8, data);

  // ===== Debug Serial =====
  Serial.print("NTC: ");
  Serial.print(tempNTC);

  Serial.print("  BMP: ");
  Serial.print(tempBMP);

  Serial.print("  Presion: ");
  Serial.print(presionPa / 100.0);
  Serial.println(" hPa");

  delay(500);
}