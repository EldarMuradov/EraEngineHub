#pragma once
#include <string>
class Debug
{
public:
	enum class Severity
	{
		INFO = 0,
		WARN = (INFO + 1),
		ERR = (WARN + 1)
	};

public:
	static void Log(const char* message, Severity severity);

	static void Log(const std::string& message, Severity severity);

private:
	static inline const char* SeverityToString(Severity severity);
};

#define LOG_INFO(message) Debug::Log(message, Debug::Severity::INFO)
#define LOG_WARN(message) Debug::Log(message, Debug::Severity::WARN)
#define LOG_ERR(message) Debug::Log(message, Debug::Severity::ERR)