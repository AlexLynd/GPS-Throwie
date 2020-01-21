#include <SD.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <TinyGPS++.h>

#define SD_CS    2
#define LOG_FILE_PREFIX "gpslog"
#define LOG_FILE_SUFFIX "csv"
#define MAX_LOG_FILES 100

char logFileName[13];
#define LOG_COLUMN_COUNT 11
const String header = "Lat ,Lon, Time, Altitude, HDOP";
#define LOG_RATE 500
unsigned long lastLog = 0;
const int interval = 10; // log interval minutes

SoftwareSerial ss(0, 15); // RT
TinyGPSPlus tinyGPS;

void setup() {
  Serial.begin(115200); ss.begin(9600);
  Serial.println("\nStarting the GPS Throwie!");
  Serial.println("----------------------------");
  Serial.print("Setting up SD card: ");
  if (!SD.begin(SD_CS)) { Serial.println("not found."); }
  while (!SD.begin(SD_CS)) ; // run until SD card found 
  Serial.println("found."); initializeSD();
}

void loop() {
  if (tinyGPS.location.isValid()) {
        logshit();
  }
  smartDelay(LOG_RATE);

  if (millis() > 5000 && tinyGPS.charsProcessed() < 10) Serial.println("No GPS data received: check wiring");
}

static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available())
      tinyGPS.encode(ss.read());
  } while (millis() - start < ms);
}

void initializeSD() {
  for (int i=0; i< MAX_LOG_FILES; i++) {
    memset(logFileName, 0, strlen(logFileName));
    sprintf(logFileName, "%s%d.%s", LOG_FILE_PREFIX, i, LOG_FILE_SUFFIX);
    if (!SD.exists(logFileName)) { break; } 
  }
   Serial.print("Creating ");
   Serial.println(logFileName);
   File logFile= SD.open(logFileName, FILE_WRITE);
   if (logFile) {
     logFile.println(header);
    logFile.close();
  }
}

void logshit() {
  File logFile= SD.open(logFileName, FILE_WRITE);
  logFile.print(tinyGPS.location.lat(), 6);    logFile.print(',');
  logFile.print(tinyGPS.location.lng(), 6);    logFile.print(',');
  logFile.print(tinyGPS.date.month());         logFile.print('-');
  logFile.print(tinyGPS.date.day());           logFile.print('-');
  logFile.print(tinyGPS.date.year());          logFile.print(' ');
  logFile.print(tinyGPS.time.hour());          logFile.print(':');
  logFile.print(tinyGPS.time.minute());        logFile.print(':');
  logFile.print(tinyGPS.time.second());        logFile.print(',');
  logFile.print(tinyGPS.altitude.meters(), 1); logFile.print(',');
  logFile.print((tinyGPS.hdop.value(), 1));    logFile.println() ;
}
