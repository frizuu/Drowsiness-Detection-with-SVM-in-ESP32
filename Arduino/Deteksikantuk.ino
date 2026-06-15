#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h> // Perbaikan typo kapitalisasi 'Sensor'

#include "model.h"

Adafruit_MPU6050 mpu; // Ditambahkan agar objek mpu terdefinisi seperti kode dataset
Eloquent::ML::Port::SVM svm;

#define WINDOW_SIZE 50

float pitchBuf[WINDOW_SIZE];
float gxBuf[WINDOW_SIZE];
float gyBuf[WINDOW_SIZE];
float gzBuf[WINDOW_SIZE];

int idx = 0;

// =========================
// StandardScaler
// =========================
// Catatan: Nilai ini harus disesuaikan kembali setelah Anda melakukan 
// retraining model menggunakan dataset baru.
float scaler_mean[7] = { 5.23117957957958, 1.8463180383021816, 9.086756756756758, 1.271111111111111, -0.05232192192192191, -0.013032432432432423, -0.02836516516516515 };
float scaler_scale[7] = { 2.7120404808987377, 1.5071142519034542, 3.9430732580831545, 4.269393655878493, 0.17108874091294576, 0.032319280392271464, 0.040121727539682475 };


float meanArray(float *arr, int n) {
  float sum = 0;
  for (int i = 0; i < n; i++) sum += arr[i];
  return sum / n;
}

float stdArray(float *arr, int n) {
  float mean = meanArray(arr, n);
  float sum = 0;
  for (int i = 0; i < n; i++) {
    sum += pow(arr[i] - mean, 2);
  }
  return sqrt(sum / n);
}

float maxArray(float *arr, int n) {
  float mx = arr[0];
  for (int i = 1; i < n; i++) {
    if (arr[i] > mx) mx = arr[i];
  }
  return mx;
}

float minArray(float *arr, int n) {
  float mn = arr[0];
  for (int i = 1; i < n; i++) {
    if (arr[i] < mn) mn = arr[i];
  }
  return mn;
}


void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Memperbaiki logika pengecekan mpu.begin() yang terbalik di kode lama Anda
  if (!mpu.begin()) {
    Serial.println("MPU6050 tidak ditemukan");
    while (1);
  }
  
  Serial.println("MPU6050 Connected & Ready");
}


void loop() {
  // Menggunakan struct event bawaan Adafruit Sensor sesuai kode dataset
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Mengambil nilai akselerasi (m/s²)
  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  // Mengambil nilai gyroscope (rad/s) sesuai format dataset Anda
  float gx = g.gyro.x;
  float gy = g.gyro.y;
  float gz = g.gyro.z;

  // Kalkulasi pitch menggunakan nilai m/s² 
  // (skala metrik saling meniadakan dalam fungsi atan2, jadi hasilnya tetap sama)
  float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

  // Memasukkan data ke buffer pembacaan
  pitchBuf[idx] = pitch;
  gxBuf[idx] = gx; // Langsung rad/s, tidak perlu dibagi 131.0 lagi
  gyBuf[idx] = gy;
  gzBuf[idx] = gz;

  idx++;

  if (idx >= WINDOW_SIZE) {

    float mean_pitch = meanArray(pitchBuf, WINDOW_SIZE);
    float std_pitch  = stdArray(pitchBuf, WINDOW_SIZE);
    float max_pitch  = maxArray(pitchBuf, WINDOW_SIZE);
    float min_pitch  = minArray(pitchBuf, WINDOW_SIZE);
    float mean_gx    = meanArray(gxBuf, WINDOW_SIZE);
    float mean_gy    = meanArray(gyBuf, WINDOW_SIZE);
    float mean_gz    = meanArray(gzBuf, WINDOW_SIZE);

    float features[7] = {
      mean_pitch,
      std_pitch,
      max_pitch,
      min_pitch,
      mean_gx,
      mean_gy,
      mean_gz
    };

    // =========================
    // StandardScaler
    // =========================
    for (int i = 0; i < 7; i++) {
      features[i] = (features[i] - scaler_mean[i]) / scaler_scale[i];
    }

    int prediction = svm.predict(features);

    Serial.println("==========");
    Serial.print("mean_pitch = "); Serial.println(mean_pitch);
    Serial.print("std_pitch = ");  Serial.println(std_pitch);
    Serial.print("max_pitch = ");  Serial.println(max_pitch);
    Serial.print("min_pitch = ");  Serial.println(min_pitch);
    Serial.print("mean_gx = ");    Serial.println(mean_gx);
    Serial.print("mean_gy = ");    Serial.println(mean_gy);
    Serial.print("mean_gz = ");    Serial.println(mean_gz);
    Serial.print("Prediction = ");

    if (prediction == 0)
      Serial.println("NORMAL");
    else
      Serial.println("FALL");

    idx = 0;
  }

  delay(50); // Diubah ke 50ms (20 Hz) agar sesuai dengan interval dataset
}