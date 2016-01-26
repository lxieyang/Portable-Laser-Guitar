#include <WaveHC.h>
#include <WaveUtil.h>

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader file;   // This object represent the WAV file 
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

// Number of files.
#define FILE_COUNT 90

// array fileList store file name of all the wav files in SD card, 
// but actually, the real file name is not necessary, it can be file code there.
// the sequence of element must be same with the sequence of file in FAT system
char *fileList[] =  {
  "100", "101", "102", "103", "104", "105", "106", "107", "108", "109", "110", "111", "112", "113", "114",
  "200", "201", "202", "203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214",
  "300", "301", "302", "303", "304", "305", "306", "307", "308", "309", "310", "311", "312", "313", "314",
  "400", "401", "402", "403", "404", "405", "406", "407", "408", "409", "410", "411", "412", "413", "414",
  "500", "501", "502", "503", "504", "505", "506", "507", "508", "509", "510", "511", "512", "513", "514",
  "600", "601", "602", "603", "604", "605", "606", "607", "608", "609", "610", "611", "612", "613", "614"
}; 

// index of DTMF files in the root directory
uint16_t fileIndex[FILE_COUNT];

/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))

//////////////////////////////////// SETUP
void setup() {
  Serial.begin(9600);

  if (!card.init()) error("card.init");
  // enable optimized read - some cards may timeout
  card.partialBlockRead(true);
  if (!vol.init(card)) error("vol.init");
  if (!root.openRoot(vol)) error("openRoot");
}

//////////////////////////////////// LOOP
void loop() { 
  byte RXData; 
  if(Serial.available())
  {
    RXData = Serial.read();
    uint8_t fileIndex = RXData;
    playByIndex(fileIndex+1);
    Serial.println(int(fileIndex+1));
  }
}

/////////////////////////////////// HELPERS
/*
 * print error message and halt
 */
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}
/*
 * print error message and halt if SD I/O error, great for debugging!
 */
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
 // Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
//  Serial.println(card.errorData(), HEX);
  while(1);
}

/*
 * Play file by file index in FAT file system
 * the index of first file is 0
 */
void playByIndex(uint8_t fileIndex) {
 // Serial.print(fileIndex);
  //Serial.print("(");
  //Serial.print((char)fileIndex);
  //Serial.print(")");
  if(fileIndex<FILE_COUNT) {
   // Serial.print(":   ");
    if (!file.open(root, fileIndex+3)) {
      error("open by index");
    }

    // create and play Wave
    if (!wave.create(file)) error("wave.create");
    wave.play();

    uint8_t n = 0;
    while (wave.isplaying) {// playing occurs in interrupts, so we print dots in realtime
      putstring(".");
      if (!(++n % 32))//Serial.println();
      delay(100);
    }       
    // check for play errors
    sdErrorCheck();
  }
 // Serial.println();
}

void playAllByIndex(void) {
  for (uint8_t i = 0; i < FILE_COUNT; i++) {
    playByIndex(i);
  }
  PgmPrintln("Done");
}

/*
 * Play file by name and print latency in ms
 */
void playByName(char *fileName) {
  uint8_t i;
  for(i=0; i<FILE_COUNT; i++)
    if(strcmp(fileName, fileList[i])==0) break;

  if(i<FILE_COUNT)
    playByIndex(i);
}


