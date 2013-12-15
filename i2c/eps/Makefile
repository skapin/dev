
TARGET = /dev/ttyACM0
BOARD_TYPE = mega2560
COMPILER = ino

build:
	${COMPILER} build -m ${BOARD_TYPE} -f=-DBOARD_ID=-${BOARD_ID}

up:
	${COMPILER} upload -m ${BOARD_TYPE} -p ${TARGET}

serial:
	${COMPILER} serial -b 19200 -p ${TARGET}