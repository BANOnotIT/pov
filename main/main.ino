// ----- настройки -----

#define DI_PIN 7                        // пин подключения ленты
#define NUM_LEDS 14                     // количество светодиодов в ленте

#define ROUND_TIME 300                  // время одного полного оборота оси на полной скорости

#define MOTOR_PIN 3                     // пин подключения потора

#define start_flashes 1                 // проверка цветов при запуске (1 - включить, 0 - выключить)

// ---- \настройки/ ----
#include <FastLED.h>
#include <SoftwareSerial.h>


char leds_mem[NUM_LEDS];

bool leds_on = false;
bool showing_half = false;

String strData = "";

CRGB leds[NUM_LEDS];  // создаём ленту

void setup() {
    FastLED.addLeds<WS2812, DI_PIN, GRB>(leds, NUM_LEDS);  // инициализация светодиодов

    // вспышки красным синим и зелёным при запуске (можно отключить)
    if (start_flashes) {
        LEDS.showColor(CRGB(255, 0, 0));
        delay(500);
        LEDS.showColor(CRGB(0, 255, 0));
        delay(500);
        LEDS.showColor(CRGB(0, 0, 255));
        delay(500);
        LEDS.showColor(CRGB(0, 0, 0));

        for (char i = 0; i < NUM_LEDS; i++) {
            leds[i].setHue(0); // показываем красный
            FastLED.show(); // записываем на ленточку
            delay(200);
            LEDS.showColor(CRGB(0, 0, 0)); // вырубаем все диоды
        }
    }


    Serial.begin(9600);

    while (!Serial) { ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.print("Hi!!!\n");
}

void loop() {

    long availableBytes = Serial.available(); // обновляем число байт в буфере serial

    if (availableBytes == 1) {
        delay(10);
        if (Serial.available() == 1) {
            char cmd = Serial.read();

            if (cmd == '^')
                Start();

            else if (cmd == 'V') {
                LEDS.showColor(CRGB::Black); // убираем полностью свет
                analogWrite(MOTOR_PIN, 0); // отключаем мотор
                leds_on = false;
            }
        }
    }

    if (availableBytes == NUM_LEDS) {

        for (char i = 0; i < NUM_LEDS; i++) {
            char sym = Serial.read();

//            magic
            sym = sym > 47 && sym < 58 ? // numeric char
                  sym - 48 :
                  sym > 96 && sym < 103 ? // a-f char
                  sym - 87 :
                  0;


            leds_mem[i] = sym << 4 & sym;
        }

        showing_half = false;
    }


    if (leds_on) {

        bool visible_half = millis() % ROUND_TIME > ROUND_TIME / 2;

        if (visible_half && !showing_half) {
            ShowColorsFromMem();
            showing_half = true;
        } else if (!visible_half && showing_half) {
            FastLED.showColor(CRGB(0, 0, 0));
            showing_half = false;
        }


    }

}

void Start() {
// показываем радугу на первых 14 лампочках
//    leds[13].setHue(0); // каждый
//    leds[12].setHue(0); // каждый
//    leds[11].setHue(28); // охотник
//    leds[10].setHue(28); // охотник
//    leds[9].setHue(43); // желает
//    leds[8].setHue(43); // желает
//    leds[7].setHue(85); // знать
//    leds[6].setHue(85); // знать
//    leds[5].setHue(128); // где
//    leds[4].setHue(128); // где
//    leds[3].setHue(170); // сидит
//    leds[2].setHue(170); // сидит
//    leds[1].setHue(213); // фазан
//    leds[0].setHue(213); // фазан

    leds_mem = {213, 213, 170, 170, 128, 128, 85, 85, 43, 43, 28, 28, 0, 0};
    showing_half = false;
    leds_on = true;

// "да будет движение" - сказал Ньютон и включил моторчик
    Serial.print("Starting Motor");
    analogWrite(MOTOR_PIN, 1023);
}

void ShowColorsFromMem() {

    for (char i = 0; i < NUM_LEDS; i++)
        leds[i].setHue(leds_mem[i]);       // назначаем цвет

    FastLED.show();                 // записываем в ленточку

}