
#define MAX_SPEED 300
#define RESPAWN_PROBABILITY 998 // of 1000
#define MOVE_PROBABILITY 50     // of 100
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
  pinState[0] = {223, 1081, false};
  pinState[1] = {416, 1356, false};
  pinState[2] = {853, 1349, false};
  pinState[3] = {502, 720, false};
  pinState[4] = {641, 1071, false};
  pinState[5] = {1228, 1349, false};
  pinState[6] = {1083, 572, false};
  pinState[7] = {199, 412, false};
  pinState[8] = {1271, 1490, false};
  pinState[9] = {805, 177, false};
  pinState[10] = {1357, 1081, false};
  pinState[11] = {531, 184, false};
  pinState[12] = {214, 773, false};
  pinState[13] = {1146, 177, false};
  pinState[14] = {1357, 184, false};
  pinState[15] = {204, 171, false};

  for (int i=0; i<16; i++) {
    pinMode(i+31, OUTPUT);
  }
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
        int be_moving = random(0, 100)>MOVE_PROBABILITY;
        hotSpots[i] = HotSpot((float)random(0, 1600), (float)random(0, 1600), 400.0, (float)random(-MAX_SPEED, MAX_SPEED) * be_moving, (float)random(-MAX_SPEED, MAX_SPEED) * be_moving, random(3000, 10000));
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
