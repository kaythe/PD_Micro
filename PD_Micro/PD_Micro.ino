
#include <Wire.h>
#include "src/PD_UFP.h"

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// You can use any (4 or) 5 pins
#define SCLK_PIN 15
#define MOSI_PIN 16
#define DC_PIN   18
#define CS_PIN   19
#define RST_PIN  20

// Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

#define V_PD(v)     ((((uint16_t)v - 0.01f) / 20.0f))
#define A_PD(a)     ((((uint16_t)a - 0.01f) / 100.0f))

PD_UFP_c PD_UFP;

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

bool is_tft_setup = false;
uint8_t tft_step = 0;
uint16_t tft_line = 0;

void setup() {
  Serial.begin(115200);
  //Serial1.begin(115200);
  Wire.begin();
  PD_UFP.init(PD_POWER_OPTION_MAX_20V);
}

PD_power_info_t pinf;
uint8_t ind = 0;
uint16_t wait_til = 5000;

void loop() {
  PD_UFP.run();
  //PD_UFP.print_status(Serial1);
  if (PD_UFP.is_power_ready()) {
    if (PD_UFP.get_voltage() == PD_V(20.0) && PD_UFP.get_current() >= PD_A(1.5)) {
      PD_UFP.set_output(1);   // Turn on load switch
      PD_UFP.set_led(1);      // Output reach 20V and 1.5A, set indicators on
      if (PD_UFP.get_power_info(&pinf, ind))
      {
        ind++;
      }
      else
      {
        ind = 0;
      }
    } else {
      PD_UFP.set_output(0);   // Turn off load switch
      PD_UFP.blink_led(400);  // Output less than 20V or 1.5A, blink LED
    }

    if (millis() > wait_til)
    {
      if (!is_tft_setup)
      {
        tft.begin();
        tft.fillRect(0, 0, 128, 128, BLACK);

        tft.setCursor(0, 0);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);

        is_tft_setup = true;
      }

      //Serial1.print(ind - 1);
      //Serial1.print(V_PD(pinf.max_v));
      //Serial1.println(A_PD(pinf.max_i));
      tft.setCursor(0, 16 * (ind - 1));
      tft.print(V_PD(pinf.max_v));
      tft.print("V");
      tft.print(A_PD(pinf.max_i));
      tft.println("A");

      wait_til = millis() + 2500;
    }
  }
}
