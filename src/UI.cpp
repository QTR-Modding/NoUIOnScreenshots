#include "UI.h"
#include "Translations.h"

void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection(MOD_NAME);
    SKSEMenuFramework::AddSectionItem("Config", Main::Render);
}


void __stdcall UI::Main::Render() {
    if (ImGuiMCPComponents::ToggleButton(Translations::Get("MCP.EnableUIOnScreenShots"), &Configuration::EnableUIOnScreenShots)) {
        Configuration::Save();
    }
}

