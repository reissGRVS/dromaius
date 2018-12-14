#include "registers.h"

Word & RegisterPair::word() {
	return reg.wordReg;
}

Byte & RegisterPair::first() {
	return reg.byteReg[1];
}

Byte & RegisterPair::second() {
	return reg.byteReg[0];
}
