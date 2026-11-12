#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"

// ComPtr members inside an exported class: harmless here, the members are private.
#pragma warning(push)
#pragma warning(disable : 4251)

class PHOENIX_API CoreRenderer
{
public:
	enum : UINT { FrameCount = 2 };

	CoreRenderer();
	~CoreRenderer();

	// Throws std::runtime_error on failure (details are written to the log).
	VOID	Initialize(HWND hwnd);
	VOID	Render();
	VOID	Resize(UINT width, UINT height);
	VOID	Shutdown();

	BOOL	IsInitialized() const { return m_Initialized; }

private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	struct Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
	};

	struct FrameContext
	{
		ComPtr<ID3D12CommandAllocator>	Allocator;
		UINT64							FenceValue = 0;
	};

	// Setup (cookbook tasks 2-9)
	static VOID EnableDiagnostics();
	VOID	CreateDevice();
	VOID	ConfigureInfoQueue();
	VOID	CreateQueueAndSwapChain();
	VOID	CreateRenderTargets();
	VOID	CreateCommandObjects();
	VOID	CreatePipeline();
	VOID	CreateVertexBuffer();
	VOID	UpdateViewport();

	// Per frame (tasks 7 and 13)
	VOID	PopulateCommandList();
	VOID	WaitForFenceValue(UINT64 value);
	VOID	WaitForGpu();

private:
	HWND	m_Hwnd = nullptr;
	UINT	m_Width = 0;
	UINT	m_Height = 0;
	BOOL	m_Initialized = FALSE;
	BOOL	m_Paused = FALSE;	// window minimised / zero-sized

	ComPtr<IDXGIFactory4>				m_Factory;
	ComPtr<ID3D12Device>				m_Device;
	ComPtr<ID3D12CommandQueue>			m_CommandQueue;
	ComPtr<IDXGISwapChain3>				m_SwapChain;
	ComPtr<ID3D12DescriptorHeap>		m_RtvHeap;
	ComPtr<ID3D12Resource>				m_RenderTargets[FrameCount];
	UINT								m_RtvDescriptorSize = 0;

	FrameContext						m_Frames[FrameCount];
	ComPtr<ID3D12GraphicsCommandList>	m_CommandList;
	ComPtr<ID3D12Fence>					m_Fence;
	HANDLE								m_FenceEvent = nullptr;
	UINT64								m_NextFenceValue = 1;
	UINT								m_FrameIndex = 0;

	D3D12_VIEWPORT						m_Viewport = {};
	D3D12_RECT							m_ScissorRect = {};

	ComPtr<ID3D12RootSignature>			m_RootSignature;
	ComPtr<ID3D12PipelineState>			m_PipelineState;
	ComPtr<ID3D12Resource>				m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_VertexBufferView = {};
};

#pragma warning(pop)