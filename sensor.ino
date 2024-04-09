/*
Title   : Sensor Code on ESP32
Made by : Bonifasius Raditya Pandu Hendrianto
Date    : 9 April 2024
Ver: 1.0

Description
Sensor yang dipake dibagi menjadi dua jenis, yaitu:
A. Sensor Analog -> TDS Sensor (SEN0244 DF ROBOT), PH Sensor (PH-4502C)
B. Sensor Digital -> Sensor Suhu (DHT22), Sensor Suhu Air (DS18B20), Sensor Ultrasonik (HC-SR04)

Kedua sensor suhu menggunakan input digital karena sensor DHT22 dan DS18B20 bersifat intermuka atau 
OneWire yang sistem tegangannya 1 jalur yang ngebuat sensor ini bisa konversi menjadi analog sendiri 
didalam perangkat tersebut sehingga tidak terjadi wire traffic

Sensor-sensor diatas akan menghasilkan output berupa angka sesuai perhitungan yang akan ditampilkan
dalam LCD dan serial monitor.

Install library:
1. OneWire
2. DallasTemperature
*/

#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define DHTTYPE DHT22 
#define DHTPIN 4
#define ONE_WIRE_BUS 5 
#define phSensorPin 34
#define tdsSensorPin 18
/*
don't know why it doesn't work on ESP32. 
Suprisingly can work on Arduino IDE
*/

int trig = 0, echo = 2;
double duration, ultrasonic, tdsValue, phValue;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor;

//deklarasi pin sensor pada ESP32
void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.init();					
  lcd.backlight();

  //deklarasi sensor HC-SR04
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  
  //deklarasi sensor DS20B18
  sensors.begin();
  sensors.setResolution(sensor, 12); //menggunakan 12 bit untuk DS18B20 (presisinya lebih baik)
  if (!sensors.getAddress(sensor, 0)) //menampilkan pada serial monitor
    Serial.println("Tidak dapat menemukan sensor DS18B20");
}

//main function untuk sensor
void loop()
{
  SR04();
  water_temperature();
  ds_temperature();
  tds_sensor();
  ph_sensor();
}

void SR04()
{
  digitalWrite(trig, HIGH);
  delay(500);
  digitalWrite(trig, LOW);
  
  duration = pulseIn(echo, HIGH);

  //menghitung penangkapan ultrasonik dan diubah menjadi panjang (cm)
  ultrasonic = (duration/2) * 0.034391;
  lcd.setCursor(0, 0);
  lcd.print(ultrasonic, 1);
  lcd.print(" cm");
}

void water_temperature()
{
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature))
  {
    lcd.setCursor(0, 1);
    lcd.print("DHT22 error");
    return;
  }
  lcd.setCursor(0, 1);
  lcd.print(temperature);
  lcd.print(" C");
}

void ds_temperature()
{
  sensors.requestTemperatures();
  float suhu = sensors.getTempC(sensor);
  lcd.setCursor(6, 1);
  lcd.print(suhu, 4); //4 angka di belakang koma
  lcd.print(" C");
  delay(500);
}

void tds_sensor()
{
  int sensorValue = analogRead(tdsSensorPin);

  //konversi nilai input analog yang diterima menjadi nilai TDS (ppm)
  tdsValue = map(sensorValue, 0, 1023, 0, 1000); //range nilai ppm 0-1000

  //output di serial monitor dulu. Belum ditambahin lagi LCD nya
  Serial.print("TDS Value: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");
  
  delay(500);
}

void ph_sensor()
{
  int sensorValue = analogRead(phSensorPin);

  //konversi nilai input analog menjadi nilai pH (pH)
  phValue = map(sensorValue, 0, 4095, 0, 14); //range nilai pH 0-14
  
  //output di serial monitor dulu. Belum ditambahin lagi LCD nya
  Serial.print("pH Value: ");
  Serial.println(phValue);
  
  delay(500);
}

  
