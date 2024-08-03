# Rally Car Tracking System

## Overview
This project is part of the BLUEBAND SPORTS initiative to develop a real-time tracking system for rally cars. The system includes features like SOS and OK interrupt buttons, providing safety and monitoring capabilities during rallies. This repository focuses on the hardware and software integration using the Maduino-Zero-LTE-SIM 7600E.

## Team Members
- [Sudharsh](https://github.com/yourusername) - CSE - Hardware and Software Integration
- [Tharun](https://github.com/theirusername) - CSE - Hardware and Software Integration
- [Akaash](https://github.com/theirusername) - ECE - Hardware
- [Methun](https://github.com/theirusername) - ECE - Hardware
- [Naresh Ram](https://github.com/Nareshram1) - IT - Server-side code and integration.
- [Joe Prathap](https://github.com/prathap03) - IT - Server-side code and integration.

## Project Phases
### Phase 1
- Real-time tracking of rally cars.
- Implementation of SOS and OK interrupt button features.
- Hardware and software integration using Maduino-Zero-4G-LTE-SIM 7600E.
- HTTP communication with the central server.

### Phase 2
- Alerting only the nearby upcoming cars and the central hub when a car sends an SOS or HAZARD alert, without affecting cars that have already passed the broken-down car.

## Getting Started
### Prerequisites
- Maduino-Zero-4G-LTE-SIM-7600E
- Arduino IDE
- SIM card with data plan

### Setup
1. Clone this repository to your local machine:
    ```bash
    git clone https://github.com/MSudharsh110305/rally-car-tracking.git
    ```
2. Open the `real_time_tracking.ino` file in Arduino IDE.
3. Connect your Maduino-Zero-4G-LTE-SIM 7600E to your computer.
4. Update the web URL to your server in the code
5. Upload the code to your device.

## Usage
1. Power on the Maduino device.
2. Ensure the SIM card is active and has a data plan.
3. The device will start sending real-time location data to the central server.
4. Use the SOS and OK buttons to send respective alerts.

## Acknowledgements

## References
- [Makerfabs Maduino-Zero-4G-LTE](https://github.com/Makerfabs/Maduino-Zero-4G-LTE) - Official GitHub repository with additional resources and examples.
- [Repository by rcravens](https://github.com/rcravens/230225-095033-zeroUSB) - A comprehensive repository with examples and additional resources for integrating Maduino-Zero-4G-LTE-SIM 7600E with other features and services.
  
## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
