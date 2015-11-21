//---------------------------------------------------------------
//
// Log.h
//

#pragma once

#include <string>
#include <iostream>
#include <sstream>

namespace Logger {

//==============================================================================

enum MessageType
{
	DEBUG_WINDOW,
	CONSOLE
};

void LogDebugMessage(const std::string& message, MessageType messageType);

//==============================================================================

} // namespace Logger
