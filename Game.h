//
// Game.h
//
#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Shader.h"
#include "modelclass.h"
#include "Light.h"
#include "Input.h"
#include "Camera.h"
#include "RenderTexture.h"
#include "Terrain.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);
#ifdef DXTK_AUDIO
    void NewAudioDevice();
#endif

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;
	
private:

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
        //float time;
	}; 

    void Update(DX::StepTimer const& timer);
    void Render();
    void Clear();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
    void SetupGUI();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    

	//input manager. 
	Input									m_input;
	InputCommands							m_gameInputCommands;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;	
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

	// Scene Objects
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

	//lights
	Light																	m_Light;

	//Cameras
	Camera																	m_Camera01;

	//textures 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture6;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture7;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture8;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture9;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture10;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture11;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture12;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture13;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture14;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture15;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture16;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture17;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture18;

	//Shaders
	Shader																	m_BasicShaderPair;
	Shader																	m_BasicShaderPair1;
	Shader																	m_BasicShaderPair2;

    Terrain                                                                 m_Terrain;
    Terrain                                                                 m_Terrain1;

	ModelClass																m_BasicModel;
	ModelClass																m_BasicModel2;
	ModelClass																m_BasicModel3;
    ModelClass																m_BasicModel4;
    ModelClass																m_BasicModel5;
    ModelClass																m_BasicModel6;
    ModelClass																m_BasicModel7;
    ModelClass																m_BasicModel8;
    ModelClass																m_BasicModel9;
    ModelClass																m_BasicModel10;
    ModelClass																m_BasicModel11;
    ModelClass																m_BasicModel12;
    ModelClass																m_BasicModel13;
    ModelClass																m_BasicModel14;
    ModelClass																m_BasicModel15;
    ModelClass																m_BasicModel16;


    //RenderTextures
    RenderTexture*                                                          m_FirstRenderPass;
    RECT																	m_fullscreenRect;
    RECT																	m_CameraViewRect;

    //Bounding spheres
    BoundingSphere                                                          m_boundSphere1;
    BoundingSphere                                                          m_boundSphere2;
    BoundingSphere                                                          m_boundSphere3;
    BoundingSphere                                                          m_boundSphere4;
    BoundingSphere                                                          m_boundSphere5;
    BoundingSphere                                                          m_boundSphereCamera;




#ifdef DXTK_AUDIO
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::SoundEffect>                                   m_waveBank;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect1;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect2;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect3;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect4;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect5;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect3;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect4;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect5;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect6;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect7;
#endif
    

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;
};