#define DEBUG false

const int stopwatchButtonPin = 10;  // Pin for stopwatch button

volatile bool stopwatchPressed = false;  // Track stopwatch button press
bool stopwatchRunning = false;           // Track if stopwatch is running
unsigned long stopwatchStartTime = 0;
unsigned long stopwatchElapsedTime = 0;

void handleStopwatchPress() {
    delay(50);
    if (digitalRead(stopwatchButtonPin) == LOW) {
        stopwatchPressed = true;
    }
}

void setup() {
    SerialUSB.begin(115200);  // Initialize serial communication for debugging

    pinMode(stopwatchButtonPin, INPUT_PULLUP);  // Set up the stopwatch button pin with internal pull-up resistor

    attachInterrupt(digitalPinToInterrupt(stopwatchButtonPin), handleStopwatchPress, FALLING);  // Attach interrupt for stopwatch button

    stopwatchPressed = false;
    SerialUSB.println("Stopwatch initialized.");
}

void loop() {
    if (stopwatchPressed) {
        stopwatchPressed = false;
        if (!stopwatchRunning) {
            // Start the stopwatch
            stopwatchStartTime = millis();
            stopwatchRunning = true;
            SerialUSB.println("Stopwatch started");
        } else {
            // Stop the stopwatch
            stopwatchElapsedTime = millis() - stopwatchStartTime;
            stopwatchRunning = false;

            // Convert elapsed time to hours, minutes, seconds, milliseconds
            unsigned long totalSeconds = stopwatchElapsedTime / 1000;
            unsigned long hours = totalSeconds / 3600;
            unsigned long minutes = (totalSeconds % 3600) / 60;
            unsigned long seconds = totalSeconds % 60;
            unsigned long milliseconds = stopwatchElapsedTime % 1000;

            // Print the formatted stop time
            SerialUSB.print("Stopwatch stopped. Time: ");
            SerialUSB.print(hours);
            SerialUSB.print("h ");
            SerialUSB.print(minutes);
            SerialUSB.print("m ");
            SerialUSB.print(seconds);
            SerialUSB.print("s ");
            SerialUSB.print(milliseconds);
            SerialUSB.println("ms");
        }
    }

    // If stopwatch is running, continuously update the elapsed time
    if (stopwatchRunning) {
        stopwatchElapsedTime = millis() - stopwatchStartTime;
    }

    delay(10);  // Short delay to avoid excessive CPU usage
}
