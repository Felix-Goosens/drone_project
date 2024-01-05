mega2560:
	(cd embedded; arduino-cli --fqbn arduino:avr:mega compile --libraries ./libs/ -u -p /dev/ttyACM0 mega2560)

mega2560_debug:
	(cd embedded; arduino-cli --fqbn arduino:avr:mega compile --build-properties build.extra_flags=-DDEBUG --libraries ./libs/ -u -p /dev/ttyACM0 mega2560)
	
esp8266:
	(cd embedded; arduino-cli --fqbn esp8266:esp8266:generic compile --libraries ./libs/ -u -p /dev/ttyACM0 esp8266)

esp32:
	(cd embedded; arduino-cli --fqbn esp32:esp32:esp32cam compile --libraries ./libs/ -u -p /dev/ttyUSB0 esp32)

screen:
	screen /dev/ttyACM0 9600
