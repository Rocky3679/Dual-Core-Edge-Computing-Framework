# Troubleshooting Guide

## Overview

This document provides common issues that may occur during hardware setup, firmware execution, sensor interfacing, and system operation, along with recommended solutions.

---

# Hardware Issues

## ESP32 Not Powering ON

### Possible Causes

- Faulty USB cable
- Insufficient power supply
- Damaged ESP32 board

### Solution

- Use a data-capable USB cable.
- Verify the USB port.
- Check the onboard power LED.
- Test with another ESP32 board if necessary.

---

## OLED Display Not Working

### Possible Causes

- Incorrect SDA/SCL wiring
- Wrong I2C address
- Missing libraries

### Solution

- Verify SDA and SCL connections.
- Scan the I2C bus using an I2C Scanner sketch.
- Confirm the display address (commonly `0x3C`).
- Install the required libraries.

---

## Relay Module Not Switching

### Possible Causes

- Incorrect GPIO assignment
- Faulty relay module
- Insufficient supply voltage

### Solution

- Verify the relay control pin.
- Test the relay independently.
- Check the power supply connections.

---

# Sensor Issues

## Voltage Sensor Reading Incorrect Values

### Possible Causes

- Loose wiring
- Improper calibration
- Electrical noise

### Solution

- Check all wiring connections.
- Recalibrate the sensor.
- Keep sensor wiring away from high-current cables.

---

## Current Sensor Reading Incorrect Values

### Possible Causes

- Incorrect sensor orientation
- Improper calibration
- Noise interference

### Solution

- Verify sensor polarity.
- Recalibrate the sensor.
- Improve grounding.

---

# Software Issues

## Upload Failed

### Possible Causes

- Wrong COM port
- Missing ESP32 board package
- USB communication failure

### Solution

- Select the correct COM port.
- Install the ESP32 Board Package.
- Restart the Arduino IDE.
- Reconnect the USB cable.

---

## Compilation Errors

### Possible Causes

- Missing libraries
- Incorrect board selection
- Syntax errors

### Solution

- Install required libraries.
- Select **ESP32 Dev Module**.
- Verify the source code before compiling.

---

# Communication Issues

## Serial Monitor Not Displaying Data

### Possible Causes

- Incorrect baud rate
- Program not running
- Wrong COM port

### Solution

- Set the baud rate correctly.
- Verify the firmware upload.
- Select the correct serial port.

---

# System Performance Issues

## Slow System Response

### Possible Causes

- Excessive processing
- Blocking delays
- Poor task scheduling

### Solution

- Use non-blocking timing (`millis()`).
- Minimize unnecessary delays.
- Optimize task execution.

---

## Incorrect Power Factor Calculation

### Possible Causes

- Sensor calibration error
- Noisy measurements
- Incorrect algorithm implementation

### Solution

- Recalibrate sensors.
- Verify electrical connections.
- Validate the calculation logic.

---

# Preventive Maintenance

- Inspect wiring regularly.
- Tighten loose connections.
- Clean dust from the hardware.
- Update firmware when improvements are made.
- Test the relay and sensors periodically.

---

# Best Practices

- Use regulated power supplies.
- Keep analog and power wiring separate.
- Verify sensor calibration before testing.
- Follow electrical safety procedures.
- Maintain proper documentation for future maintenance.