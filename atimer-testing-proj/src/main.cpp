#include <Arduino.h>

#include "atimer.h"

ATimer timer(1000);

void setup() {
  Serial.begin(115200);
	Serial.println("Starting \"atimer-testing-proj\"");

	pinMode(LED_BUILTIN, OUTPUT);

  timer.setMode(ATimerMode::REPEAT);
  timer.start();
}

bool ledState = false;
void loop() {
  timer.tick();
  if (timer.isReadyConsume()) {
    Serial.print("[");
    Serial.print(millis());
    Serial.println("] Trigger!");

    digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
    ledState = !ledState;
  }
}