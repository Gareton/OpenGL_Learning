#ifndef HELPER_H
#define HELPER_H
#include <string>
#include <cstdlib>

namespace help {
	char buf[12];
	
	std::string itos(int val)
	{
		memset(buf, static_cast<char>(0), 12);
		_itoa_s(val, buf, 10);
		return std::string(buf);
	}
}


#endif
