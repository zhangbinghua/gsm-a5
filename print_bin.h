#ifndef _PIRNT_BIN_H_
#define _PIRNT_BIN_H_

extern int covert_hex_to_binary(unsigned long *bitmap, int *val, unsigned int len);
extern int printIntBinary(int *val, int len);
extern int printUlongBinary(unsigned long *val, int len);

#endif		/* _PIRNT_BIN_H_ */
