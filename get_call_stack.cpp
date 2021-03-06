#include "get_call_stack.hpp"

#include "lengthof.hpp"
#include "string.hpp"

#ifdef _WIN32
# include "windows.h"
# include "dbghelp.h"
#endif


namespace putils {
	std::string getCallStack() noexcept {
		std::string ret;
#ifdef _WIN32
		const auto process = GetCurrentProcess();
		SymInitialize(process, nullptr, true);

		void * stack[128];
		const auto frames = CaptureStackBackTrace(0, (DWORD)putils::lengthof(stack), stack, nullptr);

		char symbolBuffer[sizeof(SYMBOL_INFO) + 256];
		auto symbol = (SYMBOL_INFO *)symbolBuffer;
		symbol->MaxNameLen = 256;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

		for (int i = 0; i < frames; i++) {
			SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

			DWORD  displacement;
			IMAGEHLP_LINE64 line;
			SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &displacement, &line);

			static constexpr auto stackFramesToIgnore = 5;
			if (i >= stackFramesToIgnore) {
				const putils::string<256> s("\t %i: %s - (l.%i)", frames - i - 1, symbol->Name, line.LineNumber);
				if (!ret.empty())
					ret += '\n';
				ret += s;
			}
		}
#endif
		return ret;
	}
}
