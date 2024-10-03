//Daniel Andres Urrego Orjuela 20211673121
//Giovanni Enrique Tabima Galeano 20221673029

#include "I2C.h"
#include "ThisThread.h"
#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h" 
#include <cstring>

#define INTERVALO_MUESTREO   1s
#define SENSOR_TMP102 0x90

//Pines y puertos
BufferedSerial pc(USBTX, USBRX);
I2C i2c_bus(D14, D15);
Adafruit_SSD1306_I2c display(i2c_bus, D0);
AnalogIn sensor_pot(A0);

// Variables globales
float voltaje = 0.0;
int valor_entero = 0;
int valor_decimal = 0;
char buffer[40];

char comando_tmp102[3] = {0x01, 0x60, 0xA0};
char datos_temperatura[2];
const char *inicio = "Programa Iniciado\n\r";

// Función para inicializar la pantalla OLED
void inicializar_display() {
    display.begin();
    display.setTextSize(1);
    display.setTextColor(1);
    display.clearDisplay();
    display.display();
}

// Función para calcular el voltaje desde el potenciómetro
void obtener_voltaje(float lectura, int &entero, int &decimal) {
    float voltaje_calculado = lectura * 3.3;
    entero = int(voltaje_calculado);
    decimal = int((voltaje_calculado - entero) * 10000);
}

// Función para mostrar el valor en la OLED y enviarlo por serial
void mostrar_lectura(const char* tipo, int entero, int decimal) {
    sprintf(buffer, "%s:\n\r %01d.%04d\n\r", tipo, entero, decimal);
    display.clearDisplay();
    display.setTextCursor(0, 2);
    display.printf(buffer);
    display.display();
    pc.write(buffer, strlen(buffer));
}

// Función para obtener la temperatura del TMP102
void obtener_temperatura(int &entero, int &decimal) {
    comando_tmp102[0] = 0;
    i2c_bus.write(SENSOR_TMP102, comando_tmp102, 1);
    i2c_bus.read(SENSOR_TMP102, datos_temperatura, 2);
    
    int16_t temperatura = (datos_temperatura[0] << 4) | (datos_temperatura[1] >> 4);
    float temp_celsius = temperatura * 0.0625;
    
    entero = int(temp_celsius);
    decimal = int((temp_celsius - entero) * 10000);
}

int main() {
    inicializar_display();
    pc.write(inicio, strlen(inicio));

    while (true) {
        // Obtener y mostrar voltaje
        obtener_voltaje(sensor_pot.read(), valor_entero, valor_decimal);
        mostrar_lectura("Voltaje", valor_entero, valor_decimal);

        // Obtener y mostrar temperatura
        obtener_temperatura(valor_entero, valor_decimal);
        mostrar_lectura("Temperatura", valor_entero, valor_decimal);

        // Pausa entre mediciones
        ThisThread::sleep_for(INTERVALO_MUESTREO);
    }
}

