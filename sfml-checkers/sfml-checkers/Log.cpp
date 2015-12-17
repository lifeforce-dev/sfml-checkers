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

void AppendLoggingInfoToLog(const std::string& fileName, int lineNumber, std::string& messageOut)
{
	// Remove full path from the filename.
	std::string strippedFilename;
	size_t i = fileName.rfind('\\');
	if (i != std::string::npos)
	{
		strippedFilename = fileName.substr(i + 1, fileName.length());
	}

	std::stringstream ss;
	ss << messageOut;
	ss << " <" << strippedFilename;
	ss << "> (" << lineNumber << ")\n";
	messageOut = ss.str();
}

//==============================================================================

} // anonymous namespace

void Logger::LogDebugMessage(const std::string& message, LogSink messageType,
	const char* fileName, int lineNumber)
{
	if (message.empty())
		return;

	std::string guardedMessage = message;
	AppendLoggingInfoToLog(fileName, lineNumber, guardedMessage);

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
