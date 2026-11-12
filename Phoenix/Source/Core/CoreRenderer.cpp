#include "Phoenix.h"
#include "CoreRenderer.h"

#include <stdexcept>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")	// runtime FXC compile (SM 5.1) - swap for DXC when moving to SM 6

using Microsoft::WRL::ComPtr;

// Logs the failing expression + HRESULT, then throws.
#define PHX_THROW_IF_FAILED(expr)                                                   \
	do {                                                                            \
		const HRESULT phx_hr = (expr);                                              \
		if (FAILED(phx_hr)) {                                                       \
			Logger::PrintLog(L"D3D12 failure 0x%08X in '%S' (%S:%d)\n",             \
				static_cast<UINT>(phx_hr), #expr, __FILE__, __LINE__);              \
			throw std::runtime_error("Direct3D call failed");                       \
		}                                                                           \
	} while (0)

namespace
{
	constexpr DXGI_FORMAT	kBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	// Same relative-path convention as PerProjectSettings::SplashURL.
	constexpr WCHAR			kShaderPath[] = L"..\\Phoenix\\Assets\\Shaders\\Triangle.hlsl";

	ComPtr<ID3DBlob> CompileShader(const char* entry, const char* target)
	{
		UINT flags = 0;
#if defined(_DEBUG)
		flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		ComPtr<ID3DBlob> code;
		ComPtr<ID3DBlob> errors;
		const HRESULT hr = D3DCompileFromFile(kShaderPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entry, target, flags, 0, &code, &errors);
		if (FAILED(hr))
		{
			if (errors)
				Logger::PrintLog(L"Shader compile error (%S): %.1500S\n", entry,
					static_cast<const char*>(errors->GetBufferPointer()));
			Logger::PrintLog(L"D3DCompileFromFile failed 0x%08X for %s\n", static_cast<UINT>(hr), kShaderPath);
			throw std::runtime_error("Shader compilation failed");
		}
		return code;
	}
}

CoreRenderer::CoreRenderer()
{
}

CoreRenderer::~CoreRenderer()
{
	Shutdown();
}

// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------

VOID CoreRenderer::Initialize(HWND hwnd)
{
	m_Hwnd = hwnd;

	RECT rc = {};
	GetClientRect(hwnd, &rc);
	m_Width = static_cast<UINT>(rc.right - rc.left);
	m_Height = static_cast<UINT>(rc.bottom - rc.top);
	if (m_Width == 0)  m_Width = 1;
	if (m_Height == 0) m_Height = 1;

	EnableDiagnostics();		// must precede device creation
	CreateDevice();
	ConfigureInfoQueue();
	CreateQueueAndSwapChain();
	CreateRenderTargets();
	CreateCommandObjects();
	CreatePipeline();
	CreateVertexBuffer();
	UpdateViewport();

	m_Initialized = TRUE;
	Logger::PrintLog(L"Direct3D 12 renderer initialized (%u x %u).\n", m_Width, m_Height);
}

VOID CoreRenderer::EnableDiagnostics()
{
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
		debug->EnableDebugLayer();

	ComPtr<ID3D12DeviceRemovedExtendedDataSettings> dred;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dred))))
	{
		dred->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		dred->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
	}
#endif
}

VOID CoreRenderer::CreateDevice()
{
	UINT factoryFlags = 0;
#if defined(_DEBUG)
	factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	PHX_THROW_IF_FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_Factory)));

	ComPtr<IDXGIAdapter1> adapter;
	for (UINT i = 0; m_Factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 desc = {};
		adapter->GetDesc1(&desc);

		if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) &&
			SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			Logger::PrintLog(L"Adapter: %s\n", desc.Description);
			break;
		}
		adapter.Reset();
	}

	if (!adapter)
	{
		Logger::PrintLog(L"No hardware D3D12 adapter found, falling back to WARP.\n");
		PHX_THROW_IF_FAILED(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
	}

	PHX_THROW_IF_FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
	m_Device->SetName(L"Phoenix D3D12 device");
}

VOID CoreRenderer::ConfigureInfoQueue()
{
#if defined(_DEBUG)
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(m_Device.As(&infoQueue)))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		// Warnings are logged to the debugger output but do not break; enable if you want it strict.
	}
#endif
}

VOID CoreRenderer::CreateQueueAndSwapChain()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	PHX_THROW_IF_FAILED(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));
	m_CommandQueue->SetName(L"Direct queue");

	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = m_Width;
	desc.Height = m_Height;
	desc.Format = kBackBufferFormat;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = FrameCount;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	ComPtr<IDXGISwapChain1> swapChain;
	// The swap chain takes the COMMAND QUEUE, not the device.
	PHX_THROW_IF_FAILED(m_Factory->CreateSwapChainForHwnd(m_CommandQueue.Get(), m_Hwnd, &desc, nullptr, nullptr, &swapChain));
	PHX_THROW_IF_FAILED(m_Factory->MakeWindowAssociation(m_Hwnd, DXGI_MWA_NO_ALT_ENTER));
	PHX_THROW_IF_FAILED(swapChain.As(&m_SwapChain));
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

VOID CoreRenderer::CreateRenderTargets()
{
	if (!m_RtvHeap)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = FrameCount;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		PHX_THROW_IF_FAILED(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_RtvHeap)));
		m_RtvHeap->SetName(L"Back-buffer RTVs");
		m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < FrameCount; ++i)
	{
		PHX_THROW_IF_FAILED(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])));
		m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, handle);
		handle.Offset(1, m_RtvDescriptorSize);
	}
}

VOID CoreRenderer::CreateCommandObjects()
{
	for (auto& frame : m_Frames)
	{
		PHX_THROW_IF_FAILED(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame.Allocator)));
	}

	PHX_THROW_IF_FAILED(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_Frames[0].Allocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));
	PHX_THROW_IF_FAILED(m_CommandList->Close());
	m_CommandList->SetName(L"Main graphics command list");

	PHX_THROW_IF_FAILED(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_FenceEvent)
		PHX_THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
}

VOID CoreRenderer::CreatePipeline()
{
	// Root signature: empty, only allows the input assembler layout.
	CD3DX12_ROOT_SIGNATURE_DESC rootDesc;
	rootDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	PHX_THROW_IF_FAILED(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	PHX_THROW_IF_FAILED(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));

	ComPtr<ID3DBlob> vs = CompileShader("VSMain", "vs_5_1");
	ComPtr<ID3DBlob> ps = CompileShader("PSMain", "ps_5_1");

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
	pso.InputLayout = { inputLayout, _countof(inputLayout) };
	pso.pRootSignature = m_RootSignature.Get();
	pso.VS = CD3DX12_SHADER_BYTECODE(vs.Get());
	pso.PS = CD3DX12_SHADER_BYTECODE(ps.Get());
	pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso.DepthStencilState.DepthEnable = FALSE;
	pso.DepthStencilState.StencilEnable = FALSE;
	pso.SampleMask = UINT_MAX;
	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso.NumRenderTargets = 1;
	pso.RTVFormats[0] = kBackBufferFormat;
	pso.SampleDesc.Count = 1;
	PHX_THROW_IF_FAILED(m_Device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&m_PipelineState)));
}

VOID CoreRenderer::CreateVertexBuffer()
{
	const Vertex vertices[] =
	{
		{ {  0.0f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};
	const UINT size = sizeof(vertices);

	// Named locals: taking the address of a temporary does not compile with /permissive- (ConformanceMode).
	const CD3DX12_HEAP_PROPERTIES	defaultHeap(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_HEAP_PROPERTIES	uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC		bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	PHX_THROW_IF_FAILED(m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_VertexBuffer)));
	m_VertexBuffer->SetName(L"Triangle vertex buffer");

	ComPtr<ID3D12Resource> upload;
	PHX_THROW_IF_FAILED(m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&upload)));

	void* mapped = nullptr;
	const CD3DX12_RANGE noRead(0, 0);
	PHX_THROW_IF_FAILED(upload->Map(0, &noRead, &mapped));
	memcpy(mapped, vertices, size);
	upload->Unmap(0, nullptr);

	PHX_THROW_IF_FAILED(m_Frames[0].Allocator->Reset());
	PHX_THROW_IF_FAILED(m_CommandList->Reset(m_Frames[0].Allocator.Get(), nullptr));
	m_CommandList->CopyBufferRegion(m_VertexBuffer.Get(), 0, upload.Get(), 0, size);
	const auto toVertexBuffer = CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	m_CommandList->ResourceBarrier(1, &toVertexBuffer);
	PHX_THROW_IF_FAILED(m_CommandList->Close());

	ID3D12CommandList* lists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(lists), lists);

	// The upload heap must stay alive until the copy has finished: drain before it goes out of scope.
	WaitForGpu();

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = size;
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
}

VOID CoreRenderer::UpdateViewport()
{
	m_Viewport = { 0.0f, 0.0f, static_cast<FLOAT>(m_Width), static_cast<FLOAT>(m_Height), 0.0f, 1.0f };
	m_ScissorRect = { 0, 0, static_cast<LONG>(m_Width), static_cast<LONG>(m_Height) };
}

// ---------------------------------------------------------------------------
// Frame
// ---------------------------------------------------------------------------

VOID CoreRenderer::PopulateCommandList()
{
	FrameContext& frame = m_Frames[m_FrameIndex];
	PHX_THROW_IF_FAILED(frame.Allocator->Reset());
	PHX_THROW_IF_FAILED(m_CommandList->Reset(frame.Allocator.Get(), m_PipelineState.Get()));

	// A reset list forgets everything: bind state again every frame.
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	const auto toRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &toRenderTarget);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
	m_CommandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->DrawInstanced(3, 1, 0, 0);

	const auto toPresent = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_CommandList->ResourceBarrier(1, &toPresent);

	PHX_THROW_IF_FAILED(m_CommandList->Close());
}

VOID CoreRenderer::Render()
{
	if (!m_Initialized || m_Paused)
		return;

	PopulateCommandList();

	ID3D12CommandList* lists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(lists), lists);

	// Tag this frame slot with the work that used it.
	const UINT64 submitted = m_NextFenceValue++;
	PHX_THROW_IF_FAILED(m_CommandQueue->Signal(m_Fence.Get(), submitted));
	m_Frames[m_FrameIndex].FenceValue = submitted;

	const HRESULT hr = m_SwapChain->Present(1, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		Logger::PrintLog(L"Device removed, reason: 0x%08X\n", static_cast<UINT>(m_Device->GetDeviceRemovedReason()));
	PHX_THROW_IF_FAILED(hr);

	// Only block if the slot we are about to record into is still in flight.
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	WaitForFenceValue(m_Frames[m_FrameIndex].FenceValue);
}

VOID CoreRenderer::WaitForFenceValue(UINT64 value)
{
	if (m_Fence->GetCompletedValue() < value)
	{
		PHX_THROW_IF_FAILED(m_Fence->SetEventOnCompletion(value, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

// Full drain: used for init, resize and shutdown only - not per frame.
VOID CoreRenderer::WaitForGpu()
{
	if (!m_CommandQueue || !m_Fence || !m_FenceEvent)
		return;

	const UINT64 value = m_NextFenceValue++;
	PHX_THROW_IF_FAILED(m_CommandQueue->Signal(m_Fence.Get(), value));
	WaitForFenceValue(value);
}

// ---------------------------------------------------------------------------
// Resize / shutdown
// ---------------------------------------------------------------------------

VOID CoreRenderer::Resize(UINT width, UINT height)
{
	if (!m_Initialized)
		return;

	if (width == 0 || height == 0)
	{
		m_Paused = TRUE;		// minimised: keep the old buffers, skip rendering
		return;
	}
	m_Paused = FALSE;

	if (width == m_Width && height == m_Height)
		return;

	WaitForGpu();

	// Every reference to the back buffers must be gone before ResizeBuffers.
	for (auto& rt : m_RenderTargets)
		rt.Reset();

	const UINT64 drained = m_NextFenceValue - 1;
	for (auto& frame : m_Frames)
		frame.FenceValue = drained;

	PHX_THROW_IF_FAILED(m_SwapChain->ResizeBuffers(FrameCount, width, height, kBackBufferFormat, 0));

	m_Width = width;
	m_Height = height;
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargets();
	UpdateViewport();
}

VOID CoreRenderer::Shutdown()
{
	m_Initialized = FALSE;

	try { WaitForGpu(); }
	catch (...) {}

	if (m_FenceEvent)
	{
		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;
	}

	m_VertexBuffer.Reset();
	m_PipelineState.Reset();
	m_RootSignature.Reset();
	m_CommandList.Reset();
	for (auto& frame : m_Frames)
		frame.Allocator.Reset();
	for (auto& rt : m_RenderTargets)
		rt.Reset();
	m_RtvHeap.Reset();
	m_Fence.Reset();
	m_SwapChain.Reset();
	m_CommandQueue.Reset();
	m_Device.Reset();
	m_Factory.Reset();
}