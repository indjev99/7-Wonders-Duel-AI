#include "listener_logger.h"

#include "game/lang.h"

#include <chrono>
#include <filesystem>

const std::string LOG_EXT = ".log";
const std::filesystem::path LOG_DIR("logs");

ListenerLogger::ListenerLogger(std::ostream& out)
    : out(out)
{}

ListenerLogger::ListenerLogger()
    : ListenerLogger(defaultOutFile)
{
    auto now = std::chrono::system_clock::now().time_since_epoch();
    long long timeSecs = std::chrono::duration_cast<std::chrono::seconds>(now).count();

    std::filesystem::create_directory(LOG_DIR);
    std::filesystem::path fileName(std::to_string(timeSecs) + LOG_EXT);
    defaultOutFile.open((LOG_DIR / fileName).c_str());
}

void ListenerLogger::notifyActionPre(const Action& action)
{
    out << actionToString(action) << std::endl;
}
