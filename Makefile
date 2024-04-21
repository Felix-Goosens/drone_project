mega2560:
	(cd embedded; arduino-cli --fqbn arduino:avr:mega compile --clean --build-properties build.extra_flags=-DSERIAL_TX_BUFFER_SIZE=320,-DSERIAL_RX_BUFFER_SIZE=320 --libraries ./libs/ -u -p /dev/ttyACM0 mega2560)

mega2560_debug:
	(cd embedded; arduino-cli --fqbn arduino:avr:mega compile --clean --build-properties build.extra_flags=-DDEBUG --libraries ./libs/ -u -p /dev/ttyACM0 mega2560)
	
esp8266:
	(cd embedded; arduino-cli --fqbn esp8266:esp8266:generic compile --clean --libraries ./libs/ -u -p /dev/ttyACM0 esp8266)

esp32:
	(cd embedded; arduino-cli --fqbn esp32:esp32:esp32cam compile --clean --libraries ./libs/ -u -p /dev/ttyUSB0 esp32)

screen_esp32:
	arduino-cli monitor --fqbn esp32:esp32:esp32cam -p /dev/ttyUSB0

screen:
	screen /dev/ttyACM0 9600

mpu9250:
	(cd embedded; arduino-cli --fqbn arduino:avr:mega compile --clean --libraries ./mpu9250/invensense-imu/ -u -p /dev/ttyACM0 mpu9250)
