#include <Arduino.h>

#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <SPI.h>
#include <Wire.h>

#include <RTClib.h>
#include <DMD.h>
#include <TimerOne.h>

#include "Arial_black_16.h"
#include "Arial_Black_16_ISO_8859_1.h"
#include "Arial14.h"
#include "SystemFont5x7.h"


static constexpr uint8_t DISPLAYS_ACROSS  = 1; ///--> Number of P10 panels used, side to side.
static constexpr uint8_t DISPLAYS_DOWN    = 1;

static DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
static RTC_DS3231 rtc;

// Struct to hold the DS3231's data
struct rtc_data_t {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    double temperature;
};

static QueueHandle_t rtc_data_queue = nullptr;


static void scan_dmd() {
    dmd.scanDisplayBySPI();
}

static void rtc_read_task(void *arg) {
  
    rtc_data_t data = {};
  
    while (1) {
        
        DateTime now = rtc.now();
        
        data.hour = now.hour();
        data.minute = now.minute();
        data.second = now.second();
        data.day = now.day();
        data.month = now.month();
        data.year = now.year();
        data.temperature = rtc.getTemperature();
        
        // Send to queue
        xQueueOverwrite(rtc_data_queue, &data);

        // Read every 100ms
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void p10_display_task(void *arg) {
  
    rtc_data_t data = {};
    char p10_display_text[150] = "";

    while (1) {

        // Receive from queue (block till we receive data from rtc_data_queue)
        if (xQueueReceive(rtc_data_queue, &data, portMAX_DELAY) != pdPASS) {
            Serial.println("Failed to receive data from rtc_data_queue");
            continue;
        }

        // Use whichever font you prefer
        dmd.selectFont(Arial_Black_16_ISO_8859_1);
        //dmd.selectFont(Arial_Black_16);
        //dmd.selectFont(Arial_14);
        //dmd.selectFont(SystemFont5x7);

        sprintf(p10_display_text, "Welcome to 300L Electrical/Electronics Engineering Class  Time: %02d:%02d:%02d  Date: %02d/%02d/%04d  Temp: %.2fC", 
                data.hour, data.minute, data.second, data.day, data.month, data.year, data.temperature);
        
        // Draw scrolling text
        dmd.drawMarquee(p10_display_text, strlen(p10_display_text), (32 * DISPLAYS_ACROSS) - 1, 0);
        
        uint64_t start = millis();
        uint64_t timer = start;
        bool ret = false;
        uint8_t interval = 100;
        
        while(!ret) {
            if ((timer + interval) < millis()) {
                ret = dmd.stepMarquee(-1, 0);
                timer = millis();
            }
        }
    }
}

void setup() {

    Serial.begin(115200);

    // Initialize RTC
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC DS3231 module");
        while (1);
    }

    // Set RTC time to compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Initialize DMD
    Timer1.initialize(1000);
    Timer1.attachInterrupt(scan_dmd);
    dmd.clearScreen(true);

    // Create queue
    rtc_data_queue = xQueueCreate(1, sizeof(rtc_data_t));

    if (!rtc_data_queue) {
        Serial.println("Failed to create rtc_data_queue");
        while (1);
    }

    // Create tasks
    BaseType_t ret = xTaskCreate(rtc_read_task, "rtc_read_task", 128, nullptr, 2, nullptr);
    if (ret != pdPASS) {
        Serial.println("Failed to create rtc_read_task");
        while (1);
    }

    ret = xTaskCreate(p10_display_task, "p10_display_task", 384, nullptr, 1, nullptr);
    if (ret != pdPASS) {
        Serial.println("Failed to create p10_display_task");
        while (1);
    }

    // Start scheduler
    vTaskStartScheduler();
}

void loop() {
    // Empty - FreeRTOS tasks handle everything
}