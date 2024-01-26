#include "Events.h"

#include "Settings.h"
#include "Utility.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

constexpr auto http_ok{ 200 };

using json = nlohmann::json;

namespace Events
{
    RE::BSEventNotifyControl OnCellFullyLoadedEventHandler::ProcessEvent(const RE::TESCellFullyLoadedEvent*               a_event,
                                                                         RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) noexcept
    {
        if (!a_event || !Utility::first_run) {
            return RE::BSEventNotifyControl::kContinue;
        }

        Utility::first_run = false;

        if (Settings::use_loadorderlibrary) {
            httplib::Client loadorderlibrary_cli("https://api.loadorderlibrary.com");

            if (const auto res{ loadorderlibrary_cli.Get(Settings::loadorderlibrary_endpoint) }) {
                if (res->status <=> http_ok != 0) {
                    return RE::BSEventNotifyControl::kContinue;
                }

                logger::debug("LoadOrderLibrary: 200 OK");

                const auto data{ json::parse(res->body)["data"] };

                auto ver{ data["version"].dump() };
                ver = ver.substr(1, ver.size() - 2);

                logger::debug("Found version number: {}", ver);

                const auto compare_result{ Utility::CompareVersions(ver, Settings::current_version) };

                Utility::SendNotification(ver, compare_result);
            }
        }
        else if (Settings::use_wj) {
            httplib::Client github_cli("https://raw.githubusercontent.com");
            if (Settings::has_repo) {
                if (const auto res{ github_cli.Get("/wabbajack-tools/mod-lists/master/repositories.json") }) {
                    if (res->status <=> http_ok != 0) {
                        return RE::BSEventNotifyControl::kContinue;
                    }

                    logger::debug("GitHub: 200 OK");

                    if (const auto data{ json::parse(res->body) }; data.contains(Settings::machineurl_repo)) {
                        auto repo_url{ data.at(Settings::machineurl_repo).dump() };
                        repo_url = repo_url.substr(1, repo_url.size() - 2);
                        repo_url.erase(0, 33);
                        logger::debug("Repo URL: {}", repo_url);

                        if (const auto repo_res{ github_cli.Get(repo_url) }) {
                            if (repo_res->status <=> http_ok != 0) {
                                return RE::BSEventNotifyControl::kContinue;
                            }

                            logger::debug("GitHub: 200 OK");
                            for (const auto d{ json::parse(repo_res->body) }; const auto& item : d) {
                                auto machineurl{ item["links"]["machineURL"].dump() };
                                machineurl = machineurl.substr(1, machineurl.size() - 2);
                                if (machineurl <=> Settings::machineurl_name != 0) {
                                    continue;
                                }

                                logger::debug("MachineURL: {}", machineurl);

                                auto ver{ item["version"].dump() };
                                ver = ver.substr(1, ver.size() - 2);

                                logger::debug("Found version: {}", ver);

                                const auto compare_result{ Utility::CompareVersions(ver, Settings::current_version) };

                                Utility::SendNotification(ver, compare_result);

                                break;
                            }
                        }
                    }
                }
            }
            else {
                if (const auto res{ github_cli.Get("/wabbajack-tools/mod-lists/master/modlists.json") }) {
                    if (res->status <=> http_ok != 0) {
                        return RE::BSEventNotifyControl::kContinue;
                    }

                    logger::debug("GitHub: 200 OK");

                    for (const auto data{ json::parse(res->body) }; const auto& item : data) {
                        auto machineurl{ item["links"]["machineURL"].dump() };
                        machineurl = machineurl.substr(1, machineurl.size() - 2);
                        if (machineurl <=> Settings::machineurl_name != 0) {
                            continue;
                        }

                        logger::debug("MachineURL: {}", machineurl);

                        auto ver{ item["version"].dump() };
                        ver = ver.substr(1, ver.size() - 2);

                        logger::debug("Found version: {}", ver);

                        const auto compare_result{ Utility::CompareVersions(ver, Settings::current_version) };

                        Utility::SendNotification(ver, compare_result);

                        break;
                    }
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }
} // namespace Events
