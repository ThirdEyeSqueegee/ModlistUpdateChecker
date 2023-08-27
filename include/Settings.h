#pragma once

class Settings : public Singleton<Settings>
{
public:
    static void LoadSettings() noexcept;

    inline static bool debug_logging{};

    inline static bool        disable{};
    inline static bool        use_wj{};
    inline static bool        use_loadorderlibrary{};
    inline static bool        has_repo{};
    inline static bool        always_show_current_version{};
    inline static int         notification_delay{};
    inline static std::string list_name{};
    inline static std::string loadorderlibrary_endpoint{};
    inline static std::string wj_machineurl{};
    inline static std::string machineurl_repo{};
    inline static std::string machineurl_name{};
    inline static std::string current_version{};
};
