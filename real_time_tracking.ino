#define DEBUG false

#define LTE_PWRKEY_PIN 5
#define LTE_RESET_PIN 6
#define LTE_FLIGHT_PIN 7

const int switch1Pin = 8;
const int switch2Pin = 9;

volatile bool sosPressed = false;
volatile bool okPressed = false;
volatile bool sendingMessage = false;

unsigned long lastTrackSendTime = 0;
const unsigned long trackSendInterval = 500; // 0.5 seconds

int gpsSendAttempts = 0;
const int maxAttempts = 25;

String sendData(String command, const int timeout, boolean debug = false) {
    String response = "";
    Serial1.println(command);

    long int startTime = millis();
    while (millis() - startTime < timeout) {
        while (Serial1.available()) {
            char c = Serial1.read();
            response += c;
        }
    }

    if (debug) {
        SerialUSB.print(command);
        SerialUSB.print(" Response: ");
        SerialUSB.println(response);
    }

    return response;
}

String extractNMEA(String response) {
    int start = response.indexOf("+CGPSINFO: ");
    if (start != -1) {
        start += 11; // Move past "+CGPSINFO: "
        int end = response.indexOf("OK", start);
        if (end != -1 && end > start) {
            String nmea = response.substring(start, end);
            nmea.trim(); // Trim any leading/trailing whitespace
            return nmea;
        }
    }
    return ""; // Return empty string if NMEA sentence not found or invalid format
}

bool sendHTTPRequest(String url, String jsonPayload, bool breath) {
    if (breath) {
        delay(50);  // Adjusted for minimal delay
    }
    SerialUSB.println("Sending HTTP request to " + url);

    sendData("AT+HTTPINIT", 100, DEBUG);
    sendData("AT+HTTPPARA=\"CID\",1", 100, DEBUG);
    sendData("AT+HTTPPARA=\"URL\",\"" + url + "\"", 100, DEBUG);
    sendData("AT+HTTPPARA=\"CONTENT\",\"application/json\"", 100, DEBUG);
    sendData("AT+HTTPDATA=" + String(jsonPayload.length()) + ",2000", 100);
    sendData(jsonPayload, 100, DEBUG);
    String response = sendData("AT+HTTPACTION=1", 2000, DEBUG);
    sendData("AT+HTTPTERM", 100, DEBUG);

    if (breath) {
        if (response.indexOf("200") != -1) {
            SerialUSB.println("HTTP request sent successfully.");
            return false;
        } else {
            SerialUSB.println("Failed to send HTTP request.");
            return true;
        }
    } else {
        return true;
    }
}

void sendTrackData() {
    String gpsInfo = sendData("AT+CGPSINFO", 1300, DEBUG);
    String nmeaSentence = extractNMEA(gpsInfo);
    SerialUSB.println(nmeaSentence + " nmea");

    if (nmeaSentence.length() > 8) {
        SerialUSB.println("Sending Track data");

        String jsonPayload = "{\"nmea\": \"" + nmeaSentence + "\",\"carId\":2}";
        String requestBinURL = "<YOUR_TRACK_ENDPOINT>";
        if (sendHTTPRequest(requestBinURL, jsonPayload, false)) {
            // Retry logic if needed
        }
    }
}

void handleSosPress() {
    delay(50);  // Simple debounce
    if (digitalRead(switch1Pin) == LOW) {  // Check if the button is still pressed
        sosPressed = true;
    }
}

void handleOkPress() {
    delay(50);  // Simple debounce
    if (digitalRead(switch2Pin) == LOW) {  // Check if the button is still pressed
        okPressed = true;
    }
}

void setup() {
    SerialUSB.begin(115200);
    Serial1.begin(115200);

    pinMode(LTE_RESET_PIN, OUTPUT);
    digitalWrite(LTE_RESET_PIN, LOW);

    pinMode(LTE_PWRKEY_PIN, OUTPUT);
    digitalWrite(LTE_PWRKEY_PIN, LOW);
    delay(100);
    digitalWrite(LTE_PWRKEY_PIN, HIGH);
    delay(2000);
    digitalWrite(LTE_PWRKEY_PIN, LOW);

    pinMode(LTE_FLIGHT_PIN, OUTPUT);
    digitalWrite(LTE_FLIGHT_PIN, LOW);

    pinMode(switch1Pin, INPUT_PULLUP);  // Enable internal pull-up resistor
    pinMode(switch2Pin, INPUT_PULLUP);  // Enable internal pull-up resistor

    attachInterrupt(digitalPinToInterrupt(switch1Pin), handleSosPress, FALLING);
    attachInterrupt(digitalPinToInterrupt(switch2Pin), handleOkPress, FALLING);

    while (!SerialUSB) {
        delay(10);
    }

    sosPressed = false;
    okPressed = false;

    SerialUSB.println("Switch test initialized.");
    //String response = sendData("AT+CFUN=1,1", 5000, DEBUG);
    String response = sendData("AT+CGATT=0", 2000, DEBUG);
   response = sendData("AT+CGATT=1", 2000, DEBUG);
  response = sendData("AT+CGACT=1,1", 2000, DEBUG);
  response = sendData("AT+CGPADDR=1", 1300, DEBUG);
  if (response.indexOf("OK") != -1 && response.indexOf(".") != -1) {
    SerialUSB.println("Internet connected.");
  } else {
    SerialUSB.println("Internet not connected.");
  }
    response = sendData("AT+CGPS=0", 3000, DEBUG);
    response = sendData("AT+CGPS=1", 3000, DEBUG);
}

void loop() {
    if (sosPressed) {
        sendingMessage = true;
        sosPressed = false;
        int max_sos_attempt = 0;
        while (max_sos_attempt++ < 25) {
            if (!sendHTTPRequest("<YOUR_SOS_ENDPOINT>", "{\"carId\":2, \"message\": \"SOS\"}", true)) {
                SerialUSB.println("SOS DONE");
                break;
            }
            SerialUSB.println("SOS NOT DONE");
        }
        sendingMessage = false;
    }

    if (okPressed) {
        sendingMessage = true;
        okPressed = false;
        int max_ok_attempt = 0;
        while (max_ok_attempt++ < 25) {
            if (!sendHTTPRequest("<YOUR_OK_ENDPOINT>", "{\"carId\":2, \"message\": \"OK\"}", true)) {
                SerialUSB.println("OK DONE");
                break;
            }
            SerialUSB.println("OK NOT DONE");
        }
        sendingMessage = false;
    }

    if (!sendingMessage) {
        sendTrackData();  // Attempt to send track data only if no SOS or OK is being sent
    }

    delay(trackSendInterval); // Add a delay to prevent constant looping
}
