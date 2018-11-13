
long currT = 0;
long dT = 0;

void setup() {
  for (int i=0; i<16; i++) {
    pinMode(i+31, OUTPUT);
  }
  digitalWrite(31, HIGH);
}

void loop() {
  dT = millis();
  if (dT - currT > 2000) {
    for (int i=0; i<16; i++) {
      digitalWrite(i+31, LOW);
    }
    for (int i=0; i<4; i++) {
       int pin = random(0, 15);
       digitalWrite(pin+31, HIGH);
    }
  }

  
  currT = millis();
}
