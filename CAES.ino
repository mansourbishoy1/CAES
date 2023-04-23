unsigned long previousMillis = 0;     // stores the last time the if statement was executed
const unsigned long interval = 1000;  // interval at which to perform the if statement (in milliseconds)
const int averageValue = 500;
long int sensorValue = 0;
float sensitivity = 1000.0 / 200.0;  //1000mA per 200mV
float Vref = 2490.23;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);    // or any other baud rate that matches the one set on your Arduino
  pinMode(A7, INPUT);    // pressure sensor
  pinMode(A5, INPUT);    //current sensor
  pinMode(A4, INPUT);    //voltage sensor
  pinMode(3, OUTPUT);    //compressor
  pinMode(4, OUTPUT);    //valve
  digitalWrite(3, LOW);  //Turning off Compressor just in case
}

void loop() {
  // Read the current value 500 times:
  for (int i = 0; i < averageValue; i++) {
    sensorValue += analogRead(A5);
    delay(0.0001);
  }

  sensorValue = sensorValue / averageValue;
  // The on-board ADC is 10-bits
  // Different power supply will lead to different reference sources
  // example: 2^10 = 1024 -> 5V / 1024 ~= 4.88mV
  //          unitValue= 5.0 / 1024.0*1000 ;
  float unitValue = 5.0 / 1024.0 * 1000;
  float voltage_curr = unitValue * sensorValue;
  // Calculate the corresponding current
  float current = (voltage_curr - Vref) * sensitivity;
  // Reset the sensorValue for the next reading
  sensorValue = 0;
  float pressure = (analogRead(A7) * 4.0 / 1023.0) + 0.5;
  float PSIreading = (pressure - 0.5) * 100.0 / 4.0;
  Serial.println(String(PSIreading));
  float input = Serial.read();
  char input1 = Serial.read();
  float voltage = (analogRead(A4) / 1023.0) * 25.0;  //input voltage
  if (voltage > 10) {
    digitalWrite(4, LOW);  //valve off
    if (millis() - previousMillis >= interval) {
      if (PSIreading <= 45) {
        digitalWrite(3, HIGH);  //compressor on
        Serial.println("compressor on");
      } else if (PSIreading > 55) {
        digitalWrite(3, LOW);  //compressor off
        Serial.println("compressor off");
      }
      previousMillis = millis();
    }
  } else {
    if (current > 10) {       //threshold current is 10mA
      digitalWrite(4, HIGH);  //valve on
      Serial.println("Valve on");
    } else {
      digitalWrite(4, LOW);  //valve off
      Serial.println("Valve off");
    }
  }
}