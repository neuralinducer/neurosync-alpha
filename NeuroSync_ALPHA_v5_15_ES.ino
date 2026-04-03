// ============================================================
// NEUROSYNC ALPHA v5.15
// ============================================================
// NeuralInducer.org
// Investigación Abierta en Neurotecnología
//
// Dispositivo de entrainment neural por luz LED pulsada.
// Induce estados de ondas cerebrales mediante estimulación
// visual a frecuencias específicas.
//
// PROTOCOLOS:
//   S — SUEÑO         | 2–4 Hz DELTA  | Rojo  | 90 min
//   A — ALFA          | 10 Hz ALFA    | Azul  | 30 min
//   C — CONCENTRACIÓN | 12 Hz BETA    | Verde | 20 min
//   E — ENERGÍA       | 6→12 Hz       | Rojo  | 15 min
//   O — ORACIÓN       | Modulado      | Rojo  | 40 min
//
// HARDWARE:
//   • Arduino Nano (ATmega328P)
//   • 24 LEDs WS2812B — 4 columnas × 6 LEDs — cableado zigzag
//   • 3 Botones táctiles capacitivos
//
// CONEXIONES:
//   • Pin D6  → DIN tira WS2812B
//   • Pin D2  → Botón ON/OFF
//   • Pin D3  → Botón MODE
//   • Pin D4  → Botón PLAY
//
// LIBRERÍA REQUERIDA:
//   Adafruit NeoPixel (instalar desde el gestor de librerías)
//
// Licencia: CC BY-NC 4.0
// Más información: neuralinducer.org
// ============================================================

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ========== CONFIGURACIÓN HARDWARE ==========
#define LED_PIN      6
#define NUM_LEDS     24
#define BUTTON_ONOFF 2
#define BUTTON_MODE  3
#define BUTTON_PLAY  4

// ========== CONFIGURACIÓN MATRIZ ==========
#define NUM_COLS    4
#define LEDS_PER_COL 6

// ========== COLORES ==========
#define COLOR_ROJO  strip.Color(255, 0, 0)
#define COLOR_VERDE strip.Color(0, 255, 0)
#define COLOR_AZUL  strip.Color(0, 0, 255)

