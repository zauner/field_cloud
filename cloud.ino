
long currT = 0;
long lastUpdate = 0;

void setup() {
  for (int i=0; i<16; i++) {
    pinMode(i+31, OUTPUT);
  }
  digitalWrite(31, HIGH);
}

void loop() {
  currT = millis();
  if (currT - lastUpdate > 2000) {
    for (int i=0; i<16; i++) {
      digitalWrite(i+31, LOW);
    }
    for (int i=0; i<4; i++) {
       int pin = random(0, 15);
       digitalWrite(pin+31, HIGH);
    }
    lastUpdate = currT;
  }

  
}
