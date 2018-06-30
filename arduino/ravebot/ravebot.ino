
const unsigned long int BAUD = 38400;
const unsigned long int PWM_PERIOD = 10000; // us

enum EL_CHAN {
  EL_A = 2,
  EL_B,
  EL_C,
  EL_D,
  EL_E,
  EL_F,
  EL_G,
  EL_H,
  EL_STATUS = 13
};

class ELChannel {
  public:
    ELChannel(int pin) 
      : brightness(0.0), m_pin(pin), m_last(0) {};

    float brightness;
    
    void Update() {
      auto now = micros();
      auto phase = micros() % PWM_PERIOD;
      auto onTime = brightness * PWM_PERIOD;

      if (phase < onTime) {
        digitalWrite(m_pin, HIGH);
      } else if (phase >= onTime) {
        digitalWrite(m_pin, LOW);
      }

      m_last = phase;
    }
    
  private:
    int m_pin;
    unsigned long int m_last;
};

// ---- MAIN FUNCTIONS ----

bool hasReceivedCommand = false;

char buf[4];

ELChannel channels[4] = {
  ELChannel(EL_A), 
  ELChannel(EL_B), 
  ELChannel(EL_C),
  ELChannel(EL_D)
};

void setup() {      
  setupELPins();          
  Serial.begin(BAUD);
}

void loop() {
  parseMessage();
  for (int c=0; c<4; c++) { 
    if (!hasReceivedCommand) {
      float b = sin(PI * (millis() * 0.0005 + float(c))) * 0.5 + 0.5;
      channels[c].brightness = b;
    }
    channels[c].Update();
  }
}

// --- UTILITY ---

void setupELPins() {
  for (int c = EL_A; c <= EL_H; c++) {
    pinMode(c, OUTPUT);
  }
  pinMode(EL_STATUS, OUTPUT);
}

void parseMessage() {
  // 4-byte messages: 'C' + idx + 'B' + brightness

  // Flush buffer until we have a C at the front
  while (Serial.available() > 0 && Serial.peek() != 'C') {
    Serial.read();
  }

  // Wait for a 4 byte message
  if (Serial.available() < 4) {
    return;
  }
  
  Serial.readBytes(buf, 4);
  if (buf[0] != 'C' && buf[2] != 'B') return;

  hasReceivedCommand = true;
 
  char channel = buf[1];
  unsigned char brightness = buf[3];
  channels[channel].brightness = float(brightness)/255.0;
}

