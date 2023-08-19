#pragma once

namespace Events {
    class OnCellFullyLoadedEventHandler : public EventSingleton<OnCellFullyLoadedEventHandler, RE::TESCellFullyLoadedEvent> {
    public:
        RE::BSEventNotifyControl ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event,
                                              RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) override;

        static void SendNotification(std::string version, int compare_result);

        static int CompareVersions(std::string fetched, std::string current);
    };
}
