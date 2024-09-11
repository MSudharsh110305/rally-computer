#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GPS.h>  // Include your GPS library here

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// GSM/GPS variables (assuming GPS is used for time)
Adafruit_GPS gps = Adafruit_GPS(&Serial1);  // Initialize GPS on Serial1

bool blinkState = false;
unsigned long lastBlinkTime = 0;
const unsigned long BLINK_INTERVAL = 500;
unsigned long lastPageSwitchTime = 0;
const unsigned long PAGE_SWITCH_DELAY = 2000;

int currentPage = 0;
unsigned long stopwatchStart = 0;
unsigned long stopwatchElapsed = 0;
bool stopwatchRunning = false;

int scrollOffset = 0; // Variable for scrolling message

void setup() {
    SerialUSB.begin(115200);
    gps.begin(9600);  // Start GPS at 9600 baud

    // Initialize the OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC)) {
        SerialUSB.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    display.clearDisplay();
    display.display();
}

void loop() {
    unsigned long currentTime = millis();

    // Update GPS to fetch real-time clock (RTC)
    if (gps.newNMEAreceived()) {
        gps.parse(gps.lastNMEA());  // Parse new NMEA data
    }

    // Handle page switching
    if (currentPage == 0 && currentTime - lastPageSwitchTime >= PAGE_SWITCH_DELAY) {
        currentPage = 1;  // Switch to the main UI after 2 seconds
    }

    // Blinking logic for battery low warning
    if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
        blinkState = !blinkState;  // Toggle blink state
        lastBlinkTime = currentTime;
    }

    // Render appropriate page
    display.clearDisplay();
    switch (currentPage) {
        case 0:
            drawStartupPage();
            break;
        case 1:
            drawMainUI(gpsTime(), "C1", 100, "--.--", "--.--", "Hazard 153m ahead! Car3 pressed SOS. Stay safe!");
            break;
        case 2:
            drawStopwatchAndElapsedUI(gpsTime(), "C1", 20, "00:00:00", "00:00:00", "Broadcasting message");
            break;
        case 3:
            drawStartEndTimePage(gpsTime(), "C1", 20, "00:00:00", "00:00:00");
            break;
        case 4:
            drawOverallResultsPage(gpsTime(), "C1", 20, "--.--", "00:00:00", "--");
            break;
    }
    display.display();

    delay(10);
}

// Function to fetch time from GPS (RTC)
String gpsTime() {
    if (gps.hour < 10) {
        return String("0") + gps.hour + ":" + (gps.minute < 10 ? "0" + String(gps.minute) : String(gps.minute));
    }
    return String(gps.hour) + ":" + (gps.minute < 10 ? "0" + String(gps.minute) : String(gps.minute));
}

// Draw the startup page
void drawStartupPage() {
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(SCREEN_WIDTH / 6, SCREEN_HEIGHT / 2 - 10);
    display.println("BLUEBAND  SPORTS");
}

// Draw the main UI page
/* void drawMainUI(String time, String carID, int batteryPercentage, String speed, String distance, String message) {
    // Top section
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(time);

    // Car ID with circle (toggle between filled and hollow)
    display.setCursor(SCREEN_WIDTH / 2 - 10, 0);
    display.print(carID);
    drawCircleWithToggle(SCREEN_WIDTH / 2 + 18, 4, blinkState);

    drawBatteryStatus(batteryPercentage);

    // Divider line
    drawLine(0, 12, SCREEN_WIDTH, 12);

    // Speed and Distance
    display.setCursor(0, 16);
    display.println("spd: " + (speed != "--.--" ? speed : "N/A"));

    display.setCursor(0, 26);
    display.println("dis: " + (distance != "--.--" ? distance : "N/A"));

    // Divider line
    drawLine(0, 50, SCREEN_WIDTH, 36);

    // Broadcasting message with scrolling
    display.setCursor(-scrollOffset+15, 56);  // Scrolling from left to right
    display.println(message);
    scrollOffset++;
    if (scrollOffset > SCREEN_WIDTH) scrollOffset = 0;  // Reset scroll
} */
// Draw the main UI page
void drawMainUI(String time, String carID, int batteryPercentage, String speed, String distance, String message) {
    // Top section
    display.setTextSize(1);
    display.setCursor(0, 3);
    display.print(time);

    // Car ID with circle (toggle between filled and hollow)
    display.setCursor(SCREEN_WIDTH / 2 - 15, 3);
    display.print(carID);
    drawCircleWithToggle(SCREEN_WIDTH / 2 + 18, 5, blinkState);

    drawBatteryStatus(batteryPercentage);

    // Divider line
    drawLine(0, 12, SCREEN_WIDTH, 12);

    // Speed and Distance
    display.setCursor(5, 16);
    display.setTextSize(2);
    display.println("spd:" + (speed != "--.--" ? speed : "N/A"));

    display.setCursor(5, 36);
    display.println("dis:" + (distance != "--.--" ? distance : "N/A"));
    display.setTextSize(1);

    // Divider line
    drawLine(0, 52, SCREEN_WIDTH, 36);

    // Broadcasting message with scrolling
    int messageWidth = message.length() * 6;  // Approx. width of the message
    display.setCursor(-scrollOffset, 55);  // Scroll from left to right
    display.print(message + " ");  // Append a space for separation
    display.print(message);  // Repeat the message to create a loop

    scrollOffset++;
    if (scrollOffset > messageWidth) scrollOffset = 0;  // Reset scroll when end of message is reached
    drawLine(0, 63, SCREEN_WIDTH, 36);

}

// Draw Circle with toggle between hollow/filled
void drawCircleWithToggle(int x, int y, bool isFilled) {
    if (isFilled) {
        display.fillCircle(x, y, 4, SSD1306_WHITE);  // Filled circle
    } else {
        display.drawCircle(x, y, 4, SSD1306_WHITE);  // Hollow circle
    }
}

// Draw Stopwatch and Elapsed Time combined UI
void drawStopwatchAndElapsedUI(String time, String carID, int batteryPercentage, String stopwatchTime, String elapsedTime, String message) {
    // Top section
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(time);

    // Car ID with circle (toggle between filled and hollow)
    display.setCursor(SCREEN_WIDTH / 2 - 10, 0);
    display.print(carID);
    drawCircleWithToggle(SCREEN_WIDTH / 2 + 18, 4, blinkState);

    drawBatteryStatus(batteryPercentage);

    // Divider line
    drawLine(0, 12, SCREEN_WIDTH, 12);

    // Stopwatch and Elapsed Time
    display.setCursor(0, 16);
    display.println("SW: " + (stopwatchTime != "00:00:00" ? stopwatchTime : "N/A"));

    display.setCursor(0, 26);
    display.println("ET: " + (elapsedTime != "00:00:00" ? elapsedTime : "N/A"));

    // Divider line
    drawLine(0, 35, SCREEN_WIDTH, 36);

    // Broadcasting message with scrolling
    display.setCursor(-scrollOffset, 40);  // Scrolling from left to right
    display.println(message);
    scrollOffset++;
    if (scrollOffset > SCREEN_WIDTH) scrollOffset = 0;  // Reset scroll
}

// Draw start/end time record page
void drawStartEndTimePage(String time, String carID, int batteryPercentage, String startTime, String endTime) {
    // Top section
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(time);

    // Car ID with circle (toggle between filled and hollow)
    display.setCursor(SCREEN_WIDTH / 2 - 10, 0);
    display.print(carID);
    drawCircleWithToggle(SCREEN_WIDTH / 2 + 18, 4, blinkState);

    drawBatteryStatus(batteryPercentage);

    // Divider line
    drawLine(0, 12, SCREEN_WIDTH, 12);

    // Start and End times
    display.setCursor(0, 16);
    display.println("st: " + (startTime != "00:00:00" ? startTime : "N/A"));
    
    display.setCursor(0, 26);
    display.println("en: " + (endTime != "00:00:00" ? endTime : "N/A"));
}

// Draw overall results page
void drawOverallResultsPage(String time, String carID, int batteryPercentage, String avgSpeed, String totalTime, String penalties) {
    // Top section
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(time);

    // Car ID with circle (toggle between filled and hollow)
    display.setCursor(SCREEN_WIDTH / 2 - 10, 0);
    display.print(carID);
    drawCircleWithToggle(SCREEN_WIDTH / 2 + 18, 4, blinkState);

    drawBatteryStatus(batteryPercentage);

    // Divider line
    drawLine(0, 12, SCREEN_WIDTH, 12);

    // Average speed, total time, and penalties
    display.setCursor(0, 16);
    display.println("avg spd: " + (avgSpeed != "--.--" ? avgSpeed : "N/A") + " km/h");

    display.setCursor(0, 26);
    display.println("total time: " + (totalTime != "00:00:00" ? totalTime : "N/A"));
    
    display.setCursor(0, 36);
    display.println("penalties: " + (penalties != "--" ? penalties : "N/A"));
}

// Function to draw Battery Status in a box and blink "low" when below 10%
void drawBatteryStatus(int batteryPercentage) {
    display.setCursor(SCREEN_WIDTH - 25, 3);
    display.println(String(batteryPercentage) + "%");
    if (batteryPercentage < 10 && blinkState) {
        display.setCursor(SCREEN_WIDTH - 30, 10);
        display.println("LOW");
    }
}

// Function to draw a dashed line
void drawLine(int x0, int y0, int x1, int y1) {
    for (int x = x0; x < x1; x += 1) {
        display.drawPixel(x, y0, SSD1306_WHITE);
    }
}
