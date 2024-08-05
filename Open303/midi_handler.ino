

inline void MidiInit() {
  
#ifdef MIDI_VIA_SERIAL
  Serial.begin(115200, SERIAL_8N1);
#endif
#ifdef MIDI_VIA_SERIAL2
  pinMode( MIDIRX_PIN , INPUT_PULLDOWN);
  pinMode( MIDITX_PIN , OUTPUT);
  Serial2.begin( 31250, SERIAL_8N1, MIDIRX_PIN, MIDITX_PIN ); // midi port
#endif

#ifdef MIDI_VIA_SERIAL
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleCC);
  MIDI.setHandlePitchBend(handlePitchBend);
  MIDI.setHandleProgramChange(handleProgramChange);
  MIDI.begin(MIDI_CHANNEL_OMNI);
#endif
#ifdef MIDI_VIA_SERIAL2
  MIDI2.setHandleNoteOn(handleNoteOn);
  MIDI2.setHandleNoteOff(handleNoteOff);
  MIDI2.setHandleControlChange(handleCC);
  MIDI2.setHandlePitchBend(handlePitchBend);
  MIDI2.setHandleProgramChange(handleProgramChange);
  MIDI2.begin(MIDI_CHANNEL_OMNI);
#endif

}


inline void handleNoteOn(uint8_t inChannel, uint8_t inNote, uint8_t inVelocity) {
#ifdef DEBUG_MIDI
  DEB("MIDI note on ");
  DEBUG(inNote);
#endif 
  Synth.noteOn(inNote, inVelocity, 0.0f);
}

inline void handleNoteOff(uint8_t inChannel, uint8_t inNote, uint8_t inVelocity) {
  Synth.noteOff(inNote, 0.0f);
}

inline void handleCC(uint8_t inChannel, uint8_t cc_number, uint8_t cc_value) {
  float norm_val ;
  switch (cc_number) { // global parameters yet set via ANY channel CCs
   
    case  CC_303_CUTOFF:
      norm_val = MIDI_NORM * cc_value;
      Synth.setCutoff(linToExp(norm_val, 0.0f, 1.0f, MIN_CUTOFF_FREQ, MAX_CUTOFF_FREQ));
      break;
    case  CC_303_RESO:
      Synth.setResonance(MIDI_NORM_100 * cc_value);
      break;
    case CC_303_ATTACK:
      break;
    case CC_303_DECAY:
      break;
    case CC_303_ENVMOD_LVL:
      Synth.setEnvMod(MIDI_NORM_100 * cc_value);
      break;
    case CC_303_ACCENT_LVL:
      Synth.setAccent(MIDI_NORM_100 * cc_value);
      break;
    case CC_303_VOLUME:
      //Synth.setVolume(amp2dBWithCheck((int)127-(int)cc_value, 0.000001f));
      break;
    case CC_303_DISTORTION:
      break;
    case CC_303_WAVEFORM:
      Synth.setWaveform(MIDI_NORM * cc_value);
      break;
    case  CC_303_PORTAMENTO:
      break;
    /*
#define CC_303_PORTATIME    5
#define CC_303_VOLUME       7
#define CC_303_PORTAMENTO   65
#define CC_303_PAN          10
#define CC_303_WAVEFORM     70
#define CC_303_RESO         71
#define CC_303_CUTOFF       74
#define CC_303_ATTACK       73
#define CC_303_DECAY        72
#define CC_303_ENVMOD_LVL   75
#define CC_303_ACCENT_LVL   76
#define CC_303_REVERB_SEND  91
#define CC_303_DELAY_SEND   92
#define CC_303_DISTORTION   94
#define CC_303_OVERDRIVE    95
#define CC_303_SATURATOR    128
*/
    case CC_ANY_RESET_CCS:
    case CC_ANY_NOTES_OFF:
    case CC_ANY_SOUND_OFF:
      Synth.allNotesOff();
      break; 

  }
}

void handleProgramChange(uint8_t inChannel, uint8_t number) {
}

inline void handlePitchBend(uint8_t inChannel, int number) {
  float semitones = ((((float)number + 8191.5f) * (float)TWO_DIV_16383 ) - 1.0f ) * 2.0f;
  Synth.setPitchBend(semitones);
}
