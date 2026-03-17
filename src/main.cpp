#include <d3d11.h>
#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <d3dcompiler.h>
#include <math.h>

int
main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    GLFWwindow *window = NULL;
    IDXGISwapChain *swap_chain = NULL;
    ID3D11Device *device = NULL;
    ID3D11DeviceContext *device_context = NULL;
    ID3D11Resource *back_buffer = NULL;
    ID3D11RenderTargetView *render_target_view = NULL;

    DXGI_SWAP_CHAIN_DESC swap_chain_descriptor = {};

    // Init Stuff
    glfwWindowHint(GLFW_CLIENT_API, GLFW_CLIENT_API);
    if (glfwInit() == false) {
        printf("Error!\n");
    }

    window = glfwCreateWindow(800, 600, "DirectX", NULL, NULL);
    if (window == NULL) {
        printf("Failed to initialize window!\n");
    }
    HWND window_handle = glfwGetWin32Window(window);
    if (window_handle == INVALID_HANDLE_VALUE) {
        printf("Invalid windows window handle!\n");
    }

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

    if (FAILED(swap_chain->GetBuffer(0U, __uuidof(ID3D11Resource), (void **)(&back_buffer)))) {
        printf("Failed to get swap chain buffer!\n");
    }
    if (FAILED(device->CreateRenderTargetView(back_buffer, NULL, &render_target_view))) {
        printf("Failed to create render target view!\n");
    }

    struct Vertex {
        float x, y;
    };

    const Vertex vertices[] = {{0.0f, 0.5f}, {0.5f, -0.5f}, {-0.5f, -0.5f}};
    ID3D11Buffer *vertex_buffer = NULL;

    D3D11_BUFFER_DESC buffer_descriptor = {};
    buffer_descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_descriptor.Usage = D3D11_USAGE_DEFAULT;
    buffer_descriptor.CPUAccessFlags = 0U;
    buffer_descriptor.MiscFlags = 0U;
    buffer_descriptor.ByteWidth = sizeof(vertices);
    buffer_descriptor.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA subresource_data = {};
    subresource_data.pSysMem = vertices;

    device->CreateBuffer(&buffer_descriptor, &subresource_data, &vertex_buffer);
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0U;
    device_context->IASetVertexBuffers(0U, 1U, &vertex_buffer, &stride, &offset);

    ID3DBlob *blob = NULL;

    // Create Pixel Shader
    ID3D11PixelShader *pixel_shader = NULL;
    D3DReadFileToBlob(L"ps.cso", &blob);
    device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pixel_shader);
    device_context->PSSetShader(pixel_shader, NULL, 0U);

    // Create Vertex Shader
    ID3D11VertexShader *vertex_shader = NULL;
    D3DReadFileToBlob(L"vs.cso", &blob);
    device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &vertex_shader);
    device_context->VSSetShader(vertex_shader, NULL, 0U);

    ID3D11InputLayout *input_layout = NULL;
    D3D11_INPUT_ELEMENT_DESC input_element_descriptor = {};
    input_element_descriptor.SemanticName = "Position";
    input_element_descriptor.SemanticIndex = 0U;
    input_element_descriptor.Format = DXGI_FORMAT_R32G32_FLOAT;
    input_element_descriptor.InputSlot = 0U;
    input_element_descriptor.AlignedByteOffset = 0;
    input_element_descriptor.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    input_element_descriptor.InstanceDataStepRate = 0U;

    device->CreateInputLayout(&input_element_descriptor, 1, blob->GetBufferPointer(), blob->GetBufferSize(),
                              &input_layout);

    device_context->IASetInputLayout(input_layout);

    device_context->OMSetRenderTargets(1U, &render_target_view, NULL);
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT view_port = {};
    view_port.Width = 800.0f;
    view_port.Height = 600.0f;
    view_port.TopLeftX = 0.0f;
    view_port.TopLeftY = 0.0f;
    view_port.MinDepth = 0.0f;
    view_port.MaxDepth = 1.0f;

    device_context->RSSetViewports(1U, &view_port);

    while (glfwWindowShouldClose(window) == false) {
        glfwPollEvents();

        // Present
        swap_chain->Present(1U, 0U);

        // Clear Background
        FLOAT color[4] = {(FLOAT)sin(glfwGetTime()), (FLOAT)cos(glfwGetTime()), (FLOAT)tan(glfwGetTime()), 1.0f};
        device_context->ClearRenderTargetView(render_target_view, color);
        device_context->Draw(3U, 0U);
    }

    // Cleanup
    {
        if (render_target_view != NULL) {
            render_target_view->Release();
        }
        if (back_buffer != NULL) {
            back_buffer->Release();
        }
        if (device_context != NULL) {
            device_context->Release();
        }
        if (device != NULL) {
            device->Release();
        }
        if (swap_chain != NULL) {
            swap_chain->Release();
        }
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    return 0;
}
