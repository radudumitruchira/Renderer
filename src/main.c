#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <math.h>
#include <stdio.h>

typedef struct Vertex {
    float x, y;
} Vertex;

static GLFWwindow *window = NULL;
static HWND window_handle = NULL;
static IDXGISwapChain *swap_chain = NULL;
static ID3D11Device *device = NULL;
static ID3D11DeviceContext *device_context = NULL;
static ID3D11Resource *back_buffer = NULL;
static ID3D11RenderTargetView *render_target_view = NULL;

static DXGI_SWAP_CHAIN_DESC swap_chain_descriptor = {0};

static void
InitWindow() {

    glfwWindowHint(GLFW_CLIENT_API, GLFW_CLIENT_API);
    if (glfwInit() == FALSE) {
        printf("Error!\n");
    }

    window = glfwCreateWindow(800, 600, "DirectX", NULL, NULL);
    if (window == NULL) {
        printf("Failed to initialize window!\n");
    }
    window_handle = glfwGetWin32Window(window);
    if (window_handle == INVALID_HANDLE_VALUE) {
        printf("Invalid windows window handle!\n");
    }
}

static void
InitDirectX() {
    swap_chain_descriptor.BufferDesc.Width = 0;
    swap_chain_descriptor.BufferDesc.Height = 0;
    swap_chain_descriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swap_chain_descriptor.BufferDesc.RefreshRate.Numerator = 0;
    swap_chain_descriptor.BufferDesc.RefreshRate.Denominator = 0;
    swap_chain_descriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_descriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_descriptor.BufferDesc.RefreshRate.Denominator = 0;
    swap_chain_descriptor.SampleDesc.Count = 1;
    swap_chain_descriptor.SampleDesc.Quality = 0;
    swap_chain_descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_descriptor.BufferCount = 1;
    swap_chain_descriptor.OutputWindow = window_handle;
    swap_chain_descriptor.Windowed = TRUE;
    swap_chain_descriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_descriptor.Flags = 0;

    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0,
                                             D3D11_SDK_VERSION, &swap_chain_descriptor, &swap_chain, &device, NULL,
                                             &device_context))) {
        printf("DirectX failed to initialize!\n");
    }
    if (FAILED(swap_chain->lpVtbl->GetBuffer(swap_chain, 0U, &IID_ID3D11Resource, (void **)(&back_buffer)))) {
        printf("Failed to get swap chain buffer!\n");
    }
    if (FAILED(device->lpVtbl->CreateRenderTargetView(device, back_buffer, NULL, &render_target_view))) {
        printf("Failed to create render target view!\n");
    }
}

int
main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Init Stuff
    InitWindow();
    InitDirectX();

    const Vertex vertices[] = {{0.0f, 0.5f}, {0.5f, -0.5f}, {-0.5f, -0.5f}};
    ID3D11Buffer *vertex_buffer = NULL;

    D3D11_BUFFER_DESC buffer_descriptor = {0};
    buffer_descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_descriptor.Usage = D3D11_USAGE_DEFAULT;
    buffer_descriptor.CPUAccessFlags = 0U;
    buffer_descriptor.MiscFlags = 0U;
    buffer_descriptor.ByteWidth = sizeof(vertices);
    buffer_descriptor.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA subresource_data = {0};
    subresource_data.pSysMem = vertices;

    device->lpVtbl->CreateBuffer(device, &buffer_descriptor, &subresource_data, &vertex_buffer);
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0U;
    device_context->lpVtbl->IASetVertexBuffers(device_context, 0U, 1U, &vertex_buffer, &stride, &offset);

    ID3DBlob *blob = NULL;

    // Create Pixel Shader
    ID3D11PixelShader *pixel_shader = NULL;
    D3DReadFileToBlob(L"ps.cso", &blob);
    device->lpVtbl->CreatePixelShader(device, blob->lpVtbl->GetBufferPointer(blob), blob->lpVtbl->GetBufferSize(blob),
                                      NULL, &pixel_shader);
    device_context->lpVtbl->PSSetShader(device_context, pixel_shader, NULL, 0U);

    // Create Vertex Shader
    ID3D11VertexShader *vertex_shader = NULL;
    D3DReadFileToBlob(L"vs.cso", &blob);
    device->lpVtbl->CreateVertexShader(device, blob->lpVtbl->GetBufferPointer(blob), blob->lpVtbl->GetBufferSize(blob),
                                       NULL, &vertex_shader);
    device_context->lpVtbl->VSSetShader(device_context, vertex_shader, NULL, 0U);

    ID3D11InputLayout *input_layout = NULL;
    D3D11_INPUT_ELEMENT_DESC input_element_descriptor = {0};
    input_element_descriptor.SemanticName = "Position";
    input_element_descriptor.SemanticIndex = 0U;
    input_element_descriptor.Format = DXGI_FORMAT_R32G32_FLOAT;
    input_element_descriptor.InputSlot = 0U;
    input_element_descriptor.AlignedByteOffset = 0;
    input_element_descriptor.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    input_element_descriptor.InstanceDataStepRate = 0U;

    device->lpVtbl->CreateInputLayout(device, &input_element_descriptor, 1, blob->lpVtbl->GetBufferPointer(blob),
                                      blob->lpVtbl->GetBufferSize(blob), &input_layout);

    device_context->lpVtbl->IASetInputLayout(device_context, input_layout);

    device_context->lpVtbl->OMSetRenderTargets(device_context, 1U, &render_target_view, NULL);
    device_context->lpVtbl->IASetPrimitiveTopology(device_context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT view_port = {0};
    view_port.Width = 800.0f;
    view_port.Height = 600.0f;
    view_port.TopLeftX = 0.0f;
    view_port.TopLeftY = 0.0f;
    view_port.MinDepth = 0.0f;
    view_port.MaxDepth = 1.0f;

    device_context->lpVtbl->RSSetViewports(device_context, 1U, &view_port);

    while (glfwWindowShouldClose(window) == FALSE) {
        glfwPollEvents();

        // Present
        swap_chain->lpVtbl->Present(swap_chain, 1U, 0U);

        // Clear Background
        FLOAT color[4] = {(FLOAT)sin(glfwGetTime()), (FLOAT)cos(glfwGetTime()), (FLOAT)tan(glfwGetTime()), 1.0f};
        device_context->lpVtbl->ClearRenderTargetView(device_context, render_target_view, color);
        device_context->lpVtbl->Draw(device_context, 3U, 0U);
    }

    // Cleanup
    {
        if (render_target_view != NULL) {
            render_target_view->lpVtbl->Release(render_target_view);
        }
        if (back_buffer != NULL) {
            back_buffer->lpVtbl->Release(back_buffer);
        }
        if (device_context != NULL) {
            device_context->lpVtbl->Release(device_context);
        }
        if (device != NULL) {
            device->lpVtbl->Release(device);
        }
        if (swap_chain != NULL) {
            swap_chain->lpVtbl->Release(swap_chain);
        }
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    return 0;
}
