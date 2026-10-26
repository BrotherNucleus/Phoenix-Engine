#include "Phoenix.h"

Logger* Logger::Inst;

Logger::Logger()
{
	Inst = this;
}

Logger::~Logger()
{
}
