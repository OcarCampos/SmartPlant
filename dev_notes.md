# SmartPlant Refactoring Notes

This document outlines potential improvements for the `SmartPlant.ino` sketch to enhance its functionality, efficiency, and reliability.

## Key Areas for Improvement

### 1. Critical Bug: `watering_check()` Not Called
- **Issue**: The `watering_check()` function, which handles the automated watering logic, is defined but never called within the main `loop()`.
- **Impact**: The core feature of automated watering is currently non-functional.
- **Solution**: Add a call to `watering_check()` inside the `loop()` function.

### 2. Replace Blocking `delay()` Calls
- **Issue**: The code extensively uses `delay()`, which blocks the microcontroller from performing other tasks, making the system unresponsive.
- **Impact**: While a `delay()` is active, the Arduino cannot read sensors, update the LCD, or check the WiFi connection.
- **Solution**: Refactor the code to use a non-blocking timing mechanism with the `millis()` function. This will allow the system to handle multiple tasks concurrently.

### 3. Simplify Repetitive Code
- **Issue**: The `watering_check()` function contains repeated blocks of code for its three watering cycles.
- **Impact**: This makes the code harder to read and maintain. If the watering logic needs to be changed, it must be updated in three separate places.
- **Solution**: Use a `for` loop to iterate through the watering cycles. This will make the code more concise and easier to manage.

### 4. Improve LCD Display Logic
- **Issue**: The current implementation cycles through each sensor reading on the LCD with a delay between each screen. This is slow and inefficient.
- **Impact**: The user has to wait to see all the relevant data.
- **Solution**: Redesign the LCD layout to display multiple sensor readings on the screen simultaneously. The display can be updated periodically without long delays.

### 5. Add Sensor Error Handling
- **Issue**: The code does not validate the readings from the DHT11 sensor. Sensors can occasionally return invalid data (e.g., `NaN` - Not a Number).
- **Impact**: Invalid data could lead to incorrect logic or display errors.
- **Solution**: Add checks to ensure the sensor readings for temperature and humidity are valid before using them.

### 6. General Readability and Maintenance
- **Issue**: While the code is functional, it could benefit from better organization.
- **Solution**:
    - Group related constants and variables together.
    - Break down large functions into smaller, more specialized helper functions.
    - Add more descriptive comments to explain complex parts of the code.
