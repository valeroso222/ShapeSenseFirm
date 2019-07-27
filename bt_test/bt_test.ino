#define SW_PIN 8
#define serial Serial1

void setup() {
  serial.begin(115200);
  //serial.begin(9600);
  pinMode(SW_PIN, INPUT_PULLUP);
  serial.println("start");
}

void loop() {
  if (digitalRead(SW_PIN) == HIGH) {
    serial.println("off");
  } else {
    serial.println("on");    
  }
  delay(100);
}
