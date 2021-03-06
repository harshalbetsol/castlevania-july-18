#include "CGame.h"
#include "GameLog.h"
#include <d3dtypes.h>
#include <stdlib.h>
#include "Camera.h"

CGame::CGame() : 
	m_handleWindow(NULL), 
	m_lpDirect3D(NULL), 
	m_lpDirect3DDevice(NULL), 
	m_lpSpriteDirect3DHandle(NULL),
	m_Input(NULL)
{
	
}

CGame::CGame(HINSTANCE hInstance, int scrWidth, int scrHeight, bool WndMode, int frameRate)
{

}

CGame::~CGame()
{
	SAFE_RELEASE(m_lpDirect3D)
	SAFE_RELEASE(m_lpDirect3DDevice)
	SAFE_DELETE(m_handleWindow)
}

bool CGame::InitializeHandleWindow(HINSTANCE hInstance)
{
	WNDCLASSEX WndcEx;
	ZeroMemory(&WndcEx, sizeof(WndcEx));
	WndcEx.cbClsExtra		= 0;
	WndcEx.cbSize			= sizeof(WNDCLASSEX);
	WndcEx.cbWndExtra		= 0;
	WndcEx.hbrBackground	= (HBRUSH)COLOR_BACKGROUND;
	WndcEx.hCursor			= LoadCursor(NULL, IDC_ARROW);
	WndcEx.hIcon			= NULL;
	WndcEx.hIconSm			= NULL;
	WndcEx.hInstance		= hInstance;
	WndcEx.lpfnWndProc		= (WNDPROC)WndProceduce;
	WndcEx.lpszClassName	= "CGame";
	WndcEx.lpszMenuName		= NULL;
	WndcEx.style			= NULL;

	if(!RegisterClassEx(&WndcEx))
	{
		CGameLog::GetInstance("CGame")->SaveError("Can't Regist the WndcEx!!!");
		return false;
	}

	//sprintf(fps, "%f", m_fps);

	m_handleWindow = CreateWindow(
		"CGame",
		GAMETITLE,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX,
		200,
		100,
		WIDTH,
		HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	if(!m_handleWindow)
	{
		CGameLog::GetInstance("CGame")->SaveError("Can't Create Window!");
		return false;
	}
	
	ShowWindow(m_handleWindow, SW_SHOW);
	UpdateWindow(m_handleWindow);
	//SetCursorPos(200, 100);
	return true;
}

bool CGame::InitializeDirect3DEnvironment()
{
	this->m_lpDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(!this->m_lpDirect3D)
	{
		CGameLog::GetInstance("CGame")->SaveError("Can't Create Direct3D Object!");
		return false;
	}

	return true;
}

bool CGame::InitializeDirect3DDevice(bool isWindowed)
{
	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	if (isWindowed)//if Play in Window Mode
	{
		d3dpp.BackBufferCount	= 1;
		d3dpp.BackBufferFormat	= D3DFMT_A8R8G8B8;
		d3dpp.BackBufferHeight	= 600;
		d3dpp.BackBufferWidth	= 800;
		d3dpp.Windowed			= true;
		d3dpp.hDeviceWindow		= this->m_handleWindow;
		d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD; 
	}
	else//if Play in Full ScreenMode
	{
		D3DDISPLAYMODE DisPlayMode;
		ZeroMemory(&DisPlayMode, sizeof(DisPlayMode));

		m_lpDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisPlayMode);

		HRESULT handleResult;
		handleResult = m_lpDirect3D->CheckDeviceFormat(
			D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_HAL, 
			DisPlayMode.Format, 
			D3DUSAGE_RENDERTARGET, 
			D3DRTYPE_SURFACE, 
			D3DFMT_A8R8G8B8);

		if(FAILED(handleResult))
		{
			CGameLog::GetInstance("CGame")->SaveError("Device format is unacceptable for full screen mode");
			return false;
		}

		d3dpp.BackBufferCount	= 1;
		d3dpp.BackBufferFormat	= D3DFMT_A8R8G8B8;
		d3dpp.Windowed			= false;
		d3dpp.BackBufferWidth	= DisPlayMode.Width;
		d3dpp.BackBufferHeight	= DisPlayMode.Height;
		d3dpp.hDeviceWindow		= this->m_handleWindow;
		d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD; 
	}

	m_lpDirect3D->CreateDevice(
			  D3DADAPTER_DEFAULT,
			  D3DDEVTYPE_HAL,//DON'T USE D3DDEVTYPE_REF BECAUSE YOU WILL REGRET IN REST OF YOUR LIFE EVERYTIME YOU PLAY THIS GAME
			  m_handleWindow,
			  D3DCREATE_HARDWARE_VERTEXPROCESSING,
			  &d3dpp,
			  &m_lpDirect3DDevice);

	if(FAILED(m_lpDirect3DDevice))
	{
		CGameLog::GetInstance("CGame")->SaveError("Can't create Direct3D Device");
		return false;
	}

	return true;
}

