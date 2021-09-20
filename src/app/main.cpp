#include "main.prerequisites.hpp"

#if defined(WINDOWS_RELEASE_MAIN)
INT WINAPI
WinMain([[maybe_unused]] HINSTANCE hInst, HINSTANCE, [[maybe_unused]] LPSTR strCmdLine, INT)
#else
int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
#endif
{
#if defined(WINDOWS_RELEASE_MAIN)
    int    argc              = __argc;
    char** argv              = __argv;
    bool   is_parent_console = false;
    if (std::getenv("FORCE_ATOMICDEX_CONSOLE") != nullptr)
    {
        if (is_parent_console = (AttachConsole(ATTACH_PARENT_PROCESS) != 0); is_parent_console)
        {
            SPDLOG_INFO("Attaching console to the parent process");
            freopen("conin$", "r", stdin);
            freopen("conout$", "w", stdout);
            freopen("conout$", "w", stderr);
        }
        else if (static_cast<bool>(AllocConsole()))
        {
            SPDLOG_INFO("Creating the console and redirecting output");
            freopen("conin$", "r", stdin);
            freopen("conout$", "w", stdout);
            freopen("conout$", "w", stderr);
        }
    }
#endif

    int res = 0;
    //! run app
    try
    {
        res = run_app(argc, argv);
    }
    catch (const std::exception& error)
    {
        SPDLOG_ERROR("Exception caught: {}", error.what());
#if defined(linux) || defined(__APPLE__)
        SPDLOG_ERROR("stacktrace: {}", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
#endif
    }

#if defined(WINDOWS_RELEASE_MAIN)
    if (std::getenv("FORCE_ATOMICDEX_CONSOLE") != nullptr)
    {
        if (!is_parent_console)
        {
            system("pause");
        }
    }
#endif
    return res;
}