// ========== DATOS ORACIÓN (1202 FRAMES a 29.97 fps) ==========
const uint8_t PROGMEM oracionBrillo[] = {
   10,  11,  12,  11,  12,  12,  11,  11,  12,  12,  11,  11,  11,  12,  13,  11,
   11,  11,  73, 124, 157, 182, 163, 106, 145, 101, 199, 196, 129, 110, 143, 124,
  141, 193, 194,  99,  82,  60,  23,  14,  21,  97, 106, 127, 101,  68,  39,  20,
   14,  12,  11,  11,  12,  12,  19, 116, 164, 162,  87,  67,  72,  68,  53,  15,
   12,  43, 185, 181, 120,  74,  53,  51,  52, 154, 144, 111, 106,  92,  98, 150,
  121,  64,  35,  48,  47,  42,  62, 110, 121, 144, 137, 119,  88,  77,  77,  60,
   46,  35,  26,  35,  32,  29,  23,  17,  13,  12,  11,  11,  11,  12,  12,  12,
   12,  11,  12,  12,  12,  11,  12,  11,  12,  13,  12,  13,  22,  43,  53,  53,
  204, 201, 145, 121, 135,  75,  19,  34, 134, 162,  43,  24, 132, 156,  65,  17,
   20, 101, 200, 200, 153, 168, 188, 155,  60,  64,  54,  75, 176, 170, 192, 236,
  122,  18,  12, 117, 166, 134, 127, 148, 178, 168, 159, 143, 127, 137, 129, 119,
  109,  77, 108, 111,  97, 101,  77,  54,  38,  22,  16,  11,  11,  12,  12,  11,
   11,  10,  11,  12,  12,  11,  10,  11,  11,  12,  12,  11,  13,  11,  48,  61,
   49, 119, 243, 194, 150, 146, 143, 148, 150, 111,  64, 192, 237, 126,  16,  13,
  118, 205, 187, 132, 120, 104, 106, 187, 186, 172, 176, 196, 184, 145, 148, 151,
  130, 116, 122, 127,  90,  78,  48,  32,  18,  12,  11,  12,  12,  11,  11,  11,
   11,  11,  12,  12,  11,  11,  11,  12,  12,  11,  11,  12,  11,  11,  14,  33,
   93, 147, 194, 195, 155,  70,  84, 198, 185,  70,  70,  55, 145, 179,  41,  13,
   46, 162, 121,  66,  93, 155, 156, 143, 121, 134, 149, 111, 107,  97,  72,  28,
   16, 105, 165, 188, 189, 189, 185,  88,  28,  15,  14,  11,  12,  12,  12,  12,
   12,  12,  13,  20, 128, 212, 186, 114, 129, 129, 164, 185, 189, 113, 111, 157,
  174, 154, 152,  79,  65,  98, 102,  70,  80, 147, 163, 125, 200, 171, 125, 155,
  195, 167, 134,  45,  16,  14,  12,  12,  12,  11,  11,  12,  12,  12,  11,  11,
   95, 145,  78,  50,  61,  57,  53,  43,  40, 107, 101,  70,  16,  13,  40, 171,
  149, 137, 112, 116, 107,  94,  59,  77,  95, 126, 148, 140, 125,  86,  70,  35,
   19,  38,  93,  94,  81,  90,  87,  90, 100, 113,  76,  24,  13,  15,  57,  82,
   91, 102, 100,  97,  80,  55,  41,  40,  60,  71,  52,  38,  24,  16,  11,  12,
   11,  11,  12,  11,  11,  11,  10,  10,  10,  10,  10,  12,  12,  11,  11,  12,
   12,  11,  11,  11,  11,  11,  11,  11,  12,  11,  11,  11,  11,  12,  11,  15,
   91, 140, 165, 120,  33,  17,  15,  12, 142, 180, 145, 107, 116, 111, 114, 109,
  109, 127, 130, 156, 182, 141,  64,  77,  51,  17,  14,  88, 120, 151, 143,  67,
  134, 145,  70,  15,  13,  15,  36, 122, 127, 130, 100, 133, 133,  91,  46,  93,
  133, 123, 137, 140, 114, 121,  79,  32,  13,  12,  12,  12,  12,  11,  11,  11,
   12,  12,  11,  11,  11,  11,  22,  24, 122, 221, 155,  96, 107, 167, 170,  86,
   32,  54,  78, 133, 165, 176,  99,  23,  47, 118, 129, 133, 137, 128, 107,  81,
   69,  53,  37,  27,  19,  15,  12,  12,  11,  11,  11,  11,  11,  12,  11,  11,
   11,  12,  11,  11,  10,  11,  12,  11,  11,  11,  12,  36, 103, 150,  99,  15,
   12,  56, 162, 174,  99, 123,  57,  46,  48,  50,  85, 173, 148, 192, 175, 129,
   93, 127, 166, 114,  82,  89, 118, 143, 131,  67,  46,  31,  21,  77,  94, 136,
  173,  84,  47,  22,  14,  15, 102, 143, 177, 163, 130,  93,  56,  35,  33,  83,
  179, 176, 180, 145, 140, 124,  71,  92, 146, 140, 112,  77,  44,  42,  48,  39,
   24,  12,  13,  12,  12,  12,  13,  12,  11,  11,  13,  12,  19, 148, 177, 109,
  106, 183, 176, 115,  17,  17, 186, 177, 145, 114, 107,  72, 101, 172, 207, 191,
  142, 114, 130, 123, 117, 170, 161,  90,  48,  39,  38,  35,  76, 168, 179, 126,
   22,  14,  18,  91,  69, 108,  93,  83,  59,  31,  37,  31,  22,  13,  11,  11,
   12,  12,  12,  11,  12,  12,  12,  11,  20, 172, 194, 154, 128, 112, 112,  64,
   63, 144, 182, 140,  95,  88, 135, 197, 211, 195, 154,  94,  91, 117, 127, 126,
   69,  40,  30, 110, 123,  94,  92, 100, 125, 124,  55,  34,  17,  16,  48, 118,
  115, 104,  59,  53, 117, 118,  93,  92,  65,  47, 113, 106, 100, 103,  92,  57,
   85,  78,  52,  31,  25,  24,  21,  17,  14,  13,  11,  11,  11,  11,  12,  12,
   11,  11,  11,  11,  12,  12,  12,  11,  11,  12,  12,  12,  12,  12,  12,  12,
   83, 141, 145, 148, 201, 116,  49, 179, 179,  67,  37,  79, 126, 179, 203,  91,
   18, 136, 198, 109,  62,  50, 154, 161, 125, 117,  79,  40,  23, 161, 177, 125,
   94,  75,  25,  15,  86, 168, 138,  54,  66,  65,  68,  63,  51,  74,  97, 111,
  143, 144, 141, 121, 106,  74,  61,  50,  38,  27,  14,  11,  10,  12,  12,  12,
   11,  11,  11,  12,  12,  12,  13,  11,  46,  91, 130, 123,  64,  49,  43,  50,
  127, 125, 162, 143, 134, 107, 154, 145, 192, 138,  96, 131, 144, 137,  68,  26,
   51, 157, 120, 121,  94,  94, 108, 148, 136, 118, 109, 125, 191, 141,  73,  46,
   94, 116, 102, 117, 103,  94,  64,  37,  20,  12,  11,  12,  12,  11,  11,  11,
   11,  12,  11,  11,  11,  99, 172, 135, 125,  21,  18,  88, 239, 165,  29,  15,
   13,  13,  12,  44, 207, 201, 190, 223, 219, 117,  69, 108, 162, 159, 167, 174,
  171, 129,  65,  52, 109, 162, 157,  77,  73,  76,  70, 119, 158, 162, 160, 161,
  124, 103,  96,  96, 119, 108,  96,  59,  21,  13,  12,  12,  12,  12,  11,  13,
   36,  74, 165, 255, 177, 160, 132,  52,  15,  13,  12,  94, 145, 185,  82, 134,
  182, 179, 178, 187, 236, 194, 113,  91,  96,  72,  80,  46,  28,  14,  12,  11,
   11,  11,  11,  11,  11,  54, 110, 129,  88, 125, 152, 177, 148,  99,  45,  40,
  105, 135, 149, 162, 176, 188, 162,  74, 155, 142, 148, 149, 150, 109,  29,  13,
   13,  12,  13,  11,  12,  11,  10,  57, 187, 205, 178, 145, 134, 107, 100,  51,
   18,  13,  14,  12,  86, 188, 199, 220, 222, 225, 202, 173,  71,  79, 212, 186,
  182,  64,  36,  35,  72, 124, 163, 162,  77,  43,  47,  49,  42,  47,  43,  33,
   68, 158, 151, 138,  86,  40,  31,  54,  88,  83,  86,  78,  63,  50,  40,  21,
   25,  24,  23,  18,  14,  12,  12,  11,  10,  11,  11,  12,  11,  11,  11,  11,
   11,  12,  11,  11,  11,  11,  12,  11,  29,  78,  98,  92,  79,  65,  70,  80,
   96,  87,  75,  56,  49,  33,  25,  22,  20,  18,  14,  11,  11,  11,  11,  11,
   12,  11,  11,  11,  11,  11,  11,  11,  11,  11,  10,  10,  10
};

