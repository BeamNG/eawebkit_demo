#include "eawebkit.h"

#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebkitAllocator.h>
#include <EAWebKit/EAWebKitFileSystem.h>
#include <EAWebKit/EAWebKitClient.h>
#include <EAWebKit/EAWebKitView.h>
#include <EAText/EAText.h>

#include <stdio.h>

#include <windows.h> // LoadLibraryA
#include <assert.h>
#include <array>

#include <Shlwapi.h>
#include "EAWebkit/EAWebKitTextInterface.h"
#pragma comment(lib, "shlwapi.lib")

HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

EA::WebKit::EAWebKitLib* wk = nullptr;
EA::WebKit::View* v = nullptr;


// Callbacks
double EAWebKitTimerCallback_() { return 0; }
double EAWebKitMonotonicTimerCallback_() { return 0; };
void*  EAWebKitStackBaseCallback_() { return nullptr; };
bool   EAWebKitCryptographicallyRandomValueCallback_(unsigned char *buffer, size_t length) { return false; } // Returns true if no error, else false
void   EAWebKitGetCookiesCallback_(const char16_t* pUrl, EA::WebKit::EASTLFixedString16Wrapper& result, uint32_t flags) { }
bool   EAWebKitSetCookieCallback_(const EA::WebKit::CookieEx& cookie) { return false;  }



struct EA::WebKit::AppCallbacks callbacks = {
    EAWebKitTimerCallback_,
    EAWebKitMonotonicTimerCallback_,
    EAWebKitStackBaseCallback_,
    EAWebKitCryptographicallyRandomValueCallback_,
    EAWebKitGetCookiesCallback_,
    EAWebKitSetCookieCallback_
};

class EaWebkitClient : public EA::WebKit::EAWebKitClient {
public:
    virtual void DebugLog(EA::WebKit::DebugLogInfo& l) override {
        printf("%s\n", l.mpLogText);
        OutputDebugStringA(l.mpLogText);
        OutputDebugStringA("\n");
    }
};

class DX11Surface : public EA::WebKit::ISurface {
public:
    DXContexts& dxc;

    ID3D11Texture2D* tex = nullptr;
    ID3D11ShaderResourceView *view = nullptr;
    D3D11_MAPPED_SUBRESOURCE mapped;
public:
    DX11Surface(DXContexts& _dxc) : dxc(_dxc) {}
    virtual ~DX11Surface() { }

    virtual void Lock(SurfaceDescriptor *pSDOut, const EA::WebKit::IntRect *rect = NULL) override {
        // TODO rect
        HRESULT res = dxc.ctx->Map(tex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        assert(SUCCEEDED(res));
        pSDOut->mData = mapped.pData;
        pSDOut->mStride = mapped.RowPitch;
    }

    virtual void Unlock(void) override {
        dxc.ctx->Unmap(tex, 0);
    }

    virtual void Release(void) override {
        tex->Release();
    }

    virtual bool IsAllocated(void) const override {
        return (tex != nullptr);
    }

    virtual void Reset(void) override {
        // no idea what this is supposed to do
    }

protected:
    virtual void AllocateSurface(int width, int height) override {
        // no idea if this is correct
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;

        HRESULT res = dxc.dev->CreateTexture2D(&desc, NULL, &tex);
        assert(SUCCEEDED(res));

        D3D11_SHADER_RESOURCE_VIEW_DESC descView = {};
        descView.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        descView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        descView.Texture2D.MipLevels = 1;
        descView.Texture2D.MostDetailedMip = 0;

        res = dxc.dev->CreateShaderResourceView(tex, &descView, &view);
        assert(SUCCEEDED(res));
    }

};

class DX11Renderer : public EA::WebKit::IHardwareRenderer {
    DXContexts& dxc;
public:
    DX11Renderer(DXContexts& _dxc) : dxc(_dxc) {}
    virtual ~DX11Renderer() { }

    virtual EA::WebKit::ISurface * CreateSurface(EA::WebKit::SurfaceType surfaceType, const void* data = 0, size_t length = 0) override {
        DX11Surface* res = new DX11Surface(dxc);
        if (data && length) {
            EA::WebKit::ISurface::SurfaceDescriptor sd = {};
            res->Lock(&sd);
            memcpy(sd.mData, data, length);
            res->Unlock();
        }
        return res;
    }

    virtual void SetRenderTarget(EA::WebKit::ISurface *target) override {
        printf("*** %s\n", __FUNCTION__);
    }

    virtual void RenderSurface(EA::WebKit::ISurface *surface, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix, float opacity, EA::WebKit::CompositOperator op, EA::WebKit::TextureWrapMode wrap, EA::WebKit::Filters &filters) override {
        DX11Surface *d3dSurface = static_cast<DX11Surface*>(surface);
        dxc.ctx->PSSetShaderResources(0, 1, &d3dSurface->view);
        dxc.ctx->UpdateSubresource(dxc.cbuffer, 0, nullptr, &matrix, sizeof(float[16]), 0);
        dxc.ctx->Draw(6, 0);
    }

    virtual void FillColor(uint32_t premultiplied_rgba32, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix, EA::WebKit::CompositOperator op) override {
        printf("*** %s\n", __FUNCTION__);
    }

    virtual void DrawOutline(uint32_t premultiplied_rgba32, EA::WebKit::FloatRect &target, EA::WebKit::TransformationMatrix &matrix) override {
        printf("*** %s\n", __FUNCTION__);
    }

    virtual int32_t MaxTextureSize(void) override {
        return 4096;
    }

    virtual void Clear(EA::WebKit::ClearFlags flags, uint32_t premultiplied_rgba32, float z, uint32_t stencil) override {
        printf("*** %s\n", __FUNCTION__);
    }

    virtual void ScissorClip(EA::WebKit::IntRect axisAlignedRect) override {
        printf("*** %s\n", __FUNCTION__);
    }

    virtual void DrawStencil(EA::WebKit::TransformationMatrix &matrix, EA::WebKit::FloatRect &target, uint32_t stencilIndex) override {
        printf("*** %s\n", __FUNCTION__);
    }

    virtual void ClipAgainstStencil(uint32_t stencilIndex) override {
        printf("*** %s\n", __FUNCTION__);
    }

    virtual bool UseCustomClip() override {
        return false;
    }

    virtual void BeginClip(EA::WebKit::TransformationMatrix &matrix, EA::WebKit::FloatRect &target) override {
        printf("*** %s\n", __FUNCTION__);
    }

    virtual void EndClip(void) override {
        printf("*** %s\n", __FUNCTION__);
    }

    virtual EA::WebKit::IntRect CurrentClipBound() override {
        return EA::WebKit::IntRect(0, 0, 800, 600);
    }

    virtual void BeginPainting(void) override 
    {
        dxc.ctx->VSSetShader(dxc.vShader, nullptr, 0);
        dxc.ctx->PSSetShader(dxc.pShader, nullptr, 0);
        dxc.ctx->VSSetConstantBuffers(0, 1, &dxc.cbuffer);
        dxc.ctx->PSSetSamplers(0, 1, &dxc.samplerState);
        UINT stride = sizeof(float[4]);
        UINT offfset = 0;
        dxc.ctx->IASetVertexBuffers(0, 1, &dxc.vBuffer, &stride, &offfset);
        dxc.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        dxc.ctx->IASetInputLayout(dxc.inputLayout);
    }

    virtual void EndPainting(void)  override{
        printf("*** %s\n", __FUNCTION__);
    }
};


std::string getExePath() {
    char path[MAX_PATH] = "";
    DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
    PathRemoveFileSpecA(path);
    return std::string(path);
}

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void initUI(DXContexts& dxc) {
    
    // init the systems: using DefaultAllocator, DefaultFileSystem, no text/font support, DefaultThreadSystem
    struct EA::WebKit::AppSystems systems = { nullptr };
    systems.mEAWebkitClient = new EaWebkitClient();

    typedef EA::WebKit::EAWebKitLib* (*PF_CreateEAWebkitInstance)(void);
    PF_CreateEAWebkitInstance create_webkit_instance = nullptr;

    HMODULE wdll = LoadLibraryA("EAWebkit.dll");
    if (wdll != nullptr) {
        create_webkit_instance = reinterpret_cast<PF_CreateEAWebkitInstance>(GetProcAddress(wdll, "CreateEAWebkitInstance"));
    }

    if (!create_webkit_instance) {
        printf("EAWebkit.dll missing\n");
        exit(1);
    }

    wk = create_webkit_instance();
    wk->Init(&callbacks, &systems);

    EA::WebKit::ITextSystem* ts = wk->GetTextSystem();
    ts->Init();

    EA::WebKit::Parameters& params = wk->GetParameters();
    params.mEAWebkitLogLevel = 1337;
    params.mHttpManagerLogLevel = 1337;
    wk->SetParameters(params);

    v = wk->CreateView();

    EA::WebKit::ViewParameters vp;
    vp.mHardwareRenderer = new DX11Renderer(dxc);
    vp.mDisplaySurface = new DX11Surface(dxc);
    //vp.mTileSize = 32;
    //vp.mUseTiledBackingStore = true;
    v->InitView(vp);

    //v->SetDrawDebugVisuals(true);
    //v->ShowInspector(true);

    //v->SetURI("http://html5test.com/");
    
    //v->SetURI("about:version");

    std::string exe_path = replaceAll(getExePath(), "\\", "/");
    std::string html_path = "file:///" + exe_path + "/test.html";

    v->SetURI(html_path.c_str());

    //const char test[] = "<div style='border:10px dashed red;'> </div>";
    //v->SetHtml(test, sizeof(test));

    // Directx init
    HRESULT res;
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.ByteWidth = sizeof(float[4]) * 6; // todo
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        res = dxc.dev->CreateBuffer(&desc, nullptr, &dxc.vBuffer);
        assert(SUCCEEDED(res));
    }

    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = sizeof(float[16]);
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        res = dxc.dev->CreateBuffer(&desc, nullptr, &dxc.cbuffer);
        assert(SUCCEEDED(res));
    }

    {
        std::array<D3D11_INPUT_ELEMENT_DESC, 2> desc;
        desc[0].SemanticName = "POSITION";
        desc[0].SemanticIndex = 0;
        desc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
        desc[0].InputSlot = 0;
        desc[0].AlignedByteOffset = 0;
        desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        desc[0].InstanceDataStepRate = 0;
        desc[1].SemanticName = "TEXCOORD";
        desc[1].SemanticIndex = 0;
        desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        desc[1].InputSlot = 0;
        desc[1].AlignedByteOffset = 8;
        desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        desc[1].InstanceDataStepRate = 0;

        ID3DBlob* blob = nullptr;
        res = CompileShaderFromFile(L"ui.hlsl", "main_vs", "vs_4_0", &blob);
        assert(SUCCEEDED(res));

        res = dxc.dev->CreateInputLayout(desc.data(), (UINT)desc.size(), blob->GetBufferPointer(), blob->GetBufferSize(), &dxc.inputLayout);
        assert(SUCCEEDED(res));

        res = dxc.dev->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &dxc.vShader);
        blob->Release();

        res = CompileShaderFromFile(L"ui.hlsl", "main_ps", "ps_4_0", &blob);
        assert(SUCCEEDED(res));

        res = dxc.dev->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &dxc.pShader);
        assert(SUCCEEDED(res));
        blob->Release();
    }

    {
        D3D11_SAMPLER_DESC desc = {};
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        dxc.dev->CreateSamplerState(&desc, &dxc.samplerState);
    }

    {
        float vertices[] = {
            //float2 pos, float2 uv
            -1, -1, 0, 1,
             1,  1, 1, 0,
             1, -1, 1, 1,

            -1, -1, 0, 1,
            -1,  1, 0, 0,
             1,  1, 1, 0,
        };

        dxc.ctx->UpdateSubresource(dxc.vBuffer, 0, nullptr, vertices, sizeof(float[4]) * 6, 0);
    }
}

void updateUI() {
    if (!wk || !v) return;

    wk->Tick();
    
    //v->ForceInvalidateFullView();
    v->Paint();

    //v->SaveSurfacePNG("test.png");
}