#include "pch.h"
#include "Input.h"


Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
	m_quitApp = false;

	m_GameInput.forward		= false;
	m_GameInput.back		= false;
	m_GameInput.right		= false;
	m_GameInput.left		= false;
	m_GameInput.rotRight	= false;
	m_GameInput.rotLeft		= false;
	m_GameInput.up          = false;
	m_GameInput.down        = false;

 }

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	//A key
	if (kb.A)	m_GameInput.left = true;
	else		m_GameInput.left = false;

	//D key
	if (kb.D)	m_GameInput.right = true;
	else		m_GameInput.right = false;

	//W key
	if (kb.W)	m_GameInput.up = true;
	else		m_GameInput.up = false;

	//S key
	if (kb.S)	m_GameInput.down = true;
	else		m_GameInput.down = false;

	//left mouse
	if (kb.Space)   m_GameInput.forward = true;
	else                                m_GameInput.forward = false;

	//right mouse
	if (mouse.rightButton || kb.Tab)   m_GameInput.back = true;
	else                                m_GameInput.back = false;

	//space
	if (kb.G) m_GameInput.generate = true;
	else		m_GameInput.generate = false;

	//V key
	if (kb.V) m_GameInput.smoothing = true;
	else      m_GameInput.smoothing = false;

	//F1 key
	if (kb.F1) m_GameInput.blur = true;
	else      m_GameInput.blur = false;

	//F2 key
	if (kb.F2) m_GameInput.swirl = true;
	else      m_GameInput.swirl = false;

}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
