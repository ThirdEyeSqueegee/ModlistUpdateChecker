#pragma once

namespace Events
{
    class OnCellFullyLoadedEventHandler final : public EventSingleton<OnCellFullyLoadedEventHandler, RE::TESCellFullyLoadedEvent>
    {
    public:
        RE::BSEventNotifyControl ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource) noexcept override;
    };
} // namespace Events
