#include "Debug.h"

void Debug::Log(const char* message, Severity severity)
{
	std::string strMessage(message);
	std::string fullMessage = SeverityToString(severity) + strMessage + "\n";

	printf_s(fullMessage.c_str());
}

void Debug::Log(const std::string& message, Severity severity)
{
	std::string fullMessage = SeverityToString(severity) + message + "\n";

	printf_s(fullMessage.c_str());
}

const char* Debug::SeverityToString(Severity severity)
{
	switch (severity)
	{
		case Severity::INFO:
			return "[INFO] ";
		case Severity::WARN:
			return "[WARN] ";
		case Severity::ERR:
			return "[ERR] ";
		default:
			return "[INFO] ";
	}
}