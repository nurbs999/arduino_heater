#include <LiquidCrystal.h>
#include <math.h>

#define runs 20
#define runDelay 10
#define pwmPin 5
#define voltagePin 0
#define heaterTemperaturePin 1
#define lipoTemperaturePin 2

float hystereseHeater = 0.5;
float heaterTempMax = 40.0;

bool warmUp = true;
bool heating = false;
bool cooling = true;
bool lipoSaveMode = false;

const float referenceVoltage = 506.4;
const float lipoTempMax = 35.0;
const float lipoTempMin = 32.0;
const float critVin = 10.9;

const int heatingPower[3] = { 255, 223, 159 };

//Initialize LCD
LiquidCrystal lcd( 12, 11, 6, 4, 3, 2 );

void setup() {
  //analogReference(INTERNAL);
  pinMode(0, INPUT); //Voltage
  pinMode(1, INPUT); //Temperature
  pinMode(pwmPin, OUTPUT); //PWM for MOSFET

  lcd.begin( 20, 4);
  lcd.print("booting");
}

void loop() {
  delay(800);
  float tempHeater = readTemperature(heaterTemperaturePin);
  float tempLipo = readTemperature(lipoTemperaturePin);
  float LiPo = getBatteryVoltage();

  if (LiPo < critVin) {
    if (!lipoSaveMode) lcd.clear();
    lipoSaveMode = true;
    analogWrite(pwmPin, 0);
    lcd.setCursor(0, 2);
    lcd.print("Vcc low");
    lcd.setCursor(0, 3);
    lcd.print("PANIC! LiPo burning!");
  }

  if (!lipoSaveMode) {
    int heaterPower = getHeaterPower(tempHeater);
    if (tempLipo > lipoTempMax) heaterTempMax = 36.0;
    if (tempLipo < lipoTempMin) heaterTempMax = 40.0;

    if (tempHeater < heaterTempMax && !heating && !cooling) {
      heating = true;
      cooling = false;
    }
    if (tempHeater >= heaterTempMax) {
      heating = false;
      cooling = true;

      if (warmUp) {
        warmUp = false;
        lcd.setCursor(0, 2);
        lcd.print("      ");
      }
    }
    if (cooling && tempHeater < (heaterTempMax - hystereseHeater)) {
      cooling = false;
    }

    if (heating) {
      lcd.setCursor(0, 3);
      lcd.print("heating");
    } else {
      heaterPower = 0;
      lcd.setCursor(0, 3);
      lcd.print("       ");
    }

    if (warmUp) {
      lcd.setCursor(0, 2);
      lcd.print("warmup");
    }
    analogWrite(pwmPin, heaterPower);
    printHeaterPower(heaterPower);
  }
  printRuntime();
  printTemperature(tempHeater, 0);
  printTemperature(tempLipo, 1);
  printLiPoVoltage(LiPo);
  printMaxTemp(heaterTempMax);
}

void printRuntime() {
  int total_minutes = millis()/60000;
  int hours = (int)(total_minutes/60);
  int minutes = total_minutes - hours*60;

  lcd.setCursor(11, 1);
  lcd.print("Time:");
  lcd.print(hours);
  if (minutes < 10) lcd.print(":0");
  else lcd.print(":");
  lcd.print(minutes);
}

float readTemperature(int temperaturePin) {
  // read multiple values and sort them to take the mode
  int sortedValues[runs];
  for( int i = 0; i < runs; i++) {
    int value = analogRead(temperaturePin);
    int j;

    if( value < sortedValues[0] || i == 0) {
      j = 0; //insert at first position
    } else {
      for( j = 1; j < i; j++) {
        if( sortedValues[j-1] <= value && sortedValues[j] >= value) {
          // j is insert position
          break;
        }
      }
    }

    for( int k = i; k > j; k--) {
      // move all values higher than current reading up one position
      sortedValues[k] = sortedValues[k-1];
    }
    sortedValues[j] = value; //insert current reading
  }
  
  //return scaled mode of 10 values
  float returnval = 0;
  for( int i = runs/2-5; i < (runs/2+5); i++) {
    returnval +=sortedValues[i];
  }
  returnval = returnval/10;
  
  return returnval*referenceVoltage/1023;
}

void printTemperature(float temp, int line) {
  lcd.setCursor(0, line);
  lcd.print("T");
  lcd.print(line+1);
  lcd.print(":");
  lcd.print(temp);
  lcd.setCursor(7, line);
  lcd.print((char)223);
  lcd.print("C");
}

float getBatteryVoltage() {
  float rawVal = analogRead(voltagePin);
  //4.16 RefV = 1024/4.16 = 246.15
  float vBat = rawVal/222*3;
  return vBat;
}

void printLiPoVoltage(float voltage) {
  lcd.setCursor(14, 0);
  lcd.print(voltage);
  lcd.print("V");
}

void printMaxTemp(float temp) {
  lcd.setCursor(11, 2);
  lcd.print("Tmax:");
  lcd.print((int)temp);
  lcd.setCursor(18, 2);
  lcd.print((char)223);
  lcd.print("C");
}

int getHeaterPower(float temperatur) {
  if (warmUp) return heatingPower[0];

  float tempDelta = heaterTempMax - temperatur;
  if (tempDelta > 6.0) return heatingPower[0];
  else if (tempDelta > 1.5) return heatingPower[1];
  else return heatingPower[2];
}

void printHeaterPower(int heaterPower) {
  lcd.setCursor(8, 3);
  lcd.print("    ");
  lcd.setCursor(8, 3);
  if (heaterPower == heatingPower[0]) lcd.print("max");
  if (heaterPower == heatingPower[1]) lcd.print("med");
  if (heaterPower == heatingPower[2]) lcd.print("low");
  if (heaterPower == 0) lcd.print("off");
}
