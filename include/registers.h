#pragma once
#include "forwarddecls.h"
#include "bytes/byte.h"
#include "bytes/word.h"

class RegisterPair{
	public:
		RegisterPair() :
		firstReg(&_first), secondReg(&_second), wordReg(&_first, &_second)
		{}
		Word & word(){return wordReg;}
		Byte & first(){return firstReg;}
		Byte & second(){return secondReg;}
		
	private:
		uint8_t _first = 0;
		uint8_t _second = 0;
		Byte firstReg;
		Byte secondReg;
		Word wordReg;
		
};