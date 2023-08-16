#include "Settings.h"

#include "SimpleIni.h"

Settings* Settings::GetSingleton() {
    static Settings singleton;
    return std::addressof(singleton);
}

void Settings::LoadSettings() {
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\ModlistUpdateChecker.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging) {
        spdlog::get("Global")->set_level(spdlog::level::level_enum::debug);
        logger::debug("Debug logging enabled");
    }

    use_wj = ini.GetBoolValue("General", "bUseWJ");
    use_loadorderlibrary = ini.GetBoolValue("General", "bUseLoadOrderLibrary");
    notification_delay = static_cast<int>(ini.GetLongValue("General", "uNotificationDelay"));
    list_name = ini.GetValue("General", "Name");
    loadorderlibrary_endpoint = ini.GetValue("General", "Endpoint");
    wj_machineurl = ini.GetValue("General", "MachineURL");
    current_version = ini.GetValue("General", "Version");

    if (wj_machineurl.contains("/")) {
        has_repo = true;
        machineurl_repo = wj_machineurl.substr(0, wj_machineurl.find("/"));
        machineurl_name = wj_machineurl.substr(wj_machineurl.find("/") + 1, wj_machineurl.size());
        logger::debug("Repository {} and name {} found in MachineURL", machineurl_repo, machineurl_name);
    }

    logger::info("Loaded settings");
    logger::info("\tbUseWJ = {}", use_wj);
    logger::info("\tbUseLoadOrderLibrary = {}", use_loadorderlibrary);
    logger::info("\tuNotificationDelay = {}", notification_delay);
    logger::info("\tName = {}", list_name);
    logger::info("\tEndpoint = {}", loadorderlibrary_endpoint);
    logger::info("\tMachineURL = {}", wj_machineurl);
    logger::info("\tVersion = {}", current_version);
}
