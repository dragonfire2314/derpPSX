#ifndef DERP_LOGGER
#define DERP_LOGGER


#include <iostream>
#include <string>
#include <stdarg.h>
#include <map>
#include <iomanip>

#define DerpLog(level, type, fmt, ...) Derp_derpLogMessage(level, type, __LINE__, __FILE__, fmt"\n", __VA_ARGS__)

enum LEVEL { INFO, WARN, ERROR, FATAL, GOOD, OTHER };

std::map<int, std::pair<bool, std::string>> _derp_type_map_private;

void Derp_derpLogMessage(int level, int type, int line_number, const char* file_name, const char *fmt, ...) {

	va_list arg;
	std::string file_name_fixed;
	std::string str_level;
	std::string str_type;

	//Get level type
	switch (level) 
	{
	case INFO: 
		str_level = "INFO";
		break;
	case WARN: 
		str_level = "WARN";
		break;
	case ERROR:
		str_level = "ERROR";
		break;
	case FATAL: 
		str_level = "FATAL";
		break;
	case GOOD: 
		str_level = "GOOD";
		break;
	case OTHER: 
		str_level = "OTHER";
		break;
	}

	//Check type
	str_type = _derp_type_map_private[type].second;

	//Remove the directory and just keep the file name
	//Find end of file
	int index = 0;
	while (file_name[index] != 0) index++;
	//Copy to new array until the \ is hit then stop
	index--;
	while (file_name[index] != '\\') {
		file_name_fixed += file_name[index];
		index--;
	}
	std::reverse(file_name_fixed.begin(), file_name_fixed.end());

	//Print message header
	if (_derp_type_map_private[type].first) {
		std::cout << std::setw(6) << str_type << " " << std::setw(20) << file_name_fixed << ":" << line_number << std::setw(8) << str_level << " | ";

		va_start(arg, fmt);
		vprintf(fmt, arg);
		va_end(arg);
	}

#ifdef DEBUG
	fflush(log_file);
	fsync(fileno(log_file));
#endif
}

/*
returns true or false based on success
type_key - the key that will be used for the type
type_message - the text that is displayed for the type
*/
bool Derp_addType(int type_key, std::string type_message)
{
	//Check if key is already used
	return _derp_type_map_private.insert(std::pair<int, std::pair<bool, std::string>>(type_key, std::make_pair(true, type_message))).second;
}

void Derp_set_type(int type_key, bool mode) 
{
	_derp_type_map_private[type_key].first = mode;
}

#else

#endif // DERP_LOGGER