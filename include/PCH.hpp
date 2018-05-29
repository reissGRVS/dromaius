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