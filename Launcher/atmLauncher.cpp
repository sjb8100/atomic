﻿#include "launcherPCH.h"

istImplementOperatorNewDelete();

namespace atm {


void glob( const char *path, const char *filter_regex, const std::function<void (const std::string &file)> &f)
{
    try {
        std::regex filter(filter_regex);
        Poco::DirectoryIterator end;
        for(Poco::DirectoryIterator i(Poco::Path((const char*)path)); i!=end; ++i) {
            if(std::regex_search(i->path(), filter)) {
                f(i->path());
            }
        }
    }
    catch(...) {
    }
}

template<class T, class F>
inline void each(T &v, const F &f) {
    std::for_each(v.begin(), v.end(), f);
}


class Launcher
{
private:
    ist::vector<HMODULE> m_commondlls;
    HMODULE m_enginedll;

public:
    Launcher() : m_commondlls(), m_enginedll()
    {
        // 環境変数 PATH に Binaries/Common を追加し、dll サーチパスに加える
        {
            std::string path;
            //path.resize(1024*64);
            //DWORD ret = ::GetEnvironmentVariableA("PATH", &path[0], path.size());
            {
                char path_to_this_module[MAX_PATH + 1];
                HMODULE mod = 0;
                ::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&glob, &mod);
                DWORD size = ::GetModuleFileNameA(mod, path_to_this_module, sizeof(path_to_this_module));
                for (int i = size - 1; i >= 0; --i) {
                    if (path_to_this_module[i] == '\\') {
                        path_to_this_module[i] = '\0';
                        break;
                    }
                }
                path += path_to_this_module;
                path += "\\Binaries\\Common;";
            }
            {
                char currentdir[MAX_PATH + 1];
                GetCurrentDirectoryA(sizeof(currentdir), currentdir);
                path += currentdir;
                path += "\\Binaries\\Common;";
            }
            ::SetEnvironmentVariableA("PATH", path.c_str());
        }

#ifdef ist_env_Master
        const char *engine = "Binaries\\atomic_engine.dll";
#elif defined(ist_env_Debug)
        const char *engine = "Binaries\\atomic_engine_dbg.dll";
#else
        const char *engine = "Binaries\\atomic_engine_dev.dll";
#endif
        m_enginedll = ::LoadLibraryA(engine);
    }

    ~Launcher()
    {
        each(m_commondlls, [&](HMODULE dll){ ::FreeLibrary(dll); });
    }

    int32 run(int argc, char* argv[])
    {
        typedef int (*EntryPoint)(int argc, char *argv[]);
        if(EntryPoint e = (EntryPoint)::GetProcAddress(m_enginedll, "atmMain")) {
            return e(argc, argv);
        }
        return -1;
    }
};
} // namespace atm
using namespace atm;

int istmain(int argc, char* argv[])
{
    Launcher *launcher = istNew(Launcher)();
    int32 ret = launcher->run(argc, argv);
    istDelete(launcher);
    return ret;
}
