#include "Settings.h"

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\ModlistUpdateChecker.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging)
    {
        spdlog::get("Global")->set_level(spdlog::level::level_enum::debug);
        logger::debug("Debug logging enabled");
    }

    disable = ini.GetBoolValue("General", "bDisable");

    if (disable)
    {
        logger::info("bDisable set to true. Disabling...");
        return;
    }

    use_wj                      = ini.GetBoolValue("Wabbajack", "bUseWJ");
    use_loadorderlibrary        = ini.GetBoolValue("LoadOrderLibrary", "bUseLoadOrderLibrary");
    always_show_current_version = ini.GetBoolValue("General", "bAlwaysShowCurrentVersion");
    notification_delay          = static_cast<int>(ini.GetLongValue("General", "uNotificationDelay"));
    list_name                   = ini.GetValue("General", "Name");
    loadorderlibrary_endpoint   = ini.GetValue("LoadOrderLibrary", "Endpoint");
    wj_machineurl               = ini.GetValue("Wabbajack", "MachineURL");
    current_version             = ini.GetValue("General", "Version");

    if (wj_machineurl.contains('/'))
    {
        has_repo        = true;
        machineurl_repo = wj_machineurl.substr(0, wj_machineurl.find("/"));
        machineurl_name = wj_machineurl.substr(wj_machineurl.find("/") + 1, wj_machineurl.size());
        logger::debug(R"(Repository "{}" and name "{}" found in MachineURL)", machineurl_repo, machineurl_name);
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
