#include <shutter.h>
#include <ShutterSystem.h>
#include <IRremote.h>
#include <NewTone.h>

const uint8_t PIN_STEP1 = 22;
const uint8_t PIN_DIR1 = 23;
const uint8_t PIN_STEP2 = 24;
const uint8_t PIN_DIR2 = 25;
const uint8_t PIN_STEP3 = 26;
const uint8_t PIN_DIR3 = 27;
const uint8_t PIN_STEP4 = 28;
const uint8_t PIN_DIR4 = 29;

const uint8_t PIN_UP1 = 32;
const uint8_t PIN_DOWN1 = 33;
const uint8_t PIN_UP2 = 34;
const uint8_t PIN_DOWN2 = 35;
const uint8_t PIN_UP3 = 36;
const uint8_t PIN_DOWN3 = 37;
const uint8_t PIN_UP4 = 38;
const uint8_t PIN_DOWN4 = 39;

const uint8_t PHOTOCELL_PIN = A0;
const short unsigned int DEFAULT_LIGHT_THRESHOLD = 800;
short int light_threshold;
short int current_light_level;

const uint8_t BUZZER_PIN = 2;
const uint8_t RELAY_PIN = 4;

const uint8_t IR_RECEIVER_PIN = 3;
IRrecv ir_receiver(IR_RECEIVER_PIN);
decode_results results;
unsigned long command = 0;

const uint8_t LED_PIN = 13;

float SHUTTER_SPEED = 4000;
unsigned long MAX_TIME_TO_RUN = 160000;
float current_speed = 0;
shutter shutter1(PIN_UP1, PIN_DOWN1, 1, MAX_TIME_TO_RUN, PIN_STEP1, PIN_DIR1);
shutter shutter2(PIN_UP2, PIN_DOWN2, 1, MAX_TIME_TO_RUN, PIN_STEP2, PIN_DIR2);
shutter shutter3(PIN_UP3, PIN_DOWN3, 1, MAX_TIME_TO_RUN, PIN_STEP3, PIN_DIR3);
shutter shutter4(PIN_UP4, PIN_DOWN4, 1, MAX_TIME_TO_RUN, PIN_STEP4, PIN_DIR4);
shutter* shutter_p = &shutter1;
ShutterSystem shutters;

enum pos {up = 1, down = 2, unknown = 0};

struct ir_codes {
  enum {start_pause = 16761405, toggle_manual = 16748655, roll_down = 16712445, roll_up = 16720605, reset = 16736925, dec_threshold = 16753245, inc_threshold = 16769565,
        shutter1_code = 16724175, shutter2_code = 16718055, shutter3_code = 16743045, shutter4_code = 16716015, all_shutters_code = 16738455, show_information = 16750695
       };
};

struct serial_codes {
  enum {start_pause = 'f', toggle_manual = 'i', roll_down = 'e', roll_up = 'd', reset = 'b', dec_threshold = 'a', inc_threshold = 'c',
        shutter1_code = 'm', shutter2_code = 'n', shutter3_code = 'o', shutter4_code = 'p', all_shutters_code = 'j', show_information = 'k', send_to_chrome = 'z'
       };
};

bool manual_mode = false;
bool all_shutters = true;

long last_check = 0;
const long TIME_INT = 180000;

void send_data() {
  short int light_level = analogRead(PHOTOCELL_PIN);
  uint8_t mode = manual_mode ? 1 : 0;
  uint8_t condition = light_level < light_threshold ? 0 : 1; //dark / sunny

  Serial.print(999);
  Serial.print(",");
  Serial.print(mode);
  Serial.print(",");
  Serial.print(light_level);
  Serial.print(",");
  Serial.print(light_threshold);
  Serial.print(",");
  Serial.print(condition);
  Serial.print(",");
  shutters.systemStatusRawData();
  Serial.println();
}

void correct_command_exec (bool send_data_ = true, bool play_sound = true) {
  if (play_sound)
    NewTone(BUZZER_PIN, 600, 100);
  if (send_data_)
    send_data();
}

void toggle_manual_mode() {
  manual_mode = true;
  digitalWrite(LED_PIN, HIGH);
  correct_command_exec();
}

void move(short int direction) {
  light_threshold = analogRead(PHOTOCELL_PIN) + 5 * direction;
  if (all_shutters) {
    shutters.setSpeed(SHUTTER_SPEED * direction);
  }
  else {
    shutter_p->setSpeed(SHUTTER_SPEED * direction);
  }
  toggle_manual_mode();
}

void shutter_selection_routine(shutter& shutter_) {
  toggle_manual_mode();
  all_shutters = false;
  shutter_p = &shutter_;
}

void readIrCode() {
  if (ir_receiver.decode(&results)) {
    command = results.value;
    ir_receiver.resume();
  }
}

void readSerial() {
  if (Serial.available())
    command = Serial.read();
}

void fatal_error() {
  Serial.println("!!! FATAL ERROR HAS OCCURED !!!");
  while (true) {
    NewTone(BUZZER_PIN, 600, 100);
    digitalRead(LED_PIN) == HIGH ? digitalWrite(LED_PIN, LOW) : digitalWrite (LED_PIN, HIGH);
    delay(500);
  }
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  Serial.begin(9600);
  ir_receiver.enableIRIn();
  light_threshold = DEFAULT_LIGHT_THRESHOLD;

  shutters.addShutter(shutter1);
  shutters.addShutter(shutter2);
  shutters.addShutter(shutter3);
  shutters.addShutter(shutter4);

  shutters.setMaxSpeed(SHUTTER_SPEED);
}

void loop() {

  readIrCode();
  readSerial();

  if (command > 0) {
    if (command == ir_codes::toggle_manual || command == serial_codes::toggle_manual) {
      manual_mode = !manual_mode;
      manual_mode ? digitalWrite(LED_PIN, HIGH) : digitalWrite(LED_PIN, LOW);
      correct_command_exec();
    }
    else if (command == ir_codes::all_shutters_code || command == serial_codes::all_shutters_code) {
      all_shutters = true;
      toggle_manual_mode();
    }
    else if (command == ir_codes::shutter1_code || command == serial_codes::shutter1_code) {
      shutter_selection_routine(shutter1);
    }
    else if (command == ir_codes::shutter2_code || command == serial_codes::shutter2_code) {
      shutter_selection_routine(shutter2);
    }
    else if (command == ir_codes::shutter3_code || command == serial_codes::shutter3_code) {
      shutter_selection_routine(shutter3);
    }
    else if (command == ir_codes::shutter4_code || command == serial_codes::shutter4_code) {
      shutter_selection_routine(shutter4);
    }
    else if (command == ir_codes::roll_down || command == serial_codes::roll_down) { //sign = -1
      move(-1);
    }
    else if (command == ir_codes::roll_up || command == serial_codes::roll_up) { //sign = 1
      move(1);
    }
    else if (command == ir_codes::start_pause || command == serial_codes::start_pause) {
      toggle_manual_mode();

      if (all_shutters && shutters.isRunning()) {
        shutters.saveSpeed();
        shutters.setSpeed(0);
      }
      else if (all_shutters) {
        shutters.retrieveSpeed();
      }
      else {
        if (shutter_p->speed() != 0) {
          current_speed = shutter_p->speed();
          shutter_p->setSpeed(0);
        }
        else {
          shutter_p->setSpeed(current_speed);
        }
      }
    }
    else if (command == ir_codes::reset || command == serial_codes::reset) {
      light_threshold = DEFAULT_LIGHT_THRESHOLD;
      correct_command_exec();
    }
    else if (command == ir_codes::inc_threshold || command == serial_codes::inc_threshold) {
      if (light_threshold <= 1019)
        light_threshold += 5;
      correct_command_exec();
    }
    else if (command == ir_codes::dec_threshold || command == serial_codes::dec_threshold) {
      if (light_threshold >= 5)
        light_threshold -= 5;
      correct_command_exec();
    }
    else if (command == ir_codes::show_information || command == serial_codes::show_information) {
      correct_command_exec(false);
      current_light_level = analogRead(PHOTOCELL_PIN);
      Serial.println(manual_mode ? "Manual Mode" : "Automatic Mode");
      if (!manual_mode)
        Serial.println("Next check in " + String((TIME_INT - (millis() - last_check)) / 1000) + " second(s)");
      Serial.println((current_light_level < light_threshold ? "Dark" : "Sunny"));
      Serial.println("Light level: " + String(current_light_level));
      Serial.println("Light treshold: " + String(light_threshold));
      shutters.systemStatus();
      Serial.println("---------------------");
    }
    else if (command == serial_codes::send_to_chrome && !shutters.isRunning()){
      correct_command_exec(true, false);
    }
    command = 0;
  }

  if (!manual_mode && (last_check == 0 || (millis() - last_check > TIME_INT))) {
    last_check = millis();
    if (shutters.shutterPositionNotEqual(up) && (analogRead(PHOTOCELL_PIN) <= light_threshold)) {
      shutters.setSpeed(SHUTTER_SPEED);
      send_data();
    }
    else if (shutters.shutterPositionNotEqual(down) && (analogRead(PHOTOCELL_PIN) > light_threshold)) {
      shutters.setSpeed(-SHUTTER_SPEED);
      send_data();
    }
  }

  if (shutters.checkEndOfRoad() == 1)
    send_data();

  if (shutters.isRunning()) {
    digitalWrite(RELAY_PIN, LOW);
  }
  else {
    digitalWrite(RELAY_PIN, HIGH);
  }
  if (shutters.isError())
    fatal_error();
  shutters.runSpeed();
}
