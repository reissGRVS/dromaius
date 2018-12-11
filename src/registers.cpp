#include "registers.h"

Word RegisterPair::getWord() const{
	return reg.wordReg;
}

Byte RegisterPair::getFirst()const {
	return reg.byteReg[0];
}

Byte RegisterPair::getSecond() const{
	return reg.byteReg[1];
}

void RegisterPair::setWord(const Word w){
	reg.wordReg = w;
}

void RegisterPair::setFirst(const Byte b){
	reg.byteReg[0] = b;
}

void RegisterPair::setSecond(const Byte b){
	reg.byteReg[1] = b;
}