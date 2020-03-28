# ESP32 (ESP-IDF) TASK PROGRAMMER LIBRARY
 
## Target:

Defines basic preconfigured patterns (diary, weekly) and temperature settings.

Creates a weekly program (that can be customized externally. TODO: interface)

Compares actual date/time to target time patter/task action and triggers action (temperature target in this case) 


## Requirements:

  Wifi lib to get date/time via SNTP.


## TODO: 

Interface to permit creation of customized time setting patterns.

Option 1: External function to detect actual temperature target based on actual program in operation and actual time. Requires polling.

Option 2: Same functionality triggered via timer and event.

Important: make it robust, to detect resets (when time is lost), and avoid misoperation, going to a safe position.
