//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ImGui;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{

	m_input.Initialise(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    //setup imgui.  its up here cos we need the window handle too
    //pulled from imgui directx11 example
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window);		//tie to our window
    ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());	//tie to directx

    m_fullscreenRect.left = 0;
    m_fullscreenRect.top = 0;
    m_fullscreenRect.right = 800;
    m_fullscreenRect.bottom = 600;

    m_CameraViewRect.left = 500;
    m_CameraViewRect.top = 0;
    m_CameraViewRect.right = 800;
    m_CameraViewRect.bottom = 240;

	//setup light
	m_Light.setAmbientColour(0.7f, 0.7f, 0.7f, 0.5f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(2.0f, 1.0f, 1.0f);
	m_Light.setDirection(-1.0f, -1.0f, 0.0f);
  

    
    //XMVector3Normalize(camPos);
    //m_Light.setLookAt(camPos.x,camPos.y,camPos.z);

	//setup camera
	m_Camera01.setPosition(Vector3(-18.0f, 13.0f, 18.0f));
	m_Camera01.setRotation(Vector3(-90.0f, -230.0f, -160.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 
    m_Camera01.setTime(m_timer.GetFrameCount());
    

    m_Terrain.setTime(m_timer.GetFrameCount());
    //variable for wireframe

    auto device = m_deviceResources->GetD3DDevice();
    //m_Terrain.GenerateHeightMap(device);
    m_Terrain1.GenerateHeightMap(device);
    
	
#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<SoundEffect>(m_audEngine.get(), L"windy_forest.wav");
    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"beach_sound.wav");

    
    m_soundEffect1 = std::make_unique<SoundEffect>(m_audEngine.get(), L"birdsNoise.wav");
    m_soundEffect2 = std::make_unique<SoundEffect>(m_audEngine.get(), L"fireNoise.wav");
    m_soundEffect3 = std::make_unique<SoundEffect>(m_audEngine.get(), L"treesNoise.wav");
    m_soundEffect4 = std::make_unique<SoundEffect>(m_audEngine.get(), L"volleyballNoise.wav");
    //m_soundEffect5 = std::make_unique<SoundEffect>(m_audEngine.get(), L"dolphinNoise.wav");

    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance();
    m_effect3 = m_soundEffect1->CreateInstance();
    m_effect4 = m_soundEffect2->CreateInstance();
    m_effect5 = m_soundEffect3->CreateInstance();
    m_effect6 = m_soundEffect4->CreateInstance();
    //m_effect7 = m_soundEffect5->CreateInstance();

    m_effect1->Play(true);
    m_effect2->Play(true);
#endif


}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game
	
	//Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	//Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif	
}




// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{

    auto device = m_deviceResources->GetD3DDevice();

	//note that currently.  Delta-time is not considered in the game object movement. 
	if (m_gameInputCommands.left)
	{
		Vector3 rotation = m_Camera01.getRotation();
		rotation.y = rotation.y - m_Camera01.getRotationSpeed();
		m_Camera01.setRotation(rotation);
	}
	if (m_gameInputCommands.right)
	{
		Vector3 rotation = m_Camera01.getRotation();
		rotation.y = rotation.y + m_Camera01.getRotationSpeed();
		m_Camera01.setRotation(rotation);
	}
	if (m_gameInputCommands.forward)
	{
		Vector3 position = m_Camera01.getPosition(); //get the position
		position += (m_Camera01.getForward()*m_Camera01.getMoveSpeed()); //add the forward vector
		m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.back)
	{
		Vector3 position = m_Camera01.getPosition(); //get the position
		position -= (m_Camera01.getForward()*m_Camera01.getMoveSpeed()); //add the forward vector
		m_Camera01.setPosition(position);
	}
    if (m_gameInputCommands.up)
    {
        Vector3 rotation = m_Camera01.getRotation();
        rotation.x = rotation.x + m_Camera01.getRotationSpeed();
        m_Camera01.setRotation(rotation);
    }
    if (m_gameInputCommands.down)
    {
        Vector3 rotation = m_Camera01.getRotation();
        rotation.x = rotation.x - m_Camera01.getRotationSpeed();
        m_Camera01.setRotation(rotation);
    }
    if (m_gameInputCommands.generate)
    {

        m_Terrain1.GenerateHeightMap(device);
    }

    if (m_gameInputCommands.smoothing) {

        m_Terrain1.isUsingSmoothing = true;
        m_Terrain1.GenerateHeightMap(device);
    }

    m_Terrain1.Update(); 

    //m_Terrain.setTime(m_timer.GetFrameCount());

    m_Terrain.GenerateHeightMapWater(device);
    m_Terrain.Update();

	m_Camera01.Update(); //camera update.
    m_Light.setTime(m_timer.GetFrameCount());

    Vector3 camPos = m_Camera01.getPosition();
    m_Light.setCameraPos(camPos.x, camPos.y, camPos.z);

	m_view = m_Camera01.getCameraMatrix();
	m_world = Matrix::Identity;

    /*create our UI*/
    SetupGUI();


    if (m_gameInputCommands.blur) {
        //load and set up our Vertex and Pixel Shaders
        m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"GaussianBlur_ps.cso");    //light_ps
        m_BasicShaderPair1.InitStandard(device, L"skyLight_vs.cso", L"GaussianBlur_ps.cso");   //skyLight_ps
        m_BasicShaderPair2.InitStandard(device, L"skyLight_vs.cso", L"GaussianBlur_ps.cso");       //water_ps
    }
    else if (m_gameInputCommands.swirl) {
        //load and set up our Vertex and Pixel Shaders
        m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");    //light_ps
        m_BasicShaderPair1.InitStandard(device, L"skyLight_vs.cso", L"Swirl_ps.cso");   //skyLight_ps
        m_BasicShaderPair2.InitStandard(device, L"skyLight_vs.cso", L"water_ps.cso");       //water_ps
    }
    else {
        //load and set up our Vertex and Pixel Shaders
        m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");    //light_ps
        m_BasicShaderPair1.InitStandard(device, L"skyLight_vs.cso", L"skyLight_ps.cso");   //skyLight_ps
        m_BasicShaderPair2.InitStandard(device, L"skyLight_vs.cso", L"water_ps.cso");       //water_ps
    }

    //Collision detection

    m_boundSphereCamera.Center = m_Camera01.getPosition();
    
    //m_effect7->Play(false);
    
    //checking if the camera bounding sphere is colliding with the dragonball. 
    if (m_boundSphere1.Intersects(m_boundSphereCamera)) {
        m_effect3->Play(true);

    }
    else {
        m_effect3->Stop(true);
    }
    if (m_boundSphere2.Intersects(m_boundSphereCamera)) {
        m_effect4->Play(true);

    }
    else {
        m_effect4->Stop(true);
    }
    if (m_boundSphere3.Intersects(m_boundSphereCamera)) {
        m_effect5->Play(true);
    }
    else {
        m_effect5->Stop(true);
    }
    if (m_boundSphere4.Intersects(m_boundSphereCamera)) {
        m_effect6->Play(true);

    }
    else {
        m_effect6->Stop(true);
    }
    if (m_boundSphere5.Intersects(m_boundSphereCamera)) {
        //m_effect7->Play(true);

    }

    
    
    
    


#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
       /* else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }*/
    }
#endif

  
	if (m_input.Quit())
	{
		ExitGame();
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{	
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Draw Text to the screen
    m_deviceResources->PIXBeginEvent(L"Draw sprite");
    m_sprites->Begin();
		m_font->DrawString(m_sprites.get(), L"DirectXTK Demo Window", XMFLOAT2(10, 10), Colors::Yellow);
    m_sprites->End();
    m_deviceResources->PIXEndEvent();
	
	//Set Rendering states. 
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());
	//context->RSSetState(m_states->Wireframe());







    //if (*m_Terrain.Setwire(true) == true) {
    //    //context->RSSetState(m_states->Wireframe());
    //}

    




    //Skybox
	// Turn our shaders on,  set parameters
    SimpleMath::Matrix newScaleSky = SimpleMath::Matrix::CreateScale(150.0f);
    SimpleMath::Matrix newposSky = SimpleMath::Matrix::CreateTranslation(m_Camera01.getPosition());
    SimpleMath::Matrix newRotSky = SimpleMath::Matrix::CreateRotationY(0.0002f*m_timer.GetFrameCount());
    m_world = m_world * newScaleSky * newRotSky * newposSky;
	m_BasicShaderPair1.EnableShader(context);
	m_BasicShaderPair1.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get(), m_texture1.Get());

	//render our sphere skybox
    m_BasicModel.setRasterizerState(context, false);
	m_BasicModel.Render(context);

    //-----------------------------------------------------
  
    //Chairs

	//prepare transform for drone object.
 //   m_world = SimpleMath::Matrix::Identity; //set world back to identity
	//SimpleMath::Matrix newPosChair1 = SimpleMath::Matrix::CreateTranslation(2.0f, 0.0f, 0.0f);

 //   //Test code for quaternion interpolation
 //   Quaternion q0(1, 1, 1, 1);
 //   Quaternion q1(0, 0, 0, m_timer.GetElapsedSeconds());
 //   Quaternion q = XMQuaternionSlerp(q0, q1 , 0.1f);
 //   SimpleMath::Matrix m = XMMatrixRotationQuaternion(q);
 //  
 //   //ends here

 //   m_world = m_world * newPosChair1; //*m;
 //  

	////setup and draw chair object
	//m_BasicShaderPair.EnableShader(context);
	//m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture2.Get(), m_texture2.Get());
	//m_BasicModel2.Render(context);

 //   //prepare transform for chair object.
 //   m_world = SimpleMath::Matrix::Identity; //set world back to identity
 //   SimpleMath::Matrix newPositionChair2 = SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
 //   SimpleMath::Matrix newScaleChair2 = SimpleMath::Matrix::CreateScale(2.0f);
 //   m_world = m_world * newScaleChair2* newPositionChair2;

 //   //setup and draw chair object
 //   m_BasicShaderPair1.EnableShader(context);
 //   m_BasicShaderPair1.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture2.Get(), m_texture2.Get());
 //   m_BasicModel5.Render(context);

    //-----------------------------------------------------------


	//prepare transform for floor object(SAND). 
    

    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition2 = SimpleMath::Matrix::CreateTranslation(-20.0f, 0.0f, -100.0f);
    //SimpleMath::Matrix newScaleForSand = SimpleMath::Matrix::CreateScale(0.05f);
    m_world = m_world * newPosition2; //* newScaleForSand;

    //setup and draw cube
    m_BasicShaderPair1.EnableShader(context);
    m_BasicShaderPair1.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture3.Get(), m_texture3.Get());
    m_BasicModel3.setRasterizerState(context, true);
    m_BasicModel3.Render(context);








    //prepare transform for mountains
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionMountains = SimpleMath::Matrix::CreateTranslation(33.0f, 0.0f, -127.0f);
    SimpleMath::Matrix newScaleForSand = SimpleMath::Matrix::CreateScale(1.0f);
    m_world = m_world * newPositionMountains * newScaleForSand;

    //setup and draw cube
    m_BasicShaderPair1.EnableShader(context);
    m_BasicShaderPair1.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture3.Get(), m_texture3.Get());
    //m_BasicModel3.setRasterizerState(context, true);
    m_Terrain1.Render(context);







    //prepare transform for floor object(sea). 

    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPosition3 = SimpleMath::Matrix::CreateTranslation(-100.0f, 0.0f, 0.0f);
    SimpleMath::Matrix newScale = SimpleMath::Matrix::CreateScale(2.0f);		//scale the terrain down a little. 
    m_world = m_world * newScale * newPosition3;

    //setup and draw cube
    m_BasicShaderPair2.EnableShader(context);
    m_BasicShaderPair2.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture4.Get(), m_texture4.Get());
    m_Terrain.Render(context);



    //for (int i = -10; i < 10; i++) {
    //        m_world = SimpleMath::Matrix::Identity; //set world back to identity
    //        SimpleMath::Matrix newPosition2 = SimpleMath::Matrix::CreateTranslation(400.0f * i, 0.0f, 200.0f);
    //        //SimpleMath::Matrix newScaleForSand = SimpleMath::Matrix::CreateScale(0.05f);
    //        m_world = m_world * newPosition2; //* newScaleForSand;

    //        //setup and draw cube
    //        m_BasicShaderPair2.EnableShader(context);
    //        m_BasicShaderPair2.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture4.Get(), m_texture4.Get());
    //        m_BasicModel4.setRasterizerState(context, true);
    //        m_BasicModel4.Render(context);
    //}
      //--------------------------------------------------------------
    
    //prepare transform for bird object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionBirds = SimpleMath::Matrix::CreateTranslation(0.0f, 20.0f, 0.0f);
    SimpleMath::Matrix newScaleBirds = SimpleMath::Matrix::CreateScale(3.0f);
    m_world = m_world * newScaleBirds * newPositionBirds;

    //setup and draw bird object
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture18.Get(), m_texture18.Get());
    m_BasicModel9.setRasterizerState(context, true);
    m_BasicModel9.Render(context);

    //-------------------------------------------------------------------
     
    //Logs and stones, cottage and palms
    
    //prepare transform for logs object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionLogs = SimpleMath::Matrix::CreateTranslation(13.0f, 0.2f, -13.0f);
    SimpleMath::Matrix newScaleLogs = SimpleMath::Matrix::CreateScale(0.2f);
    m_world = m_world * newScaleLogs * newPositionLogs;

    //setup and draw logs object
    m_BasicShaderPair1.EnableShader(context);
    m_BasicShaderPair1.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture14.Get(), m_texture14.Get());
    m_BasicModel10.setRasterizerState(context, true);
    m_BasicModel10.Render(context);

    //prepare transform for stones object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionStones = SimpleMath::Matrix::CreateTranslation(13.0f, 0.2f, -13.0f);
    SimpleMath::Matrix newScaleStones = SimpleMath::Matrix::CreateScale(0.2f);
    m_world = m_world * newScaleStones * newPositionStones;

    //setup and draw stones object
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture13.Get(), m_texture13.Get());
    m_BasicModel11.setRasterizerState(context, true);
    m_BasicModel11.Render(context);

    //prepare transform for chair4 object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionChair4 = SimpleMath::Matrix::CreateTranslation(16.0f, 0.2f, -15.0f);
    SimpleMath::Matrix newScaleChair4 = SimpleMath::Matrix::CreateScale(0.02f);
    SimpleMath::Matrix newRotChair4 = SimpleMath::Matrix::CreateRotationX(-1.57f);

    m_world = m_world * newRotChair4 * newScaleChair4 * newPositionChair4;

    //setup and draw chair4 object
    m_BasicShaderPair1.EnableShader(context);
    m_BasicShaderPair1.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture12.Get(), m_texture12.Get());
    m_BasicModel13.setRasterizerState(context, true);
    m_BasicModel13.Render(context);

    //prepare transform for Plams object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionPalms = SimpleMath::Matrix::CreateTranslation(20.0f, 0.2f, -20.0f);
    SimpleMath::Matrix newRotPalms = SimpleMath::Matrix::CreateRotationY(1.0f);
    SimpleMath::Matrix newScalePalms = SimpleMath::Matrix::CreateScale(1.0f);
    m_world = m_world * newRotPalms * newScalePalms * newPositionPalms;

    //setup and draw Plams object
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture10.Get(), m_texture11.Get());
    m_BasicModel8.setRasterizerState(context, true);
    m_BasicModel8.Render(context);

    //--------------------------------------------------
    

    //man and boat
    // 
    //prepare transform for man object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionMan = SimpleMath::Matrix::CreateTranslation(5.0f, 1.2f, 4.5f);
    SimpleMath::Matrix newRotMan = SimpleMath::Matrix::CreateRotationY(-3.14f);
    SimpleMath::Matrix newScaleMan = SimpleMath::Matrix::CreateScale(1.0f);
    m_world = m_world * newRotMan* newPositionMan * newScaleMan ;

    //setup and draw Man object
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture9.Get(), m_texture9.Get());
    m_BasicModel7.setRasterizerState(context, true);
    m_BasicModel7.Render(context);

    //prepare transform for boat object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionBoat = SimpleMath::Matrix::CreateTranslation(5.0f, 0.9f, 5.0f);
    SimpleMath::Matrix newRotBoat = SimpleMath::Matrix::CreateRotationY(-1.57f);
    SimpleMath::Matrix newScaleBoat = SimpleMath::Matrix::CreateScale(1.2f);
    m_world = m_world * newRotBoat * newScaleBoat * newPositionBoat;

    //setup and draw boat object
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture6.Get(), m_texture7.Get());
    m_BasicModel6.setRasterizerState(context, true);
    m_BasicModel6.Render(context);

    //--------------------------------------------------

    //dolphins

    //prepare transform for dolphin object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionDolphin = SimpleMath::Matrix::CreateTranslation(-10.0f, 2.0f, 10.0f);
    SimpleMath::Matrix newScaleDolphin = SimpleMath::Matrix::CreateScale(1.2f);
    m_world = m_world * newScaleDolphin * newPositionDolphin;

    //setup and draw dolphin object
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture8.Get(), m_texture8.Get());
    m_BasicModel12.setRasterizerState(context, true);
    m_BasicModel12.Render(context);
    //--------------------------------------------------

    //volleyball, man and ball
    
    //prepare transform for man object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionMan1 = SimpleMath::Matrix::CreateTranslation(-12.0f, 0.2f, -12.0f);
    SimpleMath::Matrix newRotMan1 = SimpleMath::Matrix::CreateRotationY(-1.57f);
    SimpleMath::Matrix newScaleMan1 = SimpleMath::Matrix::CreateScale(1.2f);
    m_world = m_world * newRotMan1* newScaleMan1 * newPositionMan1;

    //setup and draw man object
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture9.Get(), m_texture15.Get());
    m_BasicModel14.Render(context);

    //prepare transform for man object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionMan2 = SimpleMath::Matrix::CreateTranslation(-13.0f, 0.2f, -14.0f);
    SimpleMath::Matrix newRotMan2 = SimpleMath::Matrix::CreateRotationY(-1.57f);
    SimpleMath::Matrix newScaleMan2 = SimpleMath::Matrix::CreateScale(1.2f);
    m_world = m_world * newRotMan2 * newScaleMan2 * newPositionMan2;

    //setup and draw man object
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture9.Get(), m_texture15.Get());
    m_BasicModel14.Render(context);

    //prepare transform for man object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionMan3 = SimpleMath::Matrix::CreateTranslation(-18.0f, 0.2f, -12.0f);
    SimpleMath::Matrix newRotMan3 = SimpleMath::Matrix::CreateRotationY(1.57f);
    SimpleMath::Matrix newScaleMan3 = SimpleMath::Matrix::CreateScale(1.2f);
    m_world = m_world * newRotMan3 * newScaleMan3 * newPositionMan3;

    //setup and draw man object
    m_BasicShaderPair1.EnableShader(context);
    m_BasicShaderPair1.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture9.Get(), m_texture15.Get());
    m_BasicModel14.setRasterizerState(context, true);
    m_BasicModel14.Render(context);

    //prepare transform for man object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionMan4 = SimpleMath::Matrix::CreateTranslation(-17.0f, 0.2f, -14.0f);
    SimpleMath::Matrix newRotMan4 = SimpleMath::Matrix::CreateRotationY(1.57f);
    SimpleMath::Matrix newScaleMan4 = SimpleMath::Matrix::CreateScale(1.2f);
    m_world = m_world * newRotMan4 * newScaleMan4 * newPositionMan4;

    //setup and draw man object
    m_BasicShaderPair1.EnableShader(context);
    m_BasicShaderPair1.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture9.Get(), m_texture15.Get());
    m_BasicModel14.setRasterizerState(context, true);
    m_BasicModel14.Render(context);

    //prepare transform for ball object.
    m_world = SimpleMath::Matrix::Identity; //set world back to identity
    SimpleMath::Matrix newPositionnet = SimpleMath::Matrix::CreateTranslation(-15.0f, 3.2f, -13.0f);
    SimpleMath::Matrix newRotnet = SimpleMath::Matrix::CreateRotationY(1.57f);
    SimpleMath::Matrix newScalenet = SimpleMath::Matrix::CreateScale(0.8f);
    m_world = m_world * newRotnet * newScalenet * newPositionnet;

    //setup and draw ball object
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light,  m_texture5.Get(), m_texture18.Get());
    m_BasicModel16.setRasterizerState(context, true);
    m_BasicModel16.Render(context);




    //render our GUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);


    //setup our terrain
    m_Terrain.Initialize(device, 128, 128);
    m_Terrain1.Initialize(device, 128, 128);


	//setup our test model
	m_BasicModel.InitializeSphere(device);

	m_BasicModel2.InitializeModel(device,"beach_chair1.obj");
	m_BasicModel5.InitializeModel(device,"chair3.obj");
	m_BasicModel6.InitializeModel(device,"boat.obj");
	m_BasicModel7.InitializeModel(device,"man.obj");
	m_BasicModel8.InitializeModel(device,"palms.obj");
	m_BasicModel9.InitializeModel(device,"birds.obj");
	m_BasicModel10.InitializeModel(device,"logs.obj");
	m_BasicModel11.InitializeModel(device,"stones.obj");
	m_BasicModel12.InitializeModel(device,"dolphins.obj");
	m_BasicModel13.InitializeModel(device,"chair4.obj");
	m_BasicModel14.InitializeModel(device,"1.obj"); //standing man
	m_BasicModel15.InitializeModel(device,"fence.obj");
	//m_BasicModel16.InitializeModel(device,"dolphins.obj");
	m_BasicModel3.InitializeBox(device, 130.0f, 0.1f, 200.0f); //sand box includes dimensions
    m_BasicModel4.InitializeBox(device, 400.0f, 0.1f, 400.0f); //sea box
    m_BasicModel16.InitializeSphere(device);




	////load and set up our Vertex and Pixel Shaders
	//m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");    //light_ps
	//m_BasicShaderPair1.InitStandard(device, L"skyLight_vs.cso", L"skyLight_ps.cso");   //skyLight_ps
	//m_BasicShaderPair2.InitStandard(device, L"skyLight_vs.cso", L"water_ps.cso");       //water_ps

	//load Textures
	CreateDDSTextureFromFile(device, L"sky.dds",		nullptr,	m_texture1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr,	m_texture2.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"newSand.dds", nullptr, m_texture3.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"water4.dds", nullptr, m_texture4.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"birdsTexture.dds", nullptr, m_texture5.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"firstBoatTexture.dds", nullptr, m_texture6.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"metal.dds", nullptr, m_texture7.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"dolphinsTexture.dds", nullptr, m_texture8.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"human.dds", nullptr, m_texture9.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"bark.dds", nullptr, m_texture10.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"leaf.dds", nullptr, m_texture11.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"chair1.dds", nullptr, m_texture12.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"cobble.dds", nullptr, m_texture13.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"trunk.dds", nullptr, m_texture14.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"human1.dds", nullptr, m_texture15.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"volleyBall.dds", nullptr, m_texture16.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"yellow.dds", nullptr, m_texture17.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"birdTex1.dds", nullptr, m_texture18.ReleaseAndGetAddressOf());


    //Bounding sphere centres

    m_boundSphere1.Center = Vector3(0.0f, 20.0f, 0.0f);   //Bird object
    m_boundSphere1.Radius = 3.0f;
    m_boundSphere2.Center = Vector3(13.0f, 0.2f, -13.0f);   //logs and fire
    m_boundSphere2.Radius = 1.0f;
    m_boundSphere3.Center = Vector3(20.0f, 0.2f, -20.0f);    //Palm Trees
    m_boundSphere3.Radius = 1.0f;
    m_boundSphere4.Center = Vector3(-15.0f, 1.7f, -14.0f);     //Man and ball
    m_boundSphere4.Radius = 2.0f;
    m_boundSphere5.Center = Vector3(-10.0f, 2.0f, 10.0f);    //Dolphins
    m_boundSphere5.Radius = 1.0f;



}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f
    );
}


void Game::SetupGUI()
{

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug");

    ImGui::SliderFloat("Enable smoothing", m_Terrain1.GetSmoothingValue(), 0.0f, 1.0f);

    //ImGui::SliderInt(m_Terrain1.GetMode(), m_Terrain1.GetGenModeNo(), 1, 4);

    if (ImGui::BeginMenu("Terrain Settings"))
    {

        if (ImGui::BeginMenu("Perlin Noise")) {
            m_Terrain1.mode_PerlinNoise == true;
            ImGui::SliderFloat("Wave Amplitude", m_Terrain1.GetAmplitude(), 0.0f, 10.0f);
            ImGui::SliderFloat("Wavelength", m_Terrain1.GetWavelength(), 0.0f, 1.0f);
            ImGui::SliderFloat("Noise Time", m_Terrain1.GetnoiseTime(), 0.0f, 100.0f);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Particle Decomposition")) {
            m_Terrain1.mode_FBM == true;
            ImGui::SliderInt("Particle Deposition Iterations", m_Terrain1.GetparticleDepositIterations(), 1000, 50000);
            ImGui::SliderFloat("Particle Deposition Displacement", m_Terrain1.GetparticleDisplacement(), -3.0f, 3.0f);
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }


    ImGui::End();
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
