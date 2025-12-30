#include "Plugin.h"
#include "Hooks.h"
#include "Translations.h"
#include "Configuration.h"
void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
    }
    if (message->type == SKSE::MessagingInterface::kPostLoad) {
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");
    Configuration::Load();
    UI::Register();
    Hooks::Install();
    Translations::Install();
    return true;
}