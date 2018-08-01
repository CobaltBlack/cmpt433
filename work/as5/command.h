/*
 * command.h
 *
 *  Created on: Aug 1, 2018
 *      Author: eric
 */

#ifndef COMMAND_H_
#define COMMAND_H_

void Command_printHelpMessage(void);
void Command_serialRxIsrCallback(uint8_t rxByte);
void Command_doBackgroundSerialWork(void);

#endif /* COMMAND_H_ */