bool CGame::InitializDirect3DSpriteHandle()
{
	D3DXCreateSprite(m_lpDirect3DDevice, &m_lpSpriteDirect3DHandle);
	bool isSpriteCreated;
	isSpriteCreated = !FAILED(m_lpSpriteDirect3DHandle);

	if(!isSpriteCreated)
	{
		CGameLog::GetInstance("CGame")->SaveError("Can't create Direct3D Sprite Handle");
		return false;
	}
	return true;
}

bool CGame::Initialize(HINSTANCE hInstance, bool isWindowed)
{
	this->InitializeHandleWindow(hInstance);
	this->InitializeDirect3DEnvironment();
	this->InitializeDirect3DDevice(isWindowed);
	this->InitializDirect3DSpriteHandle();
	
	this->m_GameTime = new CGameTimeDx9();
	this->m_GameTime->InitGameTime();
	this->m_fps = 0;

	/*this->sprite = new CSpriteDx9(new D3DXVECTOR3(200, 200, 0),"resources/background1.png",0xFFFFFFFF, 1, 1, 1);*/
	//this->sprite->LoadContent(m_lpDirect3DDevice);
	/*this->Mario = new CSpriteDx9(new D3DXVECTOR3(64, 64, 0), "resources/simon.png", 0xFFFFFFFF, 8, 3, 24);*/
	//this->Mario->LoadContent(m_lpDirect3DDevice);
	/*this->Mario->getAnimation()->setStartFrame(0);
	this->Mario->getAnimation()->setEndFrame(2);*/
	this->m_Input = new CInputDx9();
	this->sprite = new CSpriteDx9();
	this->sprite->InitializeSpriteData(60, 66, 8, 3, 24);
	this->sprite->LoadTexture(m_lpDirect3DDevice, "resources/simon.png");

	this->Mario = new CSpriteDx9();
	this->Mario->InitializeSpriteData(1536, 384, 1, 1, 1);
	this->Mario->LoadTexture(m_lpDirect3DDevice, "resources/background1.png");

	this->m_Input->InitializeInput();
	this->m_Input->InitializeKeyBoardDevice(m_handleWindow);
	this->m_Input->InitializeMouseDevice(m_handleWindow);

	m_simon = new Simon();
	m_simon->InitializeData(sprite);
	return true;
}

void CGame::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	//	int _fps = 0;

	while(!CGlobal::IsExit)
	{
		#pragma region Quit Game
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}  
		#pragma endregion

		#pragma region Run Game
		else
		{

			m_GameTime->UpdateGameTime();
			m_fps += m_GameTime->getElapsedGameTime().getMilliseconds();

			m_Input->UpdateKeyBoard();
			m_Input->UpdateMouse();
			if( m_fps > 1000 / 60)
			{
				/*if((int)m_GameTime->getElapsedGameTime().getMilliseconds()%2 == 0)
				{
				m_lpDirect3DDevice->Clear(0 , 0,D3DCLEAR_TARGET,D3DCOLOR_XRGB( 0, 0, 0), 1.0f, 0); 
				}
				else
				{
				m_lpDirect3DDevice->Clear(0 , 0, D3DCLEAR_TARGET,D3DCOLOR_XRGB( 255, 255, 255), 1.0f, 0); 
				}*/
				m_lpDirect3DDevice->Clear(0 , 0,D3DCLEAR_TARGET,D3DCOLOR_XRGB( 0, 0, 0), 1.0f, 0); 
				if(m_lpDirect3DDevice->BeginScene())
				{
					m_lpSpriteDirect3DHandle->Begin(D3DXSPRITE_ALPHABLEND);
					Camera::GetInstance()->UpdateCamera(m_simon->GetPhysics()->GetLocation());
					/*sprite->UpdateAnimation(m_GameTime, 100);

					sprite->Render(m_lpSpriteDirect3DHandle, &D3DXVECTOR3(0, 0, 0), SpriteEffect::None);*/
					m_simon->GetInput(m_Input);
					m_simon->UpdateAnimation(m_GameTime);
					m_simon->UpdateMovement(m_GameTime);

					Mario->Render(m_lpSpriteDirect3DHandle, &D3DXVECTOR3(0.0f, 50.0f, 0.0f));
					m_simon->Render(m_lpSpriteDirect3DHandle, NULL);

					m_lpSpriteDirect3DHandle->End();

					m_lpDirect3DDevice->EndScene();
				}
				m_lpDirect3DDevice->Present( 0, 0, 0, 0);
				m_fps = 0;
			}
		}	  
		#pragma endregion

	}
}

void CGame::Exit()
{
	SAFE_RELEASE(m_lpDirect3D)
	SAFE_RELEASE(m_lpDirect3DDevice)
}

LRESULT CALLBACK CGame::WndProceduce(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message,wParam, lParam);
}