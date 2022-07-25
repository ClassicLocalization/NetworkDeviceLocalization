# Station for static csi exchange between hardcoded MACs and IPs

## Functionality

This program is used for creating a distance matrix between all ESP32 devices in a network. An AP will be necessary for the activation of each ESP32 device. This project therefore only with specific UDP messages sent. Depending on the step-variable, a different IP address/ESP32 device will be used for a CSI exchange. The resulting CSI data is sent to a hardcoded IP. This hardcoded IP is generally the device, with which the data analysis is performed with.
