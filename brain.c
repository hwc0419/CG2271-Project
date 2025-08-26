#include "brain.h"

void parseCommand(uint8_t data, msg* command){
  command->leftMotor = (data >> 6) & 0x03;  // Extract bits [7:6]
  command->rightMotor = (data >> 4) & 0x03;  // Extract bits [5:4]
  command->leftSpinDir = (data >> 3) & 0x01;  // Extract bit [3]
  command->rightSpinDir = (data >> 2) & 0x01;  // Extract bit [2]
	command->isMoving = (data >> 1) & 0x01;
	command->isDone = (data >> 0) & 0x01;
}