#include "make_log.h"

#include <chrono>
#include <filesystem>

const std::string LOG_EXT = ".log";
const std::filesystem::path LOG_DIR("logs");

std::ofstream makeLog()
{
    auto now = std::chrono::system_clock::now().time_since_epoch();
    long long timeSecs = std::chrono::duration_cast<std::chrono::seconds>(now).count();

    std::filesystem::create_directory(LOG_DIR);
    std::filesystem::path fileName(std::to_string(timeSecs) + LOG_EXT);
    return std::ofstream((LOG_DIR / fileName).c_str());
}