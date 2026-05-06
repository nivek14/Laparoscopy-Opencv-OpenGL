/*
 * COPYRIGHT NOTICE, DISCLAIMER, and LICENSE:
 *
 * 
 * For the purposes of this copyright and license, "Contributing Authors"
 * is defined as the following set of individuals:
 *
 *    Carlos Augusto Dietrich (cadietrich@gmail.com)
 *
 * This library is supplied "AS IS".  The Contributing Authors disclaim 
 * all warranties, expressed or implied, including, without limitation, 
 * the warranties of merchantability and of fitness for any purpose. 
 * The Contributing Authors assume no liability for direct, indirect, 
 * incidental, special, exemplary, or consequential damages, which may 
 * result from the use of the this library, even if advised of the 
 * possibility of such damage.
 *
 * Permission is hereby granted to use, copy, modify, and distribute this
 * source code, or portions hereof, for any purpose, without fee, subject
 * to the following restrictions:
 *
 * 1. The origin of this source code must not be misrepresented.
 *
 * 2. Altered versions must be plainly marked as such and must not be 
 *    misrepresented as being the original source.
 *
 * 3. This Copyright notice may not be removed or altered from any source 
 *    or altered source distribution.
 *
 * The Contributing Authors specifically permit, without fee, and 
 * encourage the use of this source code as a component in commercial 
 * products. If you use this source code in a product, acknowledgment 
 * is not required but would be appreciated.
 *
 * 
 * "Software is a process, it's never finished, it's always evolving. 
 * That's its nature. We know our software sucks. But it's shipping! 
 * Next time we'll do better, but even then it will be shitty. 
 * The only software that's perfect is one you're dreaming about. 
 * Real software crashes, loses data, is hard to learn and hard to use. 
 * But it's a process. We'll make it less shitty. Just watch!"
 */

#if !defined(LOGGER_INCLUDED)
#define LOGGER_INCLUDED

#include <cstdio>
#include <string>
#include <iostream>

#if !defined(MY_NO_STDIO)

#if defined(__ANDROID__)
#define LOG_MESSAGE(...) ((void)__android_log_print(ANDROID_LOG_INFO, "my.LOGGER", __VA_ARGS__))
#define LOG_ERROR(...) ((void)__android_log_print(ANDROID_LOG_WARN, "my.LOGGER", __VA_ARGS__))

#else //#if defined(__ANDROID__)

inline std::string MyPrettyFunction(const std::string& functionSignature)
{
    size_t functionNameEndDelimiter = functionSignature.rfind("("),
        functionNameStartDelimiter = functionSignature.substr(0, functionNameEndDelimiter).rfind(" ") + 1;

    // BUG: (18-Jan-2021) "operator +="
    while ((functionNameEndDelimiter - functionNameStartDelimiter) <= 2)
        functionNameStartDelimiter = functionSignature.substr(0, functionNameStartDelimiter - 1).rfind(" ") + 1;

    return functionSignature.substr(functionNameStartDelimiter, functionNameEndDelimiter - functionNameStartDelimiter);
}

#if defined(_MSC_VER)
#define __MY_PRETTY_FUNCTION__ MyPrettyFunction(__FUNCSIG__)
#else //#if defined(_MSC_VER)
#define __MY_PRETTY_FUNCTION__ MyPrettyFunction(__PRETTY_FUNCTION__)
#endif //#if defined(_MSC_VER)

#define LOG_MESSAGE(message) printf("%s (%d): %s\n", __MY_PRETTY_FUNCTION__.c_str(), __LINE__, std::string(message).c_str())

#define LOG_ERROR() printf("%s (%d): An error has occurred.\n", __MY_PRETTY_FUNCTION__.c_str(), __LINE__)

#define HEALTH_CHECK(statement, returnValueIfFailed) if (statement) {\
    printf("%s (%d): An assertion failure has occurred.\n", __MY_PRETTY_FUNCTION__.c_str(), __LINE__);\
    return returnValueIfFailed;\
}
#endif //#if defined(__ANDROID__)

#else //!defined(MY_NO_STDIO)

#if defined(_MSC_VER)
#pragma warning(disable : 4390)
#endif //defined(_MSC_VER)

#define LOG_MESSAGE(message) 

#define LOG_ERROR() 

#define HEALTH_CHECK(statement, returnValueIfFailed) if (statement) {\
    return returnValueIfFailed;\
}
#endif //!defined(MY_NO_STDIO)

// (BEGIN OF) DEPRECATED: (03-Sep-2019)
//#include <vector>
//#include <string>
//
//#include <boost/circular_buffer.hpp>
//
//#include "Common.h"
//
//#define TRY_CATCH_BLOCK(statement, returnValueIfFailed) try {\
//    statement;\
//}\
//catch (std::exception& e) {\
//    CLogger::Instance().PushMessage((char *)__FILE__, __LINE__, e.what());\
//    return returnValueIfFailed;\
//}
//
//#define HEALTH_CHECK(statement, returnValueIfFailed) if (statement) {\
//    CLogger::Instance().PushError((char *)__FILE__, __LINE__);\
//    return returnValueIfFailed;\
//}
//
//#define LOG_ERROR() CLogger::Instance().PushError((char *)__FILE__, __LINE__)
//
//#define LOG_MESSAGE(message) CLogger::Instance().PushMessage((char *)__FILE__, __LINE__, message)
//
//class CLogger
//{
//    SINGLETON_DECLARATION(CLogger)
//
//public:
//	void PushError(char *file, int line);
//
//	void PushMessage(char *file, int line, const char *message);
//	void PushMessage(char *file, int line, const std::string& message);
//	void PushMessage(const char *message);
//    
//	void PrintErrors() const;
//
//	void PrintMessages() const;
//    void PrintMessages(std::vector<std::string>& errorMessageArray) const;
//	void PrintMessages(const std::string& fileName) const;
//
//    void Clear();
//
//	bool IsOk() const;
//
//private:
//	void AddMessage(const std::string& message);
//
//    std::string FormatMessage(char *file, int line, const char *message) const;
//
//protected:
//    boost::circular_buffer<std::string> m_messageArray;
//};
// (END OF) DEPRECATED: (03-Sep-2019)

#endif // #if !defined(LOGGER_INCLUDED)

