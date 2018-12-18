#include <string>
#include "forwarddecls.h"
#include <functional>

struct Operation{
	const std::string mnemonic;
	const std::function<Ticks(CPU*)> action;
};