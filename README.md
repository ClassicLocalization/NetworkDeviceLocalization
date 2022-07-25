# Network Device Localization

## Description

This Repository contains all software components used in the bachelorthesis "Self-Localization of IoT-Systems".

## Requirements
For the Esp32 boards, the RSSI outputs were tested on version 4.3 and 4.4 of esp-idf. For the output of CSI values, the latest version of esp-csi is needed (tested on 06.02.2022)
and an esp-idf version of atleast 4.4.

## Directory
- esp: Projects that revolve around the ESP32 devices
- ZeroMQ: Clients and servers for the receiving and sending of messages to the ESP32 APs and STAs
- A-Frame: A positioning algorithm for the Sensor Network Localization Problem (SNLP) using A-Frame for visualising the device positions
- Radar-App: An android app for localizing the ESP32 AP of a network through RSSI and CSI values
- Data-Analysis: Contains all the data gathered for the experiments and the visualisation of the data in 2D and 3D with possible Polynomial Regressions
