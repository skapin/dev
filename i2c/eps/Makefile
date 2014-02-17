
TARGET = /dev/ttyACM1
BOARD_TYPE = mega2560
COMPILER = ino
BOARD_ID=-1

build:
	${COMPILER} build -m ${BOARD_TYPE} -f=-DBOARD_ID=${BOARD_ID}

clean:
	${COMPILER} clean

up:
	${COMPILER} upload -m ${BOARD_TYPE} -p ${TARGET}

serial:
	${COMPILER} serial -b 115200 -p ${TARGET}