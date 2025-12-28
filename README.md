# jkcmd

**jkcmd** is a simple command-line utility which reads data from a JK BMS through RS485 and prints it to stdout in readable format.

It uses the same type of request as the "JK-BMS-MONITOR" Windows application does.

Multiple devices can be addressed on the same bus. Either RS485-1 (UART1) or the "parallel" RS485-2 (UART2) can be used. In the latter case, all devices on the bus must be set as slave (address 1-15).

Use protocol **001 - BMS RS485 Modbus**.
