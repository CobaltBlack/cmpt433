// Interface to read analog input voltage 0 on the BeagleBone
// Represents the potentiometer (pot)

#ifndef POT_H_
#define POT_H_

void Pot_initialize();
void Pot_shutdown();

int Pot_getVoltage0Reading();

#endif /* POT_H_ */
