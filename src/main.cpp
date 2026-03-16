#include <d3d11.h>
#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
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
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_CLIENT_API);
        if (glfwInit() == false) {
            printf("Error!\n");
        }

        window = glfwCreateWindow(800, 600, "DirectX", NULL, NULL);
        if (window == NULL) {
            printf("Failed to initialize window!\n");
        }
        HWND window_handle = glfwGetWin32Window(window);

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

        if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION,
                                                    &swap_chain_descriptor, &swap_chain, &device, NULL,
                                                    &device_context))) {
            printf("DirectX Initialized!\n");
        }

        swap_chain->GetBuffer(0U, __uuidof(ID3D11Resource), (void **)(&back_buffer));
        device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
    }

    while (glfwWindowShouldClose(window) == false) {
        glfwPollEvents();
        swap_chain->Present(1U, 0U);
        FLOAT color[4] = {(FLOAT)sin(glfwGetTime()), (FLOAT)cos(glfwGetTime()), (FLOAT)tan(glfwGetTime()), 1.0f};
        device_context->ClearRenderTargetView(render_target_view, color);
    }

    // Cleanup
    {
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
