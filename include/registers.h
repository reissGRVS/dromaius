#pragma once
#include "forwarddecls.h"

class RegisterPair{
	public:
		RegisterPair(){

		}

		Word getWord() const;
		Byte getFirst() const; 
		Byte getSecond() const;

		void setWord(const Word w);
		void setFirst(const Byte b);
		void setSecond(const Byte b);

	private:

		//This may be broken by different endianness 
		union {
			Word wordReg;
			Byte byteReg[2];
		} reg;


};