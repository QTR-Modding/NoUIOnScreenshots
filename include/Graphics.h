#pragma once
#include "dxgi.h"
#include <d3d11.h>
#include <wrl/client.h>

class Graphics {
    static inline IDXGISwapChain* swapChain = nullptr;
    static inline ID3D11Device* device = nullptr;
    static inline ID3D11DeviceContext* context = nullptr;
    static ID3D11ShaderResourceView* FetchTextureBuffer();

public:
    static void Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context);
    static bool SaveCurrentFrame(const wchar_t* filename, RE::BSGraphics::TextureFileFormat format);
};