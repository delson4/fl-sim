
// undefine one of these two depending on which hardware is being compiled
#define ROTARY_SHIELD
// #define BUTTON_SHIELD

class PinMonitor {
  private:
    static constexpr int _HOLDOFF_TIME_MSEC = 20;
    int _pin_num;
    bool _last_emitted_state;
    unsigned long _holdoff_expiration_time;
    
  public:
    PinMonitor(int pin_num) {
      _pin_num = pin_num;
      _last_emitted_state = false;
      _holdoff_expiration_time = 0;
      pinMode(_pin_num, INPUT_PULLUP);
    }

    int pin_num() {
      return _pin_num;
    }
    
    bool poll() {
      // If we're still in the holdoff period, do nothing
      if (_holdoff_expiration_time && _holdoff_expiration_time < millis()) {
        return false;
      }

      _holdoff_expiration_time = 0;

      // If the pin state has not changed, do nothing
      bool is_pushed = (digitalRead(_pin_num) == false);
      if (is_pushed == _last_emitted_state) {
        return false;
      }

      // Pin state changed!
      _last_emitted_state = is_pushed;
      _holdoff_expiration_time = millis() + _HOLDOFF_TIME_MSEC;
      return true;
    }

    bool is_pushed() {
      return _last_emitted_state;
    }

    void print_debug_info() {
      Serial.print("Pin ");
      Serial.print(_pin_num);
      Serial.print(" is now ");
      if (_last_emitted_state) {
        Serial.println("PUSHED");
      } else {
        Serial.println("RELEASED");
      }
    }
};

class RotaryMonitor {
  private:
    PinMonitor _a, _b;
    uint8_t _oldState;

    const int XX = 0; // invalid transition
    const int8_t _quad_state_machine[16] = {
      0,   // 0000
      +1,  // 0001
      -1,  // 0010
      XX,  // 0011
      -1,  // 0100
      0,   // 0101
      XX,  // 0110
      +1,  // 0111
      +1,  // 1000
      XX,  // 1001
      0,   // 1010
      -1,  // 1011
      XX,  // 1100
      -1,  // 1101
      +1,  // 1110
      0    // 1111
    };

  public:
    RotaryMonitor(int pina_num, int pinb_num) : _a(pina_num), _b(pinb_num) {
      poll();
    }

    int poll() {
      // if neither of the underlying pins has changed state, do nothing
      if (!_a.poll() && !_b.poll()) {
        return 0;
      }

      uint8_t newState = (_a.is_pushed() << 1) | _b.is_pushed();
      uint8_t transition = (_oldState << 2) | newState;
      int8_t delta = _quad_state_machine[transition];
      _oldState = newState;
      return delta;
    }
};

#ifdef ROTARY_SHIELD

PinMonitor pins[] = {
  PinMonitor(2),
  PinMonitor(3),
  PinMonitor(4),
};

#define NUM_PINS (sizeof(pins) / sizeof(pins[0]))

RotaryMonitor rotors[] = {
  RotaryMonitor(11, 12),
};

#define NUM_ROTORS (sizeof(rotors) / sizeof(rotors[0]))

void setup() {
  Serial.begin(38400);
  Serial.println("Lectrobox / Delson Designs rotary monitor running");
}

void loop() {
  for (int i = 0; i < NUM_PINS; i++) {
    if (pins[i].poll()) {
      pins[i].print_debug_info();
    }
  }

  for (int i = 0; i < NUM_ROTORS; i++) {
    int motion = rotors[i].poll();
    if (motion) {
      Serial.print("Rotor motion: ");
      Serial.println(motion);
    }
  }
}

#endif


#ifdef BUTTON_SHIELD

PinMonitor pins[] = {
  PinMonitor(2),
  PinMonitor(3),
  PinMonitor(4),
  PinMonitor(5),
  PinMonitor(6),
  PinMonitor(7),  
};

#define NUM_PINS (sizeof(pins) / sizeof(pins[0]))

void setup() {
  Serial.begin(38400);
  Serial.println("Lectrobox / Delson Designs button monitor running");
}

void loop() {
  for (int i = 0; i < NUM_PINS; i++) {
    if (pins[i].poll()) {
      pins[i].print_debug_info();
    }
  }
}

#endif
