/*
 * This sketch illustrates opening files by index
 * which can significantly reduce latency.
 *
 * How to prepare a test SD:
 * Start with a clean newly formatted SD.
 * First copy the 400 files in the 'fill' folder to the SD.
 * Next copy the 16 files in the 'DTMF' folder to the SD.
 * There should be 416 files in the SD root directory.
 *
 * You must copy the files in the above order so the 'fill'
 * files occur in the directory before the DTMF files.
 *
 * Run this sketch using the prepared SD.  Notice the
 * difference in latency between play by name and
 * play by index.
 */

// Number of files.
const int FILE_COUNT = 90;
const int FILE_COUNT_STRING = 15;
const int STRING_NUMBER	= 6;
// the number of guitor's fret
const int FRET_NUMBER  =  13;
// the sensitivity of analog input.
const int Sensitivity   =  20 ;
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
const int STRINGS[] = { 2, 3, 4, 5, 6, 7 };

// define standard value of each key
const int FretKeyVals[] = {
1023, 980, 937, 890, 835, 776, 708, 626, 538, 427, 301, 157, 0 ,
1023, 980, 937, 890, 835, 776, 708, 626, 538, 427, 301, 157, 0 ,
1023, 980, 937, 890, 835, 776, 708, 626, 538, 427, 301, 157, 0 ,
1023, 980, 937, 890, 835, 776, 708, 626, 538, 427, 301, 157, 0 ,
1023, 980, 937, 890, 835, 776, 708, 626, 538, 427, 301, 157, 0 ,
1023, 980, 937, 890, 835, 776, 708, 626, 538, 427, 301, 157, 0 
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
int stringValue = LOW;     // value read from digital input; 
                            //value "LOW" indicates the string has been played, and "HIGH" indicates it has not been played

/*
 * Define macro to put error messages in flash memory
 */
//////////////////////////////////// SETUP
void setup() {
  Serial.begin(9600);
  
  for (int i=0; i<STRING_NUMBER; i++)
    pinMode(STRINGS[i], INPUT);
}

  int ctrl = 10;
//////////////////////////////////// LOOP
void loop() { 
  //playByIndex(30);
  //playByIndex(40);
  //delay(1000);
  //playAllByIndex();
  //playByName("510");
  //delay(2000);

  int i=0, j=0;
  for(i=0; i<STRING_NUMBER; i++)
  {

    sensorValue = analogRead(FRETS[i]);
    stringValue = digitalRead(STRINGS[i]);
//stringValue = LOW;
    if( stringValue== HIGH )
    {

      // string "i" is played, then find is there any pret is pressed
      for(j=0; j<FRET_NUMBER; j++)
      {
        // compare key value read from analog input with standard key value, to find which key is presssed
        if( abs(sensorValue - FretKeyVals[i*FRET_NUMBER+j]) <= Sensitivity )
            break;
      }
 
     // the value of variable "i" indicates which string is pressed
     // the value of variable "j" indicates which pret is pressed. if the value of "j" is 0, no pret is pressed.
     // from TuneFile[i*FRET_NUMBER+j], you can get the tune file name which will be played
   
      // print the results to the serial monitor:
     // Serial.print("String No = " );                       
     // Serial.print(i);      
     // Serial.print("\t stringValue = ");
     // Serial.print(stringValue);
     // Serial.print("\t sersorValue = ");
     // Serial.print(sensorValue);
     // Serial.print("\t Fret No = ");  
     // Serial.print(j);
     // Serial.print("\t FretKeyVals Value = ");
     // Serial.print(FretKeyVals[i*FRET_NUMBER+j]);
      if(j<FRET_NUMBER)
      {
      //  Serial.print("\t Tune File Name = ");
     //  Serial.println(TuneFile[i*FRET_NUMBER+j]);   
 
        playByName(TuneFile[i*FRET_NUMBER+j]);
      }
     // else 
     //   Serial.println("");
    }
  //  else 
   // {
   //   Serial.print("String No = " );                       
   //   Serial.print(i);      
    //  Serial.print("\t stringValue = ");
    //  Serial.println(stringValue);
    //}

 }

  // after the last reading:
 // Serial.println("");
   delay(298); 
}

/*
 * Play file by file index in FAT file system
 * the index of first file is 0
 */
void playByIndex(uint8_t fileIndex) {
 // Serial.println(fileIndex);
  byte TXData = fileIndex;
  Serial.write(TXData);
}

void playAllByIndex(void) {
  for (uint8_t i = 0; i < FILE_COUNT; i++) {
    playByIndex(i);
  }
}

/*
 * Play file by name and print latency in ms
 */
void playByName(int fileName) {
  int i=0;
  i = (fileName / 100 - 1)*FILE_COUNT_STRING + fileName % 100;
  
  if(i<FILE_COUNT)
    playByIndex(i);
}

