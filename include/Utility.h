#pragma once

struct Version
{
    int                major{};
    int                minor{};
    int                patch{};
    std::optional<int> hotfix{};
};

class Utility : public Singleton<Utility>
{
public:
    enum class UpdateState
    {
        SAVE_SAFE,
        SAVE_BREAKING,
        NO_UPDATE
    };
    using enum UpdateState;

    static void SendNotification(const std::string& version, const UpdateState compare_result) noexcept
    {
        logger::debug("Sending update notification");

        const auto update_notif{ fmt::format("A new version of {} is available: v{}", Settings::list_name, version) };
        const auto current_version_notif{ fmt::format("Current {} version: v{}", Settings::list_name, Settings::current_version) };

        std::jthread([=] {
            std::this_thread::sleep_for(std::chrono::seconds(Settings::notification_delay));
            if (Settings::exit_if_not_latest) {
                RE::DebugNotification(fmt::format("WARNING: Please update {} to the latest version.", Settings::list_name).c_str());
                std::this_thread::sleep_for(7.5s);
                TerminateProcess(GetCurrentProcess(), 0);
            }
            SKSE::GetTaskInterface()->AddTask([=] {
                if (Settings::always_show_current_version) {
                    RE::DebugNotification(current_version_notif.c_str());
                }
                if (compare_result <=> SAVE_SAFE == 0) {
                    if (!Settings::always_show_current_version) {
                        RE::DebugNotification(current_version_notif.c_str());
                    }
                    RE::DebugNotification(update_notif.c_str(), "UISkillsFocus");
                    if (!Settings::no_save_safe_notification) {
                        RE::DebugNotification(fmt::format("v{} is SAVE-SAFE", version).c_str());
                    }
                }
                else if (compare_result <=> SAVE_BREAKING == 0) {
                    if (!Settings::always_show_current_version) {
                        RE::DebugNotification(current_version_notif.c_str());
                    }
                    RE::DebugNotification(update_notif.c_str(), "UISkillsFocus");
                    if (!Settings::no_save_safe_notification) {
                        RE::DebugNotification(fmt::format("v{} REQUIRES A NEW SAVE", version).c_str());
                    }
                }
            });
        }).detach();
    }

    static UpdateState CompareVersions(const std::string& fetched, const std::string& current)
    {
        const auto& [major_f, minor_f, patch_f, hotfix_f]{ ParseVersionString(fetched) };
        const auto& [major_c, minor_c, patch_c, hotfix_c]{ ParseVersionString(current) };

        const auto major_cmp{ major_f <=> major_c };
        const auto minor_cmp{ minor_f <=> minor_c };
        const auto patch_cmp{ patch_f <=> patch_c };

        if (major_cmp > 0) {
            return SAVE_BREAKING;
        }
        if (major_cmp == 0 && minor_cmp > 0) {
            return SAVE_BREAKING;
        }
        if (hotfix_f.has_value() && hotfix_c.has_value()) {
            if (const auto hotfix_cmp{ hotfix_f.value() <=> hotfix_c.value() }; major_cmp == 0 && minor_cmp == 0 && (patch_cmp > 0 || hotfix_cmp > 0)) {
                return SAVE_SAFE;
            }
        }
        else if (major_cmp == 0 && minor_cmp == 0 && patch_cmp > 0) {
            return SAVE_SAFE;
        }

        return NO_UPDATE;
    }

    inline static bool first_run{ true };

private:
    static Version ParseVersionString(std::string version_string) noexcept
    {
        bool has_hotfix{};

        const auto major{ std::stoi(version_string.substr(0, version_string.find('.'))) };
        logger::debug("major: {}", major);
        version_string.erase(0, version_string.find('.') + 1);

        const auto minor{ std::stoi(version_string.substr(0, version_string.find('.'))) };
        logger::debug("minor: {}", minor);
        version_string.erase(0, version_string.find('.') + 1);

        if (version_string.contains('.')) {
            has_hotfix = true;
            logger::debug("has hotfix");
        }

        const auto patch{ std::stoi(version_string.substr(0, has_hotfix ? version_string.find('.') : version_string.size())) };
        logger::debug("patch: {}", patch);
        version_string.erase(0, has_hotfix ? version_string.find('.') + 1 : version_string.size());

        std::optional<int> hotfix;
        if (!version_string.empty()) {
            hotfix = std::stoi(version_string);
            logger::debug("hotfix: {}", hotfix.value());
        }

        return { major, minor, patch, hotfix };
    }
};
