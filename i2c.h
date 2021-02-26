#ifndef DISPLAY_H
#define	DISPLAY_H

void i2c_WriteSerial(int addr, uint8_t *data, int length);
void i2c_ReadSerial(int addr, uint8_t *data, int length);

#endif	/* DISPLAY_H */

