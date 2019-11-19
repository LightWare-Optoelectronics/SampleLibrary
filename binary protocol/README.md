# Binary Protocol
## Overview

Most LightWare devices that have been manufactured since 2019 include a new communication protocol, often referred to as LWNX. Some benefits:
- Binary protocol to minimize data transfer size.
- CRC checking to ensure data integrity.
- Support tools can easily configure and query devices.
- I2C and serial UART support.
- Devices all share the same protocol.
- Large amounts of data can be streamed from the device.

Newer devices like the SF22 only support the new binary protocol, however many of our devices still support several methods of communication. If you would like to use the binary protocol, you can look at samples here.

!> Please note that the LWNX protocol is not documented for all devices. If you have question out support staff will be willing to assist.