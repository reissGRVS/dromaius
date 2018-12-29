#pragma once
#include "forwarddecls.h"

//This would be nicer if I used polymorphism imo, however I dont want to have to manage the lifetime of Bytes at the moment or restructure, 
//possibly a TODO, could improve performance by minimizing construction of Byte copies, not sure what the move overhead is for unique_ptr though

enum ByteType {
	NORMAL,
	NO_WRITE,
	WRITE_RESET
};

class Byte{
	friend class Word;
    public:
    	Byte(unsigned char * val, ByteType bt = ByteType::NORMAL) : value(val), byteType(bt)  {}
    	
    	unsigned char val() const{
    		return *value;
    	}
    
    	virtual void set(const unsigned char val){
			switch(byteType){
				case ByteType::NORMAL:
					*value = val;
					break;
				case ByteType::NO_WRITE:
					spdlog::get("stderr")->info("Tried to write {:x} to protected byte", val);
					exit(0);
					break;
				case ByteType::WRITE_RESET:
					*value = 0;
					break;
			}

    	}
    
    	void operator=(const Byte & b){
    		set(b.val());
    	}
    	
       	void operator=(const unsigned char & b){
    		set(b);
    	}

		void operator|=(const unsigned char & b){
			set(val() | b);
		}

		void operator&=(const unsigned char & b){
			set(val() & b);
		}

		void operator^=(const Byte & b){
			set(val() ^ b.val());
		}

		Byte& operator~(){
			set(~val());
			return *this;
		}

		operator unsigned char() const {
			return *value; 
		}

		void add(unsigned int v){
			set(val()+v);
		}

		void operator+=(const Byte& rhs){
			add(rhs.val());
		}

		void operator+=(const unsigned char rhs){
			add(rhs);
		}

    	//postfix
		virtual unsigned char operator++( int ){

			unsigned int returnVal = val();
			if (byteType == ByteType::WRITE_RESET){
				//This is so it doesnt reset
				*value+=1;	
			}
			else{
				add(1);
			}
			return returnVal;
		}

		void subtract(unsigned char v){
			set(val()-v);
		}

		void operator-=(const Byte& rhs){
			subtract(rhs.val());
		}

		void operator-=(const unsigned char rhs){
			subtract(rhs);
		}

		unsigned char operator--( int ){
			unsigned int returnVal = val();
			subtract(1);
			return returnVal;
		}


		unsigned char getBit(unsigned char pos){
			return (val() >> pos) & 1;
		}

		void setBit(unsigned char pos, bool bitVal){
			unsigned char mask = 1 << pos;
			if (bitVal){
				set( val() | mask);
			}
			else {
				set( val() & ~mask);
			}
		}

	protected:
		unsigned char * value;
		const ByteType byteType;
};