#define ORACION_FRAMES 1202
#define ORACION_FPS    29.97

// ========== ESTADOS ==========
enum Estado {
  MENU,
  SESION_ACTIVA,
  SESION_PAUSADA
};

enum Modalidad {
  SUENO,
  ALFA,
  CONCENTRACION,
  ENERGIA,
  ORACION
};

// ========== VARIABLES GLOBALES ==========
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

Estado    estadoActual            = MENU;
Modalidad modalidadSeleccionada   = SUENO;
bool      dispositivoEncendido    = true;

unsigned long tiempoInicioSesion  = 0;
unsigned long tiempoPausado       = 0;
unsigned long ultimaActividad     = 0;

const unsigned long TIMEOUT_INACTIVIDAD = 120000; // 2 min sin actividad → apaga LEDs

unsigned long ultimaPulsacionOnOff  = 0;
unsigned long ultimaPulsacionMode   = 0;
unsigned long ultimaPulsacionPlay   = 0;
const unsigned long DEBOUNCE_DELAY  = 450;

unsigned long ultimoPulsacionGlobal = 0;
const unsigned long ZONA_MUERTA_GLOBAL = 200;

int pasoActual = 0; // Control del efecto OLA (modo SUEÑO)

// ========== MATRICES DE LETRAS — MENÚ ==========
const bool LETRA_S[LEDS_PER_COL][NUM_COLS] = {
  {0,1,1,0},{1,0,0,0},{0,1,1,0},{0,0,0,1},{0,1,1,0},{0,0,0,0}
};
const bool LETRA_A[LEDS_PER_COL][NUM_COLS] = {
  {0,1,1,0},{1,0,0,1},{1,1,1,1},{1,0,0,1},{1,0,0,1},{0,0,0,0}
};
const bool LETRA_C[LEDS_PER_COL][NUM_COLS] = {
  {0,1,1,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{0,1,1,0},{0,0,0,0}
};
const bool LETRA_E[LEDS_PER_COL][NUM_COLS] = {
  {1,1,1,1},{1,0,0,0},{1,1,1,0},{1,0,0,0},{1,1,1,1},{0,0,0,0}
};
const bool LETRA_O[LEDS_PER_COL][NUM_COLS] = {
  {0,1,1,0},{1,0,0,1},{1,0,0,1},{1,0,0,1},{0,1,1,0},{0,0,0,0}
};

// ========== SETUP ==========
void setup() {
  strip.begin();
  strip.show();
  strip.setBrightness(255);

  pinMode(BUTTON_ONOFF, INPUT);
  pinMode(BUTTON_MODE,  INPUT);
  pinMode(BUTTON_PLAY,  INPUT);

  // Recuperar modalidad guardada en EEPROM
  byte modalidadGuardada = EEPROM.read(0);
  if (modalidadGuardada <= 4) {
    modalidadSeleccionada = (Modalidad)modalidadGuardada;
  }

  mostrarLetraModalidad(modalidadSeleccionada);
  ultimaActividad = millis();
}

// ========== LOOP PRINCIPAL ==========
void loop() {
  // Zona muerta global: bloquea todos los botones brevemente tras cada toque
  if (millis() - ultimoPulsacionGlobal < ZONA_MUERTA_GLOBAL) return;

  if (digitalRead(BUTTON_ONOFF) == HIGH &&
      (millis() - ultimaPulsacionOnOff > DEBOUNCE_DELAY)) {
    ultimaPulsacionOnOff    = millis();
    ultimoPulsacionGlobal   = millis();
    dispositivoEncendido    = !dispositivoEncendido;

    if (!dispositivoEncendido) {
      strip.clear();
      strip.show();
    } else {
      estadoActual = MENU;
      mostrarLetraModalidad(modalidadSeleccionada);
      ultimaActividad = millis();
    }
  }

  if (!dispositivoEncendido) return;

  if (estadoActual == MENU) {
    if (millis() - ultimaActividad >= TIMEOUT_INACTIVIDAD) {
      strip.clear();
      strip.show();
      return;
    }
    manejarMenu();
  } else if (estadoActual == SESION_ACTIVA) {
    ejecutarSesion();
  } else if (estadoActual == SESION_PAUSADA) {
    manejarPausa();
  }
}

// ========== UTILIDADES ==========

// Devuelve el índice real del LED en la cadena según columna y fila,
// teniendo en cuenta el cableado zigzag
int getLEDIndex(int col, int fila) {
  if (col % 2 == 0) {
    return col * LEDS_PER_COL + (LEDS_PER_COL - 1 - fila);
  } else {
    return col * LEDS_PER_COL + fila;
  }
}

void manejarMenu() {
  if (digitalRead(BUTTON_MODE) == HIGH &&
      (millis() - ultimaPulsacionMode > DEBOUNCE_DELAY)) {
    ultimaPulsacionMode   = millis();
    ultimoPulsacionGlobal = millis();
    ultimaActividad       = millis();
    modalidadSeleccionada = (Modalidad)((modalidadSeleccionada + 1) % 5);
    EEPROM.write(0, modalidadSeleccionada);
    mostrarLetraModalidad(modalidadSeleccionada);
  }

  if (digitalRead(BUTTON_PLAY) == HIGH &&
      (millis() - ultimaPulsacionPlay > DEBOUNCE_DELAY)) {
    ultimaPulsacionPlay   = millis();
    ultimoPulsacionGlobal = millis();
    ultimaActividad       = millis();
    iniciarSesion();
  }
}

void mostrarLetraModalidad(Modalidad mod) {
  strip.clear();
  const bool (*letra)[NUM_COLS];
  switch (mod) {
    case SUENO:         letra = LETRA_S; break;
    case ALFA:          letra = LETRA_A; break;
    case CONCENTRACION: letra = LETRA_C; break;
    case ENERGIA:       letra = LETRA_E; break;
    case ORACION:       letra = LETRA_O; break;
  }
  for (int col = 0; col < NUM_COLS; col++) {
    for (int fila = 0; fila < LEDS_PER_COL; fila++) {
      if (letra[fila][col]) {
        strip.setPixelColor(getLEDIndex(col, fila), strip.Color(25, 0, 0));
      }
    }
  }
  strip.show();
}

void iniciarSesion() {
  // Fade in de entrada
  for (int brillo = 25; brillo <= 255; brillo += 10) {
    strip.setBrightness(brillo);
    strip.show();
    delay(30);
  }
  strip.setBrightness(255);
  strip.clear();
  strip.show();

  estadoActual       = SESION_ACTIVA;
  tiempoInicioSesion = millis();
}

void ejecutarSesion() {
  if (digitalRead(BUTTON_PLAY) == HIGH &&
      (millis() - ultimaPulsacionPlay > DEBOUNCE_DELAY)) {
    ultimaPulsacionPlay   = millis();
    ultimoPulsacionGlobal = millis();
    pausarSesion();
    return;
  }

  unsigned long ms  = millis() - tiempoInicioSesion;
  unsigned long seg = ms / 1000;

  switch (modalidadSeleccionada) {
    case SUENO:         protocoloSueno(seg, ms);         break;
    case ALFA:          protocoloAlfa(seg, ms);           break;
    case CONCENTRACION: protocoloConcentracion(seg, ms);  break;
    case ENERGIA:       protocoloEnergia(seg, ms);        break;
    case ORACION:       protocoloOracion(seg, ms);        break;
  }
}

// ============================================================
// PROTOCOLOS
// ============================================================

// SUEÑO — 90 minutos — Delta 2–4 Hz — Rojo
// Efecto OLA: 3 columnas se desplazan secuencialmente
void protocoloSueno(unsigned long seg, unsigned long ms) {
  if (seg < 120) {
    float progreso   = (float)ms / 120000.0;
    float frecuencia = 8.0 - (4.0 * progreso);
    int   brillo     = 77 + (178 * progreso);
    efectoOla3Columnas(frecuencia, COLOR_ROJO, brillo);
  } else if (seg < 900) {
    float progreso   = fmod((float)ms / 1000.0, 40.0) / 40.0;
    float frecuencia = 4.0 + (2.0 * sin(progreso * 2 * M_PI));
    efectoOla3Columnas(frecuencia, COLOR_ROJO, 255);
  } else if (seg < 3300) {
    float progreso   = (float)(ms - 900000) / 2400000.0;
    float frecuencia = 4.0 - (2.0 * progreso);
    efectoOla3Columnas(frecuencia, COLOR_ROJO, 255);
  } else if (seg < 4500) {
    float progreso   = fmod((float)ms / 1000.0, 50.0) / 50.0;
    float frecuencia = 2.0 + sin(progreso * 2 * M_PI);
    efectoOla3Columnas(frecuencia, COLOR_ROJO, 255);
  } else if (seg < 5400) {
    float progreso   = (float)(ms - 4500000) / 900000.0;
    float frecuencia = 3.0 - (2.0 * progreso);
    int   brillo     = 255 - (255 * progreso);
    efectoOla3Columnas(frecuencia, COLOR_ROJO, brillo);
  } else {
    finalizarSesion();
  }
}

// ALFA — 30 minutos — 10 Hz — Azul
// Hemisferios alternos, 1 segundo por lado
void protocoloAlfa(unsigned long seg, unsigned long ms) {
  if (seg < 90) {
    float progreso   = (float)ms / 90000.0;
    float frecuencia = 10.0 - (4.0 * progreso);
    efectoHemisferioAlterno(frecuencia, COLOR_AZUL, 255);
  } else if (seg < 1620) {
    efectoHemisferioAlterno(10.0, COLOR_AZUL, 255);
  } else if (seg < 1800) {
    float progreso = (float)(ms - 1620000) / 180000.0;
    int   brillo   = 255 - (255 * progreso);
    efectoHemisferioAlterno(10.0, COLOR_AZUL, brillo);
  } else {
    finalizarSesion();
  }
}

// CONCENTRACIÓN — 20 minutos — 12 Hz — Verde
// Hemisferios alternos, 1 segundo por lado
void protocoloConcentracion(unsigned long seg, unsigned long ms) {
  if (seg < 90) {
    float progreso   = (float)ms / 90000.0;
    float frecuencia = 8.0 + (4.0 * progreso);
    int   brillo     = 77 + (178 * progreso);
    efectoHemisferioAlterno(frecuencia, COLOR_VERDE, brillo);
  } else if (seg < 1020) {
    efectoHemisferioAlterno(12.0, COLOR_VERDE, 255);
  } else if (seg < 1200) {
    float progreso = (float)(ms - 1020000) / 180000.0;
    int   brillo   = 255 - (255 * progreso);
    efectoHemisferioAlterno(12.0, COLOR_VERDE, brillo);
  } else {
    finalizarSesion();
  }
}

// ENERGÍA — 15 minutos — 6→12 Hz — Rojo
// Rampa ascendente de frecuencia, hemisferios alternos
void protocoloEnergia(unsigned long seg, unsigned long ms) {
  if (seg < 180) {
    float progreso   = (float)ms / 180000.0;
    float frecuencia = 6.0 + (6.0 * progreso);
    efectoHemisferioAlterno(frecuencia, COLOR_ROJO, 255);
  } else if (seg < 720) {
    efectoHemisferioAlterno(12.0, COLOR_ROJO, 255);
  } else if (seg < 900) {
    float progreso = (float)(ms - 720000) / 180000.0;
    int   brillo   = 255 - (255 * progreso);
    efectoHemisferioAlterno(12.0, COLOR_ROJO, brillo);
  } else {
    finalizarSesion();
  }
}

// ORACIÓN — 40 minutos — Modulación de audio — Rojo
// VU-meter vertical: brillo sigue envolvente de audio (Padre Nuestro)
void protocoloOracion(unsigned long seg, unsigned long ms) {
  if (seg < 3) {
    // Fade in inicial
    int brillo = (int)(255 * ((float)ms / 3000.0));
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(brillo, 0, 0));
    }
    strip.show();
  } else if (seg < 2400) {
    float segundos     = (ms - 3000) / 1000.0;
    float posicion     = fmod(segundos, 40.1);
    int   frameIndex   = (int)(posicion * ORACION_FPS);
    if (frameIndex >= ORACION_FRAMES) frameIndex = ORACION_FRAMES - 1;

    uint8_t brilloActual = pgm_read_byte(&oracionBrillo[frameIndex]);

    // Número de filas activas según nivel de brillo (VU-meter vertical)
    int numFilas = 0;
    if      (brilloActual <= 10)  numFilas = 0;
    else if (brilloActual <= 42)  numFilas = 1;
    else if (brilloActual <= 85)  numFilas = 2;
    else if (brilloActual <= 127) numFilas = 3;
    else if (brilloActual <= 170) numFilas = 4;
    else if (brilloActual <= 212) numFilas = 5;
    else                          numFilas = 6;

    strip.clear();
    // Encender de abajo hacia arriba en todas las columnas
    for (int col = 0; col < NUM_COLS; col++) {
      for (int fila = 0; fila < numFilas; fila++) {
        int filaReal = (LEDS_PER_COL - 1) - fila;
        strip.setPixelColor(getLEDIndex(col, filaReal),
                            strip.Color(brilloActual, 0, 0));
      }
    }
    strip.show();
  } else {
    finalizarSesion();
  }
}

// ============================================================
// EFECTOS VISUALES
// ============================================================

// Hemisferios alternos: izquierda (col 0-1) y derecha (col 2-3)
// se alternan cada 1 segundo, parpadeando a la frecuencia indicada
void efectoHemisferioAlterno(float frecuencia, uint32_t color, int brillo) {
  const unsigned long DURACION_HEMISFERIO = 1000;
  unsigned long tiempoEnCiclo  = millis() % (DURACION_HEMISFERIO * 2);
  bool hemisferioIzquierdo     = (tiempoEnCiclo < DURACION_HEMISFERIO);

  int periodoMs = (int)(1000.0 / frecuencia);
  int tiempoON  = (int)(periodoMs * 0.30); // 30% encendido
  int tiempoOFF = periodoMs - tiempoON;

  static unsigned long tiempoInicioParpadeo = 0;
  static bool ledsEncendidos = true;
  unsigned long ahora = millis();

  if (ledsEncendidos) {
    if (ahora - tiempoInicioParpadeo >= (unsigned long)tiempoON) {
      strip.clear();
      strip.show();
      ledsEncendidos       = false;
      tiempoInicioParpadeo = ahora;
    }
  } else {
    if (ahora - tiempoInicioParpadeo >= (unsigned long)tiempoOFF) {
      strip.clear();
      uint8_t r = ((color >> 16) & 0xFF) * brillo / 255;
      uint8_t g = ((color >>  8) & 0xFF) * brillo / 255;
      uint8_t b = ( color        & 0xFF) * brillo / 255;

      int colInicio = hemisferioIzquierdo ? 0 : 2;
      for (int col = colInicio; col < colInicio + 2; col++) {
        for (int fila = 0; fila < LEDS_PER_COL; fila++) {
          strip.setPixelColor(getLEDIndex(col, fila), strip.Color(r, g, b));
        }
      }
      strip.show();
      ledsEncendidos       = true;
      tiempoInicioParpadeo = ahora;
    }
  }
}

// Efecto OLA: 3 columnas de 4 se encienden en secuencia rotatoria
// creando una sensación de movimiento continuo (modo SUEÑO)
void efectoOla3Columnas(float frecuencia, uint32_t color, int brillo) {
  int periodoMs    = (int)(1000.0 / frecuencia);
  int tiempoPorPaso = periodoMs / 4;
  int tiempoON     = (int)(tiempoPorPaso * 0.60); // 60% encendido
  int tiempoOFF    = tiempoPorPaso - tiempoON;

  static unsigned long tiempoInicioEstado = 0;
  static bool bandasEncendidas = true;
  unsigned long ahora = millis();

  if (bandasEncendidas) {
    if (ahora - tiempoInicioEstado >= (unsigned long)tiempoON) {
      strip.clear();
      strip.show();
      bandasEncendidas   = false;
      tiempoInicioEstado = ahora;
    }
  } else {
    if (ahora - tiempoInicioEstado >= (unsigned long)tiempoOFF) {
      strip.clear();
      pasoActual = (pasoActual + 1) % 4;

      // Tres columnas activas según el paso actual (rotación circular)
      int cols[3];
      switch (pasoActual) {
        case 0: cols[0]=0; cols[1]=1; cols[2]=2; break;
        case 1: cols[0]=1; cols[1]=2; cols[2]=3; break;
        case 2: cols[0]=2; cols[1]=3; cols[2]=0; break;
        case 3: cols[0]=3; cols[1]=0; cols[2]=1; break;
      }

      uint8_t r = ((color >> 16) & 0xFF) * brillo / 255;
      uint8_t g = ((color >>  8) & 0xFF) * brillo / 255;
      uint8_t b = ( color        & 0xFF) * brillo / 255;

      for (int c = 0; c < 3; c++) {
        for (int i = 0; i < LEDS_PER_COL; i++) {
          strip.setPixelColor(getLEDIndex(cols[c], i), strip.Color(r, g, b));
        }
      }
      strip.show();
      bandasEncendidas   = true;
      tiempoInicioEstado = ahora;
    }
  }
}

// ============================================================
// CONTROL DE SESIÓN
// ============================================================

void pausarSesion() {
  tiempoPausado = millis() - tiempoInicioSesion;
  estadoActual  = SESION_PAUSADA;
  strip.clear();
  strip.show();
}

void manejarPausa() {
  if (digitalRead(BUTTON_PLAY) == HIGH &&
      (millis() - ultimaPulsacionPlay > DEBOUNCE_DELAY)) {
    ultimaPulsacionPlay   = millis();
    ultimoPulsacionGlobal = millis();
    reanudarSesion();
    return;
  }

  // Parpadeo suave de indicador de pausa
  static unsigned long ultimoParpadeo = 0;
  static bool ledsEncendidos = false;

  if (millis() - ultimoParpadeo >= 500) {
    ultimoParpadeo = millis();
    ledsEncendidos = !ledsEncendidos;
    strip.clear();
    if (ledsEncendidos) {
      for (int i = 0; i < LEDS_PER_COL; i++) {
        strip.setPixelColor(getLEDIndex(1, i), strip.Color(13, 0, 0));
        strip.setPixelColor(getLEDIndex(2, i), strip.Color(13, 0, 0));
      }
    }
    strip.show();
  }
}

void reanudarSesion() {
  tiempoInicioSesion = millis() - tiempoPausado;
  estadoActual       = SESION_ACTIVA;
  strip.clear();
  strip.show();
}

void finalizarSesion() {
  // Fade out al terminar la sesión
  for (int brillo = 255; brillo >= 0; brillo -= 5) {
    strip.setBrightness(brillo);
    strip.show();
    delay(30);
  }
  strip.clear();
  strip.show();
  strip.setBrightness(255);
  delay(2000);

  estadoActual = MENU;
  mostrarLetraModalidad(modalidadSeleccionada);
  ultimaActividad = millis();
}

// ============================================================
// FIN — NEUROSYNC ALPHA v5.15 — neuralinducer.org
// ============================================================
