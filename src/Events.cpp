#include "Events.h"

#include "Settings.h"
#include "Utility.h"

#include "nlohmann/json.hpp"

constexpr auto http_ok = 200;

using json = nlohmann::json;

namespace Events {
    RE::BSEventNotifyControl OnCellFullyLoadedEventHandler::ProcessEvent(const RE::TESCellFullyLoadedEvent*               a_event,
                                                                         RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) {
        if (!a_event)
            return RE::BSEventNotifyControl::kContinue;

        if (Utility::first_run) {
            Utility::first_run = false;
            if (Settings::use_loadorderlibrary) {
                httplib::Client loadorderlibrary_cli("https://api.loadorderlibrary.com");
                if (const auto res = loadorderlibrary_cli.Get(Settings::loadorderlibrary_endpoint)) {
                    if (res->status == http_ok) {
                        logger::debug("LoadOrderLibrary: 200 OK");
                        const auto data = json::parse(res->body)["data"];
                        auto       ver  = data["version"].dump();
                        ver             = ver.substr(1, ver.size() - 2);
                        logger::debug("Found version number: {}", ver);
                        const auto compare_result = CompareVersions(ver, Settings::current_version);
                        SendNotification(ver, compare_result);
                    }
                }
            } else if (Settings::use_wj) {
                httplib::Client github_cli("https://raw.githubusercontent.com");
                if (Settings::has_repo) {
                    if (const auto res = github_cli.Get("/wabbajack-tools/mod-lists/master/repositories.json")) {
                        if (res->status == http_ok) {
                            logger::debug("GitHub: 200 OK");
                            if (const auto data = json::parse(res->body); data.contains(Settings::machineurl_repo)) {
                                auto repo_url = data.at(Settings::machineurl_repo).dump();
                                repo_url      = repo_url.substr(1, repo_url.size() - 2);
                                repo_url.erase(0, 33);
                                logger::debug("Repo URL: {}", repo_url);
                                if (const auto repo_res = github_cli.Get(repo_url)) {
                                    if (repo_res->status == http_ok) {
                                        logger::debug("GitHub: OK");
                                        for (const auto d = json::parse(repo_res->body); const auto& item : d) {
                                            auto machineurl = item["links"]["machineURL"].dump();
                                            machineurl      = machineurl.substr(1, machineurl.size() - 2);
                                            logger::debug("MachineURL: {}", machineurl);
                                            if (machineurl == Settings::machineurl_name) {
                                                auto ver = item["version"].dump();
                                                ver      = ver.substr(1, ver.size() - 2);
                                                logger::debug("Found version: {}", ver);
                                                const auto compare_result = CompareVersions(ver, Settings::current_version);
                                                SendNotification(ver, compare_result);
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    if (const auto res = github_cli.Get("/wabbajack-tools/mod-lists/master/modlists.json")) {
                        if (res->status == http_ok) {
                            logger::debug("GitHub: 200 OK");
                            for (const auto data = json::parse(res->body); const auto& item : data) {
                                auto machineurl = item["links"]["machineURL"].dump();
                                machineurl      = machineurl.substr(1, machineurl.size() - 2);
                                logger::debug("MachineURL: {}", machineurl);
                                if (machineurl == Settings::machineurl_name) {
                                    auto ver = item["version"].dump();
                                    ver      = ver.substr(1, ver.size() - 2);
                                    logger::debug("Found version: {}", ver);
                                    const auto compare_result = CompareVersions(ver, Settings::current_version);
                                    SendNotification(ver, compare_result);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    void OnCellFullyLoadedEventHandler::SendNotification(std::string version, int compare_result) {
        logger::debug("Sending update notification");
        const auto update_notif          = fmt::format("A new version of {} is available: v{}", Settings::list_name, version);
        const auto current_version_notif = fmt::format("Current {} version: v{}", Settings::list_name, Settings::current_version);
        std::jthread([=] {
            std::this_thread::sleep_for(std::chrono::seconds(Settings::notification_delay));
            SKSE::GetTaskInterface()->AddTask([=] {
                if (Settings::always_show_current_version)
                    RE::DebugNotification(current_version_notif.c_str());
                if (compare_result == 0) {
                    if (!Settings::always_show_current_version)
                        RE::DebugNotification(current_version_notif.c_str());
                    RE::DebugNotification(update_notif.c_str(), "UISkillsFocus");
                    RE::DebugNotification(fmt::format("v{} is SAVE-SAFE", version).c_str());

                } else if (compare_result == 1) {
                    if (!Settings::always_show_current_version)
                        RE::DebugNotification(current_version_notif.c_str());
                    RE::DebugNotification(update_notif.c_str(), "UISkillsFocus");
                    RE::DebugNotification(fmt::format("v{} REQUIRES A NEW SAVE", version).c_str());
                }
            });
        }).detach();
    }

    int OnCellFullyLoadedEventHandler::CompareVersions(std::string fetched, std::string current) {
        const auto major_f = std::stoi(fetched.substr(0, fetched.find('.')));
        logger::debug("major_f: {}", major_f);
        fetched.erase(0, fetched.find('.') + 1);
        logger::debug("fetched: {}", fetched);
        const auto minor_f = std::stoi(fetched.substr(0, fetched.find('.')));
        logger::debug("minor_f: {}", minor_f);
        fetched.erase(0, fetched.find('.') + 1);
        logger::debug("fetched: {}", fetched);
        const auto patch_f = std::stoi(fetched.substr(0, fetched.find('.')));
        logger::debug("patch_f: {}", patch_f);
        fetched.erase(0, fetched.find('.') + 1);
        logger::debug("fetched: {}", fetched);

        std::optional<int> hotfix_f;
        if (!fetched.empty())
            hotfix_f = std::stoi(fetched);

        logger::debug("current: {}", current);
        const auto major_c = std::stoi(current.substr(0, current.find('.')));
        logger::debug("major_c: {}", major_c);
        current.erase(0, current.find('.') + 1);
        logger::debug("current: {}", current);
        const auto minor_c = std::stoi(current.substr(0, current.find('.')));
        logger::debug("minor_c: {}", minor_c);
        current.erase(0, current.find('.') + 1);
        logger::debug("current: {}", current);
        const auto patch_c = std::stoi(current.substr(0, current.find('.')));
        logger::debug("patch_c: {}", patch_c);
        current.erase(0, current.find('.') + 1);
        logger::debug("current: {}", current);

        std::optional<int> hotfix_c;
        if (!current.empty())
            hotfix_c = std::stoi(current);

        if (hotfix_f.has_value()) {
            if (major_f == major_c && minor_f == minor_c && (patch_f > patch_c || hotfix_f.value() > hotfix_c.value()))
                return 0; // Save-safe
            if (major_f > major_c || minor_f > minor_c)
                return 1; // Save-breaking
        } else {
            if (major_f == major_c && minor_f == minor_c && patch_f > patch_c)
                return 0; // Save-safe
            if (major_f > major_c || minor_f > minor_c)
                return 1; // Save-breaking
        }

        return -1; // Inconclusive/no update
    }
}
