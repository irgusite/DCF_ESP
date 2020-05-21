# DCF_ESP
A little dcf signal generator with ntp synchronisation

## Hardware
This code is designed and tested for an esp 8266 (NodeMCU). It *should* be compatible with other arduino, wifi enables boards, but you would have to test it yourself.

## Installation
- Clone or download the code on your computer, open the DCF_esp.ino file in arduino. 
- Copy the `config_example.h` file to `config.h` and edit it to your needs (Access point, NTP server etc).
- Compile and upload it to your board.

This code supports OTA updates via arduino. 

## Notes
- There aren't a lot of parameters, as I didn't need more, but don't hesitate to open an issue, or make a pull request :)
- I provide no connection scheme as it will depend on the clock it will be in. 

## Sources
- DCF-77 on [wikipedia ](https://fr.wikipedia.org/wiki/DCF77)
