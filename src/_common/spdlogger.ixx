module;
#include <spdlog/sinks/win_eventlog_sink.h>
#include <spdlog/spdlog.h>

export module spdlogger;
import std;

export namespace winEventLogger {
spdlog::logger logger("win_event_logger",
                      std::make_shared<spdlog::sinks::win_eventlog_sink_mt>([] {
                        std::string exeName(MAX_PATH, 0);
                        GetModuleFileNameA(GetModuleHandle(nullptr),
                                           exeName.data(), exeName.size());
                        return exeName.substr(exeName.rfind("\\") + 1);
                      }()));
}