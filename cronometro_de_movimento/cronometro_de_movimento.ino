/***********************************************
 * Cronometro de movimento v1.0
 * Autor: Gabriel Takaoka Nishimura
 * Data: 16/09/2015
 * *********************************************/
int sensor1 = 7, sensor2 = 8;   // pinos de cada sensor
int estado = 0;
long t1, t2;

void setup() {
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  Serial.begin(9600);
}
 
void loop(){
  if (estado == 0 && digitalRead(sensor1) == HIGH) {
      t1 = millis();
      estado = 1;
      Serial.print("t1: ");
      Serial.println((float)t1);
  } else if (estado == 1 && digitalRead(sensor2) == HIGH) {
      t2 = millis();
      estado = 2;
      Serial.print("t2: ");
      Serial.println((float)t2);
  } else {
      Serial.print("Demorou: ");
      Serial.print((float)(t2-t1) / (float)1000);
  }
}
