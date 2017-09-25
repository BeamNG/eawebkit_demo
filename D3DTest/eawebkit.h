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

void initUI(DXContexts& dxc);
void updateUI();
