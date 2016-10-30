#include "FileSystem.h"

#include "Parser.h"
#include "Application.h"

#ifdef TARGET_OS_UNIX_LIKE

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#endif

#ifdef TARGET_OS_WINDOWS

#include <Shlobj.h>

#endif

using namespace Game;

using namespace std;

const ApplicationId FileSystem::id{"filesystem"};

FileSystem::FileSystem(const CLI::Call& call) {
    namespace fs = boost::filesystem;

    string provided_path = Parser::from_map<CLI::Arguments, string>(call.arguments, "data path");
    if (provided_path.empty()) {
        data_path_ = call.application_run_path.parent_path() / fs::path{"data"};
    } else {
        data_path_ = fs::path{provided_path.begin(), provided_path.end()};
    }
    if (!fs::is_directory(data_path_)) {
        throw ApplicationError{id, string{"data path '"}+data_path_.string() + "' does not exist or is not a directory"};
    }

    modules_path_ = data_path_ / fs::path{"modules"};
    if (!fs::is_directory(modules_path_)) {
        throw ApplicationError{id, string{"modules path '"}+modules_path_.string() + "' does not exist or is not a directory"};
    }

#ifdef TARGET_OS_UNIX_LIKE
    struct passwd *pw = getpwuid(getuid());

    const char *homedir = pw->pw_dir;
    
    settings_path_ = fs::path{homedir} / fs::path{".spacegame"};
#endif
#ifdef TARGET_OS_WINDOWS
    
    //Deprecated but works for XP as well
    
    WCHAR path[MAX_PATH];
    HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
    SHGFP_TYPE_CURRENT, path);
    
    settings_path_ = fs::path{&path} / fs::path{"SpaceGame"};
    
#endif
}
