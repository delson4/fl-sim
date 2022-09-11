
class PinState {
  private:
    static constexpr int HOLDOFF_TICKS = 1000;
    int pin_num;
    bool last_emitted_state;
    int holdoff_ticks_remaining;
  public:
    PinState(int pin_num_arg) {
      pin_num = pin_num_arg;
      last_emitted_state = false;
      holdoff_ticks_remaining = HOLDOFF_TICKS;
      pinMode(pin_num, INPUT_PULLUP);
    }

    void poll() {
      bool is_pushed = (digitalRead(pin_num) == false);
      if (is_pushed == last_emitted_state) {
        return;
      }
      if (holdoff_ticks_remaining > 0) {
        holdoff_ticks_remaining--;
      } else {
        last_emitted_state = is_pushed;
        holdoff_ticks_remaining = HOLDOFF_TICKS;
        Serial.print("Pin ");
        Serial.print(pin_num);
        Serial.print(" is now ");
        if (last_emitted_state) {
          Serial.println("PUSHED");
        } else {
          Serial.println("RELEASED");
        }
      }
    }
};

PinState pins[] = {
  PinState(2),
  PinState(3),
  PinState(4),
  PinState(5),
  PinState(6),
  PinState(7),
};

#define NUM_PINS (sizeof(pins) / sizeof(pins[0]))

void setup() {
  Serial.begin(38400);
  Serial.println("Lectrobox / Delson Designs button monitor running");
}

void loop() {
  for (int i = 0; i < NUM_PINS; i++) {
    pins[i].poll();
  }
}
