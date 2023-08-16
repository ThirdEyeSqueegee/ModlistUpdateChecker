#pragma once

namespace Events {
    class OnCellFullyLoadedEventHandler : public RE::BSTEventSink<RE::TESCellFullyLoadedEvent> {
    protected:
        OnCellFullyLoadedEventHandler() = default;
        ~OnCellFullyLoadedEventHandler() override = default;

    public:
        OnCellFullyLoadedEventHandler(const OnCellFullyLoadedEventHandler&) = delete;
        OnCellFullyLoadedEventHandler(OnCellFullyLoadedEventHandler&&) = delete;
        OnCellFullyLoadedEventHandler& operator=(const OnCellFullyLoadedEventHandler&) = delete;
        OnCellFullyLoadedEventHandler& operator=(OnCellFullyLoadedEventHandler&&) = delete;

        static OnCellFullyLoadedEventHandler* GetSingleton();

        RE::BSEventNotifyControl ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event,
                                              RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) override;

        static void Register();

        static void SendNotification(std::string version);
    };
}
