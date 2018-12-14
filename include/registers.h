#pragma once
#include "forwarddecls.h"

class RegisterPair{
	public:
		RegisterPair(){
		}
 
		Word & word();
		Byte & first(); 
		Byte & second();

	private:

		//This may be broken by different endianness 
		union {
			Word wordReg;
			Byte byteReg[2];
		} reg;


};