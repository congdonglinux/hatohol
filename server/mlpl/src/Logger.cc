/*
 * Copyright (C) 2013 Project Hatohol
 *
 * This file is part of Hatohol.
 *
 * Hatohol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Hatohol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hatohol. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <cstdlib>
#include <stdarg.h>
#include <string>
#include <syslog.h>
using namespace std;
#include "Logger.h"
using namespace mlpl;
#include <string.h>
#include "StringUtils.h"

static const char* LogHeaders [MLPL_NUM_LOG_LEVEL] = {
	"BUG", "CRIT", "ERR", "WARN", "INFO", "DBG",
};

LogLevel Logger::m_currLogLevel = MLPL_LOG_LEVEL_NOT_SET;
pthread_rwlock_t Logger::m_rwlock = PTHREAD_RWLOCK_INITIALIZER;
bool Logger::syslogoutputFlag = true;
ReadWriteLock Logger::lock;
const char *Logger::LEVEL_ENV_VAR_NAME = "MLPL_LOGGER_LEVEL";
const char *Logger::MLPL_LOGGER_FLAGS = "MLPL_LOGGER_FLAGS";
bool Logger::syslogConnected = false;
bool Logger::extraInfoFlag[256];

// ----------------------------------------------------------------------------
// Public methods
// ----------------------------------------------------------------------------
void Logger::log(LogLevel level, const char *fileName, int lineNumber,
                 const char *fmt, ...)
{
	string extraInfoString = createExtraInfoString();
	string header = createHeader(level, fileName, lineNumber, extraInfoString);

	va_list ap;
	va_start(ap, fmt);
	string body = StringUtils::vsprintf(fmt, ap);
	va_end(ap);

	fprintf(stderr, "%s%s", header.c_str(), body.c_str());

	lock.readLock();
	if (syslogoutputFlag) {
		connectSyslogIfNeeded();
		lock.unlock();
		syslog(LOG_INFO, "%s%s", header.c_str(), body.c_str());
	} else {
		lock.unlock();
	}
}

// ----------------------------------------------------------------------------
// Protected methods
// ----------------------------------------------------------------------------
bool Logger::shouldLog(LogLevel level)
{
	bool ret = false;
	pthread_rwlock_rdlock(&m_rwlock);
	if (m_currLogLevel == MLPL_LOG_LEVEL_NOT_SET) {
		pthread_rwlock_unlock(&m_rwlock);
		setCurrLogLevel();
		pthread_rwlock_rdlock(&m_rwlock);
	}
	if (level <= m_currLogLevel)
		ret = true;
	pthread_rwlock_unlock(&m_rwlock);
	return ret;
}

void Logger::enableSyslogOutput(void)
{
	lock.writeLock();
	syslogoutputFlag = true;
	lock.unlock();
}

void Logger::disableSyslogOutput(void)
{
	lock.writeLock();
	syslogoutputFlag = false;
	lock.unlock();
}

void Logger::setCurrLogLevel(void)
{
	pthread_rwlock_wrlock(&m_rwlock);
	if (m_currLogLevel != MLPL_LOG_LEVEL_NOT_SET) {
		pthread_rwlock_unlock(&m_rwlock);
		return;
	}

	char *env = getenv(LEVEL_ENV_VAR_NAME);
	if (!env) {
		m_currLogLevel = MLPL_LOG_INFO;
		pthread_rwlock_unlock(&m_rwlock);
		return;
	}
	string envStr = env;
	if (envStr == "DBG")
		m_currLogLevel = MLPL_LOG_DBG;
	else if (envStr == "INFO")
		m_currLogLevel = MLPL_LOG_INFO;
	else if (envStr == "WARN")
		m_currLogLevel = MLPL_LOG_WARN;
	else if (envStr == "ERR")
		m_currLogLevel = MLPL_LOG_ERR;
	else if (envStr == "CRIT")
		m_currLogLevel = MLPL_LOG_CRIT;
	else if (envStr == "BUG")
		m_currLogLevel = MLPL_LOG_BUG;
	else {
		log(MLPL_LOG_WARN, __FILE__, __LINE__,
		    "Unknown level: %s\n", env);
		m_currLogLevel = MLPL_LOG_INFO;
	}
	pthread_rwlock_unlock(&m_rwlock);
}

void Logger::connectSyslogIfNeeded(void)
{
	if (syslogConnected)
		return;
	openlog(NULL, LOG_CONS | LOG_PID, LOG_USER);
	syslogConnected = true;
}

string Logger::createHeader(LogLevel level, const char *fileName,
                            int lineNumber, string extraInfoString)
{
	return StringUtils::sprintf("%s[%s] <%s:%d> ", extraInfoString.c_str(),
	                            LogHeaders[level], fileName, lineNumber);
}

string Logger::createExtraInfoString(void)
{
	string extraInfoString = "";
	if (extraInfoFlag['C'])
		addCurrentTime(extraInfoString);
	if (extraInfoFlag['P'])
		addProcessId(extraInfoString);
	if (extraInfoFlag['T'])
		addThreadId(extraInfoString);

	return extraInfoString;
}

void Logger::setExtraInfoFlag(const char *extraInfoArg)
{
	if (extraInfoArg == NULL)
		return;
	for (const char *c = extraInfoArg; *c; c++) {
		const int idx = *c;
		extraInfoFlag[idx] = true;
	}
}
