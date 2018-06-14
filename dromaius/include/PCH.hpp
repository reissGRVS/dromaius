/// Utilities includes

#include <cstdlib>
// system, getenv, malloc, calloc, realloc, free, atof, atoi, abs

#include <csignal>
// signal handlers

#include <functional>

#include <utility>
// pair, make_pair

#include <ctime>

#include <chrono>

#include <initializer_list>

/// Numeric limits includes

#include <limits>

/// Error handling includes

#include <cassert>

/// Strings includes

#include <string>

/// Containers includes

#include <vector>

#include <list>

#include <set>

#include <unordered_set>

#include <map>

#include <unordered_map>

#include <queue>

#include <array>

/// Algorithms includes

#include <algorithm>

/// Iterators includes

#include <iterator>

/// Numerics includes

#include <cmath>

#include <random>

/// IO includes

#include <ios>

#include <iostream>

#include <fstream>

#include <cstdio>

#include <sstream>

/// Regex includes

#include <regex>

/// DBG Macros

#define DBG_SEVERE(msg) { \
	std::stringstream ss; \
	ss << msg; \
	std::cout << "\033[1;31m! SEVERE  -\033[0m"; \
	std::cout << " " << ss.str() << std::endl; }

#define DBG_WARNING(msg) { \
	std::stringstream ss; \
	ss << msg; \
	std::cout << "\033[1;33m! WARNING  -\033[0m"; \
	std::cout << " " << ss.str() << std::endl; }

#define DBG_INFO(msg) { \
	std::stringstream ss; \
	ss << msg; \
	std::cout << "\033[1;32m! INFO  -\033[0m"; \
	std::cout << " " << ss.str() << std::endl; }