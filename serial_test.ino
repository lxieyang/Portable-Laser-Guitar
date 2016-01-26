#include <WaveHC.h>
#include <WaveUtil.h>

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader file;   // This object represent the WAV file 
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

// Number of files.
const int FILE_COUNT = 90;
const int FILE_COUNT_STRING = 15;
const int STRING_NUMBER	= 6;
// the number of guitor's fret
const int FRET_NUMBER   =   13;
// the sensitivity of analog input.
const int Sensitivity   =   18;

// array fileList store file name of all the wav files in SD card, 
// but actually, the real file name is not necessary, it can be file code there.
// the sequence of element must be same with the sequence of file in FAT system
const int fileList[] =  {
				100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
				200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214,
				300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314,
				400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414,
				500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 513, 514,
				600, 601, 602, 603, 604, 605, 606, 607, 608, 609, 610, 611, 612, 613, 614
                      }; 
      
// the number of guitor's string
const int FRETS[] = { A0, A1, A2, A3, A4, A5 };
const int STRINGS[] = { 0, 1, 2, 3, 4, 5 };

// define standard value of each key
const int FretKeyVals[] = {
				 1023, 980, 930, 878, 819, 753, 680, 600, 510, 406, 290, 155, 0 ,
				 1023, 980, 930, 878, 819, 753, 680, 600, 510, 406, 290, 155, 0 ,
				 1023, 980, 930, 878, 819, 753, 680, 600, 510, 406, 290, 155, 0 ,
				 1023, 980, 930, 878, 819, 753, 680, 600, 510, 406, 290, 155, 0 ,
				 1023, 980, 930, 878, 819, 753, 680, 600, 510, 406, 290, 155, 0 ,
				 1023, 980, 930, 878, 819, 753, 680, 600, 510, 406, 290, 155, 0 
		};
		
// define file name code for each tune				
const int TuneFile[] = {
				 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
				 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212,
				 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312,
				 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412,
				 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512,
				 600, 601, 602, 603, 604, 605, 606, 607, 608, 609, 610, 611, 612
		};

int sensorValue = 0;        // value read from analog input;
int stringValue = HIGH;     // value read from digital input; 
                            //value "LOW" indicates the string has been played, and "HIGH" indicates it has not been played

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

  for (int i=0; i<STRING_NUMBER; i++)
    pinMode(STRINGS[i], INPUT);
    
  playByIndex(10);
  playByIndex(60);
}

//////////////////////////////////// LOOP
void loop() { 
  delay(4000);
  for (int i=0; i<STRING_NUMBER; i++) {
  stringValue = digitalRead(STRINGS[i]);
  Serial.println(stringValue);
  if(stringValue==LOW)
    playByIndex(10);
  else
    playByIndex(60);
    delay(1000);
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
  PgmPrint(", ");
  while(1);
}

/*
 * Play file by file index in FAT file system
 * the index of first file is 0
 */
void playByIndex(uint8_t fileIndex) {
    if (!file.open(root, fileIndex+3)) {
      error("open by index");
    }
   
    // create and play Wave
    if (!wave.create(file)) error("wave.create");
    wave.play();
    
       uint8_t n = 0;
        while (wave.isplaying) {// playing occurs in interrupts, so we print dots in realtime
          //putstring(".");
          //if (!(++n % 32))Serial.println();
          delay(100);
        }       
    // check for play errors
    sdErrorCheck();
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
void playByName(int fileName) {
  int i=0;
  /*
  for(i=0; i<FILE_COUNT; i++)
  {
    Serial.print("FileName=");
    Serial.print(fileName);
    Serial.print(",   ");
    Serial.print("FileList[i]=");
    Serial.println(fileList[i]);
    //if(fileName==fileList[i]) break;
  }
  */
  i = (fileName / 100 - 1)*FILE_COUNT_STRING + fileName % 100;
  
  if(i<FILE_COUNT)
    playByIndex(i);
}
