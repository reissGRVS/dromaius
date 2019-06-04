#include "spdlog/spdlog.h"
#include <cstdint>

class CPU;
class Byte;
class Word;

//A Tick is 1 CPU cycle
using Ticks = uint16_t;