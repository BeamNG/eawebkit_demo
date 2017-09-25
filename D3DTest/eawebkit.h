/*
The MIT License

Copyright (c) 2017 BeamNG GmbH. https://github.com/BeamNG/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

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
