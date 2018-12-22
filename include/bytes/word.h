#pragma once
#include "forwarddecls.h"

class Word{
	public:
		Word(unsigned char * first,unsigned char * second):
			firstReg(first), secondReg(second)
		{}

		Word(Byte first,Byte second):
			firstReg(first.value), secondReg(second.value)
		{}

		unsigned int val() const{
			return ((*firstReg << 8) + *secondReg); 
		}

		void set(const unsigned int val){
			*firstReg = val >> 8;
			*secondReg = val & 0xFF;
		}

		operator unsigned int() const {
			return val();
		}

		void add(unsigned int v){
			set(val()+v);
		}
		void operator+=(const Word& rhs){
			add(rhs.val());
		}
		void operator+=(const unsigned int rhs){
			add(rhs);
		}
    	//postfix
		unsigned int operator++( int ){
			unsigned int returnVal = val();
			add(1);
			return returnVal;
		}

		void subtract(unsigned int v){
			set(val()-v);
		}
		unsigned int operator--( int ){
			unsigned int returnVal = val();
			subtract(1);
			return returnVal;
		}

       	void operator=(const unsigned int & v){
    		set(v);
    	}
		void operator=(const Word & w){
    		set(w.val());
    	}
		
	private:
		//high
		unsigned char * firstReg;
		//low
		unsigned char * secondReg;
};