#include "logger.h"
#include <iostream>
#include <filesystem>

void init_logger() {
    try {
        // Create a console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::info); // Console logs at INFO and above
        console_sink->set_pattern("[%^%l%$] %v");    // Log level in color

        // Create a file sink
        std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink;
        if (std::filesystem::exists("build/bin/logs/truck_platooning.log")) {
            // File exists, append logs
            file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/truck_platooning.log", true);
        } else {
            // File doesn't exist, create a new one
            std::filesystem::create_directories("logs");  // Create the "logs" directory if it doesn't exist
            file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/truck_platooning.log", false);
        }
        file_sink->set_level(spdlog::level::debug);  // File logs at DEBUG and above
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v"); // Detailed log format

        // Combine the sinks into a logger
        auto logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
        logger->set_level(spdlog::level::debug); // Set overall log level
        logger->flush_on(spdlog::level::info);  // Flush logs for INFO and above

        // Set the logger as the default
        spdlog::set_default_logger(logger);

        // Optional: Log an initialization message
        spdlog::info("Logger initialized successfully.");
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
    }
}