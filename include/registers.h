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
		unsigned char _first = 0;
		unsigned char _second = 0;
		Byte firstReg;
		Byte secondReg;
		Word wordReg;
		
};