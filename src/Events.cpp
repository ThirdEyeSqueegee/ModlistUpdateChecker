#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "Events.h"

#include "Settings.h"
#include "Utility.h"

#include "httplib.h"
#include "nlohmann/json.hpp"

constexpr auto http_ok = 200;

using json = nlohmann::json;

namespace Events {
    OnCellFullyLoadedEventHandler* OnCellFullyLoadedEventHandler::GetSingleton() {
        static OnCellFullyLoadedEventHandler singleton;
        return std::addressof(singleton);
    }

    RE::BSEventNotifyControl OnCellFullyLoadedEventHandler::ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event,
                                                                         RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) {
        if (!a_event)
            return RE::BSEventNotifyControl::kContinue;

        if (Utility::first_run) {
            Utility::first_run = false;
            httplib::Result res;
            if (Settings::use_loadorderlibrary) {
                httplib::Client loadorderlibrary_cli("https://api.loadorderlibrary.com");
                res = loadorderlibrary_cli.Get(Settings::loadorderlibrary_endpoint);
                if (res->status == http_ok) {
                    logger::debug("LoadOrderLibrary: 200 OK");
                    const auto data = json::parse(res->body)["data"];
                    auto ver = data["version"].dump();
                    ver = ver.substr(1, ver.size() - 2);
                    logger::debug("Found version number: {}", ver);
                    if (ver != Settings::current_version)
                        SendNotification(ver);
                }
            } else if (Settings::use_wj) {
                httplib::Client github_cli("https://raw.githubusercontent.com");
                if (Settings::has_repo) {
                    res = github_cli.Get("/wabbajack-tools/mod-lists/master/repositories.json");
                    if (res->status == http_ok) {
                        logger::debug("GitHub: 200 OK");
                        if (const auto data = json::parse(res->body); data.contains(Settings::machineurl_repo)) {
                            auto repo_url = data.at(Settings::machineurl_repo).dump();
                            repo_url = repo_url.substr(1, repo_url.size() - 2);
                            repo_url.erase(0, 33);
                            logger::debug("Repo URL: {}", repo_url);
                            res = github_cli.Get(repo_url);
                            if (res->status == http_ok) {
                                logger::debug("OK");
                                for (const auto d = json::parse(res->body); const auto& item : d) {
                                    auto machineurl = item["links"]["machineURL"].dump();
                                    machineurl = machineurl.substr(1, machineurl.size() - 2);
                                    logger::debug("MachineURL: {}", machineurl);
                                    if (machineurl == Settings::machineurl_name) {
                                        auto ver = item["version"].dump();
                                        ver = ver.substr(1, ver.size() - 2);
                                        logger::debug("Found version: {}", ver);
                                        if (ver != Settings::current_version) {
                                            SendNotification(ver);
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    res = github_cli.Get("/wabbajack-tools/mod-lists/master/modlists.json");
                    if (res->status == http_ok) {
                        logger::debug("GitHub: 200 OK");
                        for (const auto data = json::parse(res->body); const auto& item : data) {
                            auto machineurl = item["links"]["machineURL"].dump();
                            machineurl = machineurl.substr(1, machineurl.size() - 2);
                            logger::debug("MachineURL: {}", machineurl);
                            if (machineurl == Settings::machineurl_name) {
                                auto ver = item["version"].dump();
                                ver = ver.substr(1, ver.size() - 2);
                                logger::debug("Found version: {}", ver);
                                if (ver != Settings::current_version) {
                                    SendNotification(ver);
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

    void OnCellFullyLoadedEventHandler::Register() {
        const auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->AddEventSink(GetSingleton());
        logger::info("Registered cell fully loaded event handler");
    }

    void OnCellFullyLoadedEventHandler::SendNotification(std::string version) {
        logger::debug("Sending update notification");
        const auto update_notif = std::format("A new version of {} is available: v{}", Settings::list_name, version);
        const auto current_version_notif = std::format("Current {} version: v{}", Settings::list_name, Settings::current_version);
        std::jthread([=] {
            std::this_thread::sleep_for(std::chrono::seconds(Settings::notification_delay));
            SKSE::GetTaskInterface()->AddTask([=] {
                RE::DebugNotification(current_version_notif.c_str());
                RE::DebugNotification(update_notif.c_str(), "UISkillsFocus");
            });
        }).detach();
    }
}
