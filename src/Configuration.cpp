#include "Configuration.h"
#include <nlohmann/json.hpp>

void Configuration::Load() {
    std::ifstream file(".\\Data\\SKSE\\Plugins\\NoUIOnScreenshots.json");
    if (!file.is_open()) {
        logger::error("Config file not found");
        return;
    }
    nlohmann::json j;
    file >> j;


    if (j.contains("EnableUIOnScreenShots")) {
        EnableUIOnScreenShots = j["EnableUIOnScreenShots"];
        logger::info("EnableUIOnScreenShots: {}", EnableUIOnScreenShots);
    }

}

void Configuration::Save() {
    std::filesystem::create_directories(".\\Data\\SKSE\\Plugins");

    nlohmann::json j;
    j["EnableUIOnScreenShots"] = EnableUIOnScreenShots;

    std::ofstream file(".\\Data\\SKSE\\Plugins\\NoUIOnScreenshots.json");
    if (!file.is_open()) {
        logger::error("Failed to write config file");
        return;
    }

    file << j.dump(4);
}
