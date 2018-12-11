#include <string>
#include "forwarddecls.h"
#include <functional>

struct Operation{
	const std::string mnemonic;
	const Ticks ticks;
	const std::function<void()> action;
};