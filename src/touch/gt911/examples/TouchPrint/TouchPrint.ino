#include "TAMC_GT911.h"

#define TOUCH_SDA 7
#define TOUCH_SCL 8
#define TOUCH_RST 22
#define TOUCH_INT -1

#define TOUCH_WIDTH  480
#define TOUCH_HEIGHT 800

TAMC_GT911 *tp = nullptr;

void setup() {
  Serial.begin(115200);
  Serial.println("TAMC_GT911 Example: Ready");
  tp = new TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);
  tp->begin();
  tp->setRotation(ROTATION_NORMAL);
  Serial.println("Acabou setup");
}

void loop() {
  //Serial.println("loop");
  tp->read();
  if (tp->isTouched){
    for (int i=0; i<tp->touches; i++){
      Serial.print("Touch ");Serial.print(i+1);Serial.print(": ");;
      Serial.print("  x: ");Serial.print(tp->points[i].x);
      Serial.print("  y: ");Serial.print(tp->points[i].y);
      Serial.print("  size: ");Serial.println(tp->points[i].size);
      Serial.println(' ');
    }
  }
  delay(10);
}
