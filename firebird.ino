//most stable 22s

// Encoders
#define ENCL 2
#define ENCR 3

// Motors
#define MOTLF 6
#define MOTLR 7
#define MOTRF 4
#define MOTRR 5
#define MOTPWML 10
#define MOTPWMR 9

// IRs A0=14,A1=15,A2=16,A3=17,A4=18
#define IR1 14
#define IR2 15
#define IR3 16
#define IR4 17
#define IR5 18

// LED
#define LED 13

//int whiteLevels[5] = {1023, 1023, 1023, 1023, 1023};
//int blackLevels[5] = {0, 0, 0, 0, 0};

// Preset
int whiteLevels[5] = {70, 70, 70, 70, 70};
int blackLevels[5] = {900, 900, 900, 900, 900};

// PID
int setpoint = 2000;


// 20s
int baseSpeed = 100;
double Kp = 0.062;
double Ki = 0.0015;
double Kd = 0.0039;
//*/

/* 21s
int baseSpeed = 90;
double Kp = 0.058;
double Ki = 0.0015;
double Kd = 0.0038;
//*/

/* 22s
int baseSpeed = 80;
double Kp = 0.055;
double Ki = 0.0015;
double Kd = 0.0035;
//*/

double proportional = 0;
double integral = 0;
double differential = 0;
int lastError = 0;
unsigned long lastPidLoop = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Init pin");
  pinMode(ENCL, INPUT);
  pinMode(ENCR, INPUT);
  pinMode(MOTRF, OUTPUT);
  digitalWrite(MOTRF, HIGH);
  pinMode(MOTLF, OUTPUT);
  digitalWrite(MOTLF, HIGH);
  pinMode(MOTRR, OUTPUT);
  digitalWrite(MOTRR, LOW);
  pinMode(MOTLR, OUTPUT);
  digitalWrite(MOTLR, LOW);
  pinMode(MOTPWML, OUTPUT);
  digitalWrite(MOTPWML, LOW);
  pinMode(MOTPWMR, OUTPUT);
  digitalWrite(MOTPWMR, LOW);
  pinMode(IR1, INPUT_PULLUP);
  pinMode(IR2, INPUT_PULLUP);
  pinMode(IR3, INPUT_PULLUP);
  pinMode(IR4, INPUT_PULLUP);
  pinMode(IR5, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.println("Levels calibration");
  digitalWrite(LED, HIGH);
  for (int i = 0; i < 400; i++) {
    for (int j = 0; j < 5; j++) {
      int inp = analogRead(14 + j);
      if (inp < whiteLevels[j]) {
        whiteLevels[j] = inp;
        Serial.println("whiteLevels[" + String(j) + "]=" + String(inp));
      }
      if (inp > blackLevels[j]) {
        blackLevels[j] = inp;
        Serial.println("blackLevels[" + String(j) + "]=" + String(inp));
      }
    }
    delay(2);
  }
  Serial.println("whiteLevels");
  for (int i = 0; i < 5; i++) {
    Serial.print(String(whiteLevels[i]) + " ");
  }
  Serial.println();
  Serial.println("blackLevels");
  for (int i = 0; i < 5; i++) {
    Serial.print(String(blackLevels[i]) + " ");
  }
  digitalWrite(LED, LOW);
}



void loop() {
  //Serial.println("New loop");
  int reading[5] = {0};
  for (int i = 0; i < 5; i++) {
    reading[i] = analogRead(14 + i);
    reading[i] = map(reading[i], whiteLevels[i], blackLevels[i], 0, 1000);
    //Serial.println("reading[" + String(i) + "]=" + String(reading[i]));
  }

  int lineLocation = -1;
  int maxValueInReading = 0;
  for (int i = 0; i < 5; i++) {
    if (reading[i] > maxValueInReading) {
      maxValueInReading = reading[i];
      lineLocation = i * 1000;
    }

  }
  if (maxValueInReading < 500) {
    // No lines detected
    Serial.println("No lines");
    //return;
  }else{Serial.println(lineLocation);}
  int err = lineLocation - setpoint;
  proportional = Kp * err;
  integral += Ki * err * (micros() - lastPidLoop) / 1000000;
  differential = Kd * (err - lastError) / (micros() - lastPidLoop) * 1000000;
  lastPidLoop = micros();
  lastError = err;
  if (integral > 255) integral = 255;
  if (integral < -255) integral = -255;
  double outp = proportional + integral + differential;
  Serial.print(F(" ")); 
  Serial.print(proportional);
  Serial.print(F(" "));
  Serial.print(integral);
  Serial.print(F(" "));
  Serial.print(differential);
  Serial.print(F(" "));
  Serial.println(outp);
  
  if (outp > baseSpeed) outp = baseSpeed;
  if (outp < -baseSpeed) outp =-baseSpeed;
  analogWrite(MOTPWML,baseSpeed+outp);
  analogWrite(MOTPWMR,baseSpeed-outp);

}
