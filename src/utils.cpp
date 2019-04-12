#include "utils.hpp"

std::string to_string(uint32_t number)
{
	std::stringstream ss;
	ss << number;

	return ss.str();
}

std::string round(double d, int decimals)
{
	std::string result;
	std::stringstream sx;
	sx << std::fixed << d;
	result = sx.str();

	size_t pos = result.find('.');
	if (pos == std::string::npos)
		return result;
	else if (!decimals)
	{
		return result.substr(0, pos);
	}
	else
	{
		if (pos + decimals + 1 < result.size())
		{
			return result.substr(0, pos + decimals + 1);
		}
	}

	return result;
}