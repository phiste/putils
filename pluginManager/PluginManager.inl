#include "PluginManager.hpp"

namespace putils {
	template<typename ...P>
	void PluginManager::rescanDirectory(const char * path, const char * toExecute, P && ...params) noexcept {
#ifdef _WIN32
		static constexpr auto extension = ".dll";
#else
		static constexpr auto extension = ".so";
#endif

		putils::Directory d(path);
		d.for_each([&](const putils::Directory::File & f) noexcept {
			std::string_view view = f.fullPath;
			const auto fileExt = view.substr(view.find_last_of('.'));

			if (!f.isDirectory && fileExt == extension && _libraries.find(f.fullPath) == _libraries.end()) {
				auto plugin = LibraryFactory::make(f.fullPath);
				_libraries[f.fullPath] = plugin;
				if (toExecute != nullptr) {
					const auto func = plugin->loadMethod<void, P...>(toExecute);
					if (func != nullptr)
						func(FWD(params)...);
				}
			}
		});
	}

	template<size_t MaxReturns, typename T, typename ...P>
	putils::vector<T, MaxReturns> PluginManager::rescanDirectoryWithReturn(const char * path, const char * toExecute, P && ...params) noexcept {
#ifdef _WIN32
		static constexpr auto extension = ".dll";
#else
		static constexpr auto extension = ".so";
#endif
		putils::vector<T, MaxReturns> ret;

		putils::Directory d(path);
		d.for_each([&](const putils::Directory::File & f) noexcept {
			if (ret.full())
				return;

			std::string_view view = f.fullPath;
			const auto fileExt = view.substr(view.find_last_of('.'));

			if (!f.isDirectory && fileExt == extension && _libraries.find(f.fullPath) == _libraries.end()) {
				auto plugin = LibraryFactory::make(f.fullPath);
				_libraries[f.fullPath] = plugin;
				if (toExecute != nullptr) {
					const auto func = plugin->loadMethod<T, P...>(toExecute);
					if (func != nullptr)
						ret.push_back(func(FWD(params)...));
				}
			}
			});

		return ret;
	}

	template<typename ...P>
	void PluginManager::execute(const std::string & name, P && ...params) noexcept {
		for (const auto & [_, plugin] : _libraries) {
			auto func = plugin->loadMethod<void, P...>(name);
			if (func != nullptr)
				func(std::forward<P>(params)...);
		}
	}

	template<size_t MaxReturns, typename T, typename ...P>
	putils::vector<T, MaxReturns> PluginManager::executeWithReturn(const std::string & name, P && ...params) noexcept {
		putils::vector<T, MaxReturns> ret;

		for (const auto & [_, plugin] : _libraries) {
			auto func = plugin->loadMethod<T, P...>(name);
			if (func != nullptr) {
				ret.push_back(func(std::forward<P>(params)...));
				if (ret.full())
					return;
			}
		}

		return ret;
	}
}
