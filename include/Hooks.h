#pragma once
namespace Hooks {

    struct QueueItem {
        const char* a_filePath;
        RE::BSGraphics::TextureFileFormat a_textureFileFormat;
    };

    inline std::vector<QueueItem> queue;

    class TakeScreenshot{
    public:
        static void Install();

    private:
        static void thunk(RE::BSGraphics::Renderer* renderer, RE::RENDER_TARGET a_renderTarget, const char* a_filePath, RE::BSGraphics::TextureFileFormat a_textureFileFormat);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    struct CreateD3DAndSwapChain {
        static void thunk();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };

    struct RenderUIHook {
        static int64_t thunk(int64_t gMenuManager);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };

    void Install();
}
