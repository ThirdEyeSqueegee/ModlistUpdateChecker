#pragma once

class Settings {
protected:
    Settings() = default;
    ~Settings() = default;

public:
    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings& operator=(Settings&&) = delete;

    static Settings* GetSingleton();

    static void LoadSettings();

    inline static bool debug_logging = false;

    inline static bool use_wj = false;
    inline static bool use_loadorderlibrary = false;
    inline static bool has_repo = false;
    inline static int notification_delay = 60;
    inline static std::string list_name = "";
    inline static std::string loadorderlibrary_endpoint = "";
    inline static std::string wj_machineurl = "";
    inline static std::string machineurl_repo = "";
    inline static std::string machineurl_name = "";
    inline static std::string current_version = "";
};
