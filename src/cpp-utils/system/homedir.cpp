#include "homedir.h"
#include <sys/types.h>
#include <pwd.h>

namespace bf = boost::filesystem;
using std::string;

namespace cpputils {
    namespace system {

        HomeDirectory::HomeDirectory()
                :_home_directory(_get_home_directory()) {
        }

        HomeDirectory &HomeDirectory::singleton() {
            static HomeDirectory _singleton;
            return _singleton;
        }

        const bf::path &HomeDirectory::get() {
            return singleton()._home_directory;
        }

        bf::path HomeDirectory::_get_home_directory() {
            struct passwd* pwd = getpwuid(getuid());
            string homedir;
            if (pwd) {
                homedir = pwd->pw_dir;
            } else {
                // try the $HOME environment variable
                homedir = getenv("HOME");
            }
            if (homedir == "") {
                throw std::runtime_error("Couldn't determine home directory for user");
            }
            return homedir;
        }

        bf::path HomeDirectory::getXDGDataDir() {
            const char* xdg_data_dir = std::getenv("XDG_DATA_HOME");
            if (xdg_data_dir != nullptr) {
                return xdg_data_dir;
            }

            return cpputils::system::HomeDirectory::get() / ".local" / "share";
        }

        FakeHomeDirectoryRAII::FakeHomeDirectoryRAII(const boost::filesystem::path &fakeHomeDirectory)
                :_oldHomeDirectory(HomeDirectory::singleton()._home_directory) {
            HomeDirectory::singleton()._home_directory = fakeHomeDirectory;
        }

        FakeHomeDirectoryRAII::~FakeHomeDirectoryRAII() {
            // Reset to old (non-fake) value
            HomeDirectory::singleton()._home_directory = _oldHomeDirectory;
        }
    }
}
