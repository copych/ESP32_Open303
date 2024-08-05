// Open303 port for ESP32
// by copych, 2023
// Debugging macros

#define JUKEBOX
#define JUKEBOX_PLAY_ON_START
#define DEBUG_JUKEBOX


#define SYNTH1_MIDI_CHAN        1
#define DEBUG_ON
//#define MIDI_VIA_SERIAL
#define MIDI_VIA_SERIAL2
#define MIDIRX_PIN      4       // this pin is used for input when MIDI_VIA_SERIAL2 defined (note that default pin 17 won't work with PSRAM)
#define MIDITX_PIN      0      // this pin will be used for output (not implemented yet) when MIDI_VIA_SERIAL2 defined

//#define NO_PSRAM
//#define USE_INTERNAL_DAC

#define SAMPLE_RATE     44100   // 44100 seems to be the right value, 48000 is also OK. Other values are not tested.

#define DMA_BUF_LEN     32          // there should be no problems with low values, down to 32 samples, 64 seems to be OK with some extra
#define DMA_NUM_BUF     2           // I see no reasom to set more than 2 DMA buffers, but...

#define I2S_BCLK_PIN    5
#define I2S_DOUT_PIN    6
#define I2S_WCLK_PIN    7


#ifndef MIDI_VIA_SERIAL
  #ifndef DEB
    #ifdef DEBUG_ON
      #define DEB(...) USBSerial.print(__VA_ARGS__) 
      #define DEBF(...) USBSerial.printf(__VA_ARGS__) 
      #define DEBUG(...) USBSerial.println(__VA_ARGS__) 
    #else
      #define DEB(...)
      #define DEBF(...)
      #define DEBUG(...)
    #endif
  #endif
#else
      #define DEB(...)
      #define DEBF(...)
      #define DEBUG(...)
#endif

#if defined MIDI_VIA_SERIAL2 || defined MIDI_VIA_SERIAL
#include <MIDI.h>
#endif

#ifdef MIDI_VIA_SERIAL
// default settings for Hairless midi is 115200 8-N-1
struct CustomBaudRateSettings : public MIDI_NAMESPACE::DefaultSerialSettings {
  static const long BaudRate = 115200;
};
MIDI_NAMESPACE::SerialMIDI<HardwareSerial, CustomBaudRateSettings> serialMIDI(Serial);
MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial, CustomBaudRateSettings>> MIDI((MIDI_NAMESPACE::SerialMIDI<HardwareSerial, CustomBaudRateSettings>&)serialMIDI);
#endif

#ifdef MIDI_VIA_SERIAL2
// MIDI port on UART2,   pins 16 (RX) and 17 (TX) prohibited, as they are used for PSRAM
struct Serial2MIDISettings : public midi::DefaultSettings {
  static const long BaudRate = 31250;
  static const int8_t RxPin  = MIDIRX_PIN;
  static const int8_t TxPin  = MIDITX_PIN;
};
MIDI_NAMESPACE::SerialMIDI<HardwareSerial> Serial2MIDI2(Serial2);
MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial, Serial2MIDISettings>> MIDI2((MIDI_NAMESPACE::SerialMIDI<HardwareSerial, Serial2MIDISettings>&)Serial2MIDI2);
#endif


#include "driver/i2s.h"
#include "rosic_Open303.h"


// tasks for Core0 and Core1
TaskHandle_t SynthTask1;
//TaskHandle_t SynthTask2;
const i2s_port_t i2s_num = I2S_NUM_0; // i2s port number
float bpm = 130.0f;

rosic::Open303 Synth;
rosic::AcidSequencer Sequencer;

size_t bytes_written; // i2s
volatile uint32_t s1t, s2t, drt, fxt, s1T, s2T, drT, fxT, art, arT; // debug timing: if we use less vars, compiler optimizes them

// Audio buffers of all kinds
static float mix_buf_l[DMA_BUF_LEN];    // mix L channel
static float mix_buf_r[DMA_BUF_LEN];    // mix R channel
static union { // a dirty trick, instead of true converting
  int16_t _signed[DMA_BUF_LEN * 2];
  uint16_t _unsigned[DMA_BUF_LEN * 2];
} out_buf; // i2s L+R output buffer
/*
hw_timer_t * timer1 = NULL;            // Timer variables
portMUX_TYPE timer1Mux = portMUX_INITIALIZER_UNLOCKED; 
volatile boolean timer1_fired = false;   // flag
volatile boolean timer1_flipper = false;   // flag
*/
/*
 * Timer interrupt handler **********************************************************************************************************************************
*/
/*
void IRAM_ATTR onTimer1() {
   portENTER_CRITICAL_ISR(&timer1Mux);
   timer1_fired = true;
   timer1_flipper = !timer1_flipper;
   portEXIT_CRITICAL_ISR(&timer1Mux);
}
*/

void setup() {
#ifdef DEBUG_ON
#ifndef MIDI_VIA_SERIAL
  USBSerial.begin(115200);
  DEBUG("DEBUG Started");
  delay(1000);
#endif
#endif

	btStop();
  DEBUG("BT Stopped");
  
	i2sInit();
  DEBUG("I2S Started");

  MidiInit();
  DEBUG("MIDI Started");

#ifdef JUKEBOX
  init_midi(); // AcidBanger function
#endif

	// xTaskCreatePinnedToCore( audio_task1, "SynthTask1", 8000, NULL, (1 | portPRIVILEGE_BIT), &SynthTask1, 0 );
	// xTaskCreatePinnedToCore( audio_task2, "SynthTask2", 8000, NULL, (1 | portPRIVILEGE_BIT), &SynthTask2, 1 );
  xTaskCreatePinnedToCore( audio_task1, "SynthTask1", 8000, NULL, 1, &SynthTask1, 0 );
	// xTaskCreatePinnedToCore( audio_task2, "SynthTask2", 8000, NULL, 1, &SynthTask2, 1 );

	// somehow we should allow tasks to run
	xTaskNotifyGive(SynthTask1);
	//xTaskNotifyGive(SynthTask2);

  /*
  // timer interrupt
  timer1 = timerBegin(0, 80, true);               // Setup timer 
  timerAttachInterrupt(timer1, &onTimer1, true);  // Attach callback
  timerAlarmWrite(timer1, 500000, true);            //  autoreload timer
  timerAlarmEnable(timer1);
  */
  Sequencer.start();
}

void loop() {
	
#ifdef MIDI_VIA_SERIAL
  MIDI.read();
#endif

  taskYIELD(); // this can wait

#ifdef MIDI_VIA_SERIAL2
  MIDI2.read();
#endif
/*
  if(timer1_fired) {
    timer1_fired = false;
    if (timer1_flipper) {
      Synth.noteOn(50, 100, 0.0f);  
      DEBUG("noteOn");
    } else {
      Synth.noteOn(50, 0, 0.0f);  
      DEBUG("notesOFF");
    }
  }
*/
#ifdef JUKEBOX
  run_tick();
  myRandomAddEntropy((uint16_t)(micros() & 0x0000FFFF));
#endif

}


// Core0 task
static void audio_task1(void *userData) {
  DEBUG ("TASK 1 Started");
  while (true) {
    if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
      s1t = micros();
      for (int i = 0 ; i < DMA_BUF_LEN; i++) {
        mix_buf_l[i] = mix_buf_r[i] = Synth.getSample();   
      }
      s1T = micros() - s1t;
    }
 // DEBF("time=%dus , sample=%e\r\n" , s1T, mix_buf_l[0]);
    i2s_output();
    
    taskYIELD();
    
  }
}
