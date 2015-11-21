//---------------------------------------------------------------
//
// Log.cpp
//

#include "Log.h"
#include <algorithm>

#define NOMINMAX
#include <windows.h>

namespace {

//==============================================================================

const int s_maxLength = 70;

enum MessageType
{
	DEBUG_WINDOW,
	CONSOLE
};

void WrapMessage(std::string& message)
{
	// No need to wrap.
	if (message.empty() || message.length() < s_maxLength)
		return;

	std::stringstream ss;
	while (message.length() > s_maxLength)
	{
		// Split message into chunk separated by new lines.
		std::string chunk = message.substr(0, s_maxLength);
		message.erase(0, s_maxLength);

		ss << chunk;
		ss << '\n';
	}

	// Push the remaining bits of the message into the buffer.
	ss << message;
	message = ss.str();
}

void AddMessageGuards(std::string& message)
{
	if (message.empty())
		return;

	std::stringstream ss;

	ss << '\n';
	for (int i = 0; i <= std::min(static_cast<int>(message.length()), s_maxLength); ++i)
	{
		ss << '-';
	}
	ss << '\n';

	std::string guard = ss.str();

	ss << message << guard;
	message = ss.str();
}

void AppendLoggingInfoToLog(std::string& message)
{
	std::stringstream ss;
	ss << message;
	ss << " " << __FILE__;
	ss << " (" << __LINE__ << ")";
	message = ss.str();
}

void LogMessageToConsole(const std::string& message)
{
	Logger::LogDebugMessage(message, Logger::CONSOLE);
}

void LogMessageToDebugWindow(const std::string& message)
{
	Logger::LogDebugMessage(message, Logger::DEBUG_WINDOW);
}

//==============================================================================

} // anonymous namespace

void Logger::LogDebugMessage(const std::string& message, MessageType messageType)
{
	if (message.empty())
		return;

	std::string guardedMessage = message;
	AppendLoggingInfoToLog(guardedMessage);
	WrapMessage(guardedMessage);
	AddMessageGuards(guardedMessage);

	switch (messageType)
	{
	case DEBUG_WINDOW:
		OutputDebugString(guardedMessage.c_str());
		break;
	case CONSOLE:
		std::cout << guardedMessage;
		break;
	default:
		break;
	}
}
