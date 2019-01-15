#pragma once
#include "forwarddecls.h"

class Word{
	public:
		Word(uint8_t * first,uint8_t * second):
			firstReg(first), secondReg(second)
		{}

		Word(Byte first,Byte second):
			firstReg(first.value), secondReg(second.value)
		{}

		uint16_t val() const{
			return ((*firstReg << 8) + *secondReg); 
		}

		void set(const uint16_t val){
			*firstReg = val >> 8;
			*secondReg = val & 0xFF;
		}

		operator uint16_t() const {
			return val();
		}

		void add(uint16_t v){
			set(val()+v);
		}
		void operator+=(const Word& rhs){
			add(rhs.val());
		}
		void operator+=(const uint16_t rhs){
			add(rhs);
		}
    	//postfix
		uint16_t operator++( int ){
			uint16_t returnVal = val();
			add(1);
			return returnVal;
		}

		void subtract(uint16_t v){
			set(val()-v);
		}
		uint16_t operator--( int ){
			uint16_t returnVal = val();
			subtract(1);
			return returnVal;
		}

       	void operator=(const uint16_t & v){
    		set(v);
    	}
		void operator=(const Word & w){
    		set(w.val());
    	}
		
	private:
		//high
		uint8_t * firstReg;
		//low
		uint8_t * secondReg;
};