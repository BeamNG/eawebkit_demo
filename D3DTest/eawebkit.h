#pragma once

#include <d3d11.h>



struct DXContexts {
    ID3D11Device* dev = nullptr;
    ID3D11DeviceContext* ctx = nullptr;
    ID3D11Buffer *vBuffer = nullptr;
    ID3D11Buffer *cbuffer = nullptr;
    ID3D11InputLayout *inputLayout = nullptr;
    ID3D11VertexShader *vShader = nullptr;
    ID3D11PixelShader *pShader = nullptr;
    ID3D11SamplerState *samplerState = nullptr;
};

void ui_init(DXContexts& dxc);
void ui_update();
void ui_resize(int width, int height);
void ui_mousemove(int x, int y);
void ui_mousebutton(int x, int y, int btn, bool depressed);
void ui_mousewheel(int x, int y, int keys, int delta);
void ui_keyboard(int id, bool ischar, bool depressed);
void ui_reload();