# Binary Protocol
## Overview

Most LightWare devices that have been manufactured since 2018 include a new communication protocol (LWNX). Some benefits:
- Binary protocol to minimize data transfer size.
- CRC checking to ensure data integrity.
- Support tools can easily configure and query devices.
- I2C and serial UART support.
- Devices all share the same protocol.
- Large amounts of data can be streamed from the device.

Newer devices like the SF22 only use the new binary protocol, however many of our devices still support legacy protocols as well as the new binary protocol. If you would like to use the binary protocol, you can look at samples in this repository.

> Please note that the LWNX protocol is not documented for all devices. If you have questions our support staff will be willing to assist.