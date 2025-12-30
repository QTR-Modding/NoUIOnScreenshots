#include "Hooks.h"

#include <chrono>
#include <thread>
#include <iostream>
#include "Graphics.h"
#include "Configuration.h"

void Hooks::TakeScreenshot::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(35882, 36853).address() + REL::Relocate(0x73, 0x69), thunk);
}

void Hooks::TakeScreenshot::thunk(RE::BSGraphics::Renderer* renderer, RE::RENDER_TARGET a_renderTarget, const char* a_filePath, RE::BSGraphics::TextureFileFormat a_textureFileFormat) {
    if (Configuration::HideUIOnScreenShots) {
        queue.push_back({strdup(a_filePath), a_textureFileFormat});
    } else {
        originalFunction(renderer, a_renderTarget, a_filePath, a_textureFileFormat);    
    }
}

void Hooks::CreateD3DAndSwapChain::thunk() {
    originalFunction();

    if (const auto renderer = RE::BSGraphics::Renderer::GetSingleton()) {
        auto data = renderer->GetRuntimeData();
        auto swapChain = reinterpret_cast<IDXGISwapChain*>(data.renderWindows[0].swapChain);
        if (!swapChain) {
            logger::error("couldn't find swapChain");
            return;
        }

        DXGI_SWAP_CHAIN_DESC desc{};
        if (FAILED(swapChain->GetDesc(std::addressof(desc)))) {
            logger::error("IDXGISwapChain::GetDesc failed.");
            return;
        }

        auto device = reinterpret_cast<ID3D11Device*>(data.forwarder);
        auto context = reinterpret_cast<ID3D11DeviceContext*>(data.context);

        Graphics::Install(swapChain, device, context);
    }
}

void Hooks::CreateD3DAndSwapChain::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    const REL::Relocation<std::uintptr_t> target{REL::RelocationID(75595, 77226)};  // BSGraphics::InitD3D
    originalFunction = trampoline.write_call<5>(target.address() + REL::Relocate(0x9, 0x275), CreateD3DAndSwapChain::thunk);
}

std::wstring CharToWChar(const char* str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], len);
    wstr.pop_back();
    return wstr;
}
int64_t Hooks::RenderUIHook::thunk(int64_t gMenuManager) { 
    while (!queue.empty()) {
        auto item = queue.back();
        Graphics::SaveCurrentFrame(CharToWChar(item.a_filePath).c_str(), item.a_textureFileFormat);
        delete item.a_filePath;
        queue.pop_back();
    }
    return originalFunction(gMenuManager);
}

void Hooks::RenderUIHook::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(35556, 36555).address() + REL::Relocate(0x3ab, 0x371), thunk);
}

void Hooks::Install() {
    TakeScreenshot::Install();
    CreateD3DAndSwapChain::Install();
    RenderUIHook::Install();
}