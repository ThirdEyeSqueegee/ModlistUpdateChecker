#pragma once

class Settings : public Singleton<Settings> {
public:
    static void LoadSettings();

    inline static bool debug_logging = false;

    inline static bool use_wj = false;
    inline static bool use_loadorderlibrary = false;
    inline static bool has_repo = false;
    inline static bool always_show_current_version = true;
    inline static int notification_delay = 60;
    inline static std::string list_name = "";
    inline static std::string loadorderlibrary_endpoint = "";
    inline static std::string wj_machineurl = "";
    inline static std::string machineurl_repo = "";
    inline static std::string machineurl_name = "";
    inline static std::string current_version = "";
};
