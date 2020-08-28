# ESP-Watertap-Remote
The Watertap Remote Controll connects via WiFi to the mqtt broker and sends a message to the Watertap.
To safe power in the remote controlls battery I let the ESP go into deepsleep. It is reset by a ATTiny85.
See my repository: https://github.com/ingeninge/ATTINY85wecktESP8266
It will then open or close the valve for the garden water depending on the key pressed.
You can find my Garden Water Tap System in my lately released repository
https://github.com/ingeninge/Wasserhahn
Wasserhahn stands for water tap. It's basicly a ESP-8266 with two relais witch drive a motor valve to open/close the garden water tap.
