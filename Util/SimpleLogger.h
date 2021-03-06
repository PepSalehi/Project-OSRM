/*

Copyright (c) 2013, Project OSRM, Dennis Luxen, others
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef SIMPLE_LOGGER_H_
#define SIMPLE_LOGGER_H_

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

#include <ostream>
#include <iostream>

enum LogLevel { logINFO, logWARNING, logDEBUG };
static	boost::mutex logger_mutex;

class LogPolicy : boost::noncopyable {
public:

	void Unmute() {
		m_is_mute = false;
	}

	void Mute() {
		m_is_mute = true;
	}

	bool IsMute() const {
		return m_is_mute;
	}

    static LogPolicy & GetInstance() {
	    static LogPolicy runningInstance;
    	return runningInstance;
    }

private:
	LogPolicy() : m_is_mute(true) { }
	bool m_is_mute;
};

class SimpleLogger {
public:
	SimpleLogger() : level(logINFO) { }

    std::ostringstream& Write(LogLevel l = logINFO) {
    	try {
			boost::mutex::scoped_lock lock(logger_mutex);
			level = l;
			os << "[";
			   	switch(level) {
				case logINFO:
		    		os << "info";
		    		break;
				case logWARNING:
		    		os << "warn";
		    		break;
				case logDEBUG:
#ifndef NDEBUG
		    		os << "debug";
#endif
		    		break;
				default:
		    		BOOST_ASSERT_MSG(false, "should not happen");
		    		break;
			}
			os << "] ";
		} catch (...) { }
	   	return os;
   }

	virtual ~SimpleLogger() {
		   	if(!LogPolicy::GetInstance().IsMute()) {
		   	switch(level) {
				case logINFO:
					std::cout << os.str() << std::endl;
					break;
				case logWARNING:
					std::cerr << os.str() << std::endl;
					break;
				case logDEBUG:
#ifndef NDEBUG
					std::cout << os.str() << std::endl;
#endif
					break;
				default:
					BOOST_ASSERT_MSG(false, "should not happen");
					break;
			}
	   	}
	}

private:
	LogLevel level;
	std::ostringstream os;
};

#endif /* SIMPLE_LOGGER_H_ */
