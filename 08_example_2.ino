// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 100.0
#define _DIST_MAX 300.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time;


void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);

  // LED OFF
  analogWrite(PIN_LED, 255);
}


void loop() {
  float distance;
  int brightness;

  // wait until next sampling time
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  // read distance
  distance = USS_measure(PIN_TRIG, PIN_ECHO);


  // distance is invalid or greater than 300 mm
  if ((distance == 0.0) || (distance > _DIST_MAX)) {

    distance = _DIST_MAX + 10.0;

    // LED OFF
    analogWrite(PIN_LED, 255);
  }


  // distance is less than 100 mm
  else if (distance < _DIST_MIN) {

    distance = _DIST_MIN - 10.0;

    // LED OFF
    analogWrite(PIN_LED, 255);
  }


  // distance is between 100 and 300 mm
  else {

    // 200 mm = brightest
    // 100 mm = OFF
    // 300 mm = OFF

    if (distance <= 200.0) {

      brightness = (int)(255.0 - 
                  (distance - 100.0) * 255.0 / 100.0);

    } else {

      brightness = (int)(
                  (distance - 200.0) * 255.0 / 100.0);
    }

    // keep brightness between 0 and 255
    brightness = constrain(brightness, 0, 255);

    analogWrite(PIN_LED, brightness);
  }


  // output distance to Serial Plotter
  Serial.print("Min:");
  Serial.print(_DIST_MIN);

  Serial.print(",distance:");
  Serial.print(distance);

  Serial.print(",Max:");
  Serial.print(_DIST_MAX);

  Serial.println("");


  // IMPORTANT:
  // delay(50) has been removed.
  // Sampling interval = 25 ms.


  // update last sampling time
  last_sampling_time += INTERVAL;
}


// get a distance reading from USS
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);

  delayMicroseconds(PULSE_DURATION);

  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
