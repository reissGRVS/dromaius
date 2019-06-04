#pragma once
#include "forwarddecls.h"
#include "memorymap.h"

//This would be nicer if I used polymorphism imo, however I dont want to have to manage the lifetime of Bytes at the moment or restructure, 
//possibly a TODO, could improve performance by minimizing construction of Byte copies, not sure what the move overhead is for unique_ptr though

enum ByteType {
	NORMAL,
	NO_WRITE,
	WRITE_RESET,
	DMA_SIGNAL,
	ROM_BANK_SELECT
};

class Byte{
	friend class Word;
    public:
    	Byte(uint8_t * val,  ByteType bt = ByteType::NORMAL, MemoryMap* mem = nullptr) : value(val), byteType(bt), memoryMap(mem)  {}
    	
    	uint8_t val() const{
    		return *value;
    	}
    
    	virtual void set(const uint8_t val){
			switch(byteType){
				case ByteType::NORMAL:
					*value = val;
					break;
				case ByteType::NO_WRITE:
					spdlog::get("stderr")->debug("Tried to write {:x} to protected byte", val);
					break;
				case ByteType::ROM_BANK_SELECT:
					memoryMap->setRomBank(val & 0x1F);
					break;
				case ByteType::WRITE_RESET:
					*value = 0;
					break;
				case ByteType::DMA_SIGNAL:
					*value = val;
					memoryMap->startDMA();
					break;
			}

    	}
    
    	void operator=(const Byte & b){
    		set(b.val());
    	}
    	
       	void operator=(const uint8_t & b){
    		set(b);
    	}

		void operator|=(const uint8_t & b){
			set(val() | b);
		}

		void operator&=(const uint8_t & b){
			set(val() & b);
		}

		void operator^=(const Byte & b){
			set(val() ^ b.val());
		}

		Byte& operator~(){
			set(~val());
			return *this;
		}

		operator uint8_t() const {
			return *value; 
		}

		void add(uint16_t v){
			set(val()+v);
		}

		void operator+=(const Byte& rhs){
			add(rhs.val());
		}

		void operator+=(const uint8_t rhs){
			add(rhs);
		}

    	//postfix
		//TODO: Find neater way of doing this
		virtual uint8_t operator++( int ){

			uint16_t returnVal = val();
			if (byteType == ByteType::WRITE_RESET){
				//This is so it doesnt reset
				*value+=1;	
			}
			else{
				add(1);
			}
			return returnVal;
		}

		void subtract(uint8_t v){
			set(val()-v);
		}

		void operator-=(const Byte& rhs){
			subtract(rhs.val());
		}

		void operator-=(const uint8_t rhs){
			subtract(rhs);
		}

		uint8_t operator--( int ){
			uint16_t returnVal = val();
			subtract(1);
			return returnVal;
		}


		uint8_t getBit(uint8_t pos){
			return (val() >> pos) & 1;
		}

		void setBit(uint8_t pos, bool bitVal){
			uint8_t mask = 1 << pos;
			if (bitVal){
				set( val() | mask);
			}
			else {
				set( val() & ~mask);
			}
		}

	protected:
		uint8_t * value;
		const ByteType byteType;
		MemoryMap* memoryMap;
};
