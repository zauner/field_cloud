
#define MAX_SPEED 10
#define RESPAWN_PROBABILITY 998
#define MIN_LIFETIME 3000
#define MAX_LIFETIME 10000

#define NUM_HOTSPOTS 2

unsigned long currT = 0;
unsigned long lastUpdate = 0;

typedef struct coil {
  float x;
  float y;
  bool on;
} Coil;

class HotSpot {

  public: 
  
  float x;
  float y;
  float vx;
  float vy;
  float r;
  unsigned long createdAt;
  int lifetime;

  HotSpot(float initX, float initY, float r, float velX, float velY, int l);
  long age();
  bool alive();
  bool moving();
  bool testAgainstCoil(Coil*);
  void animate(long);
  
};

HotSpot::HotSpot(float initX, float initY, float r, float velX, float velY, int l) {
  this->x = initX;
  this->y = initY;
  this->vx = velX;
  this->vy = velY;
  this->r = r;
  this->lifetime = l;
  this->createdAt = currT;
}

long HotSpot::age() {
  return currT - this->createdAt;
}

bool HotSpot::alive() {
  return this->age()<this->lifetime &&
    this->x > 0 && this->x < 20000 &&
    this->y > 0 && this->y < 20000;
}

bool HotSpot::moving() {
  return this->age()<(this->lifetime/3);
}

bool HotSpot::testAgainstCoil(Coil *c) {
  unsigned long dist = pow(c->x - this->x, 2) + pow(c->y - this->y, 2);
  if (dist < pow(this->r, 2)) {
    c->on = HIGH;
    return true;
  }
  return false;
}

void HotSpot::animate(long dT) {
  this->x += (dT * this->vx) / 1000.0;
  this->y += (dT * this->vy) / 1000.0;
}



HotSpot hotSpots[NUM_HOTSPOTS] = {
  HotSpot(0.0, 0.0, 0.0, 0.0, 0.0, 0),
  HotSpot(0.0, 0.0, 0.0, 0.0, 0.0, 0),
};

Coil pinState[16];

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  pinState[0] = {175, 1322, false};
  pinState[1] = {507, 1369, false};
  pinState[2] = {829, 1389, false};
  pinState[3] = {1136, 1362, false};
  pinState[4] = {1372, 1318, false};
  pinState[5] = {171, 827, false};
  pinState[6] = {560, 894, false};
  pinState[7] = {1002, 880, false};
  pinState[8] = {1309, 820, false};
  pinState[9] = {190, 566, false};
  pinState[10] = {492, 505, false};
  pinState[11] = {963, 539, false};
  pinState[12] = {1309, 579, false};
  pinState[13] = {209, 285, false};
  pinState[14] = {776, 244, false};
  pinState[15] = {1276, 291, false};
}

void loop() {
  long dT = millis() - currT;
  currT = millis();
  spawn();
  animate(dT);
  testHotSpots();
  writePins();

  if (currT - lastUpdate > 100) {
    printPinState();
    lastUpdate = currT;
  }
  
}

void spawn() {
  for (int i=0; i<NUM_HOTSPOTS; i++) {
    if (!hotSpots[i].alive()) {
      if (random(0, 1000)>RESPAWN_PROBABILITY) {
        hotSpots[i] = HotSpot((float)random(0, 1600), (float)random(0, 1600), 400.0, (float)random(-MAX_SPEED, MAX_SPEED), (float)random(-MAX_SPEED, MAX_SPEED), random(3000, 10000));
      }
    }
  }
}

void animate(int dT) {
  for (int i=0; i<NUM_HOTSPOTS; i++) {
    if (hotSpots[i].alive()) {
      hotSpots[i].animate(dT);
    }
  }
}

void testHotSpots() {
  for (int i=0; i<16; i++) {
    pinState[i].on = LOW;
  }
  int coilsOn = 0;
  for (int i=0; i<NUM_HOTSPOTS; i++) {
    if (hotSpots[i].alive()) {
      for (int j=0; j<16; j++) {
        if (hotSpots[i].testAgainstCoil(&(pinState[j])))
          coilsOn++;
        if (coilsOn>=5)
          return;
      }
    }
  }
}

void writePins() {
  for (int i=0; i<16; i++) {
    digitalWrite(i + 31, pinState[i].on);
  }
}

void printPinState() {
  String package = "";
  for (int i=0; i<16; i++) {
    Serial.print(pinState[i].on);
  }
  Serial.print("v");
}
