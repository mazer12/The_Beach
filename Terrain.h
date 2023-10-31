#pragma once

using namespace DirectX;

class Terrain
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};

public:
	Terrain();
	~Terrain();

	bool Initialize(ID3D11Device*, int terrainWidth, int terrainHeight);
	void Render(ID3D11DeviceContext*);
	bool GenerateHeightMap(ID3D11Device*);
	bool GenerateHeightMapWater(ID3D11Device*);
	bool Update();
	float* GetWavelength();

	float* GetAmplitude();

	float* GetSmoothingValue();

	int* GetparticleDepositIterations();

	float* GetparticleDisplacement();

	float* GetnoiseTime();

	int* GetfbmOctaves();

	float* GetfbmLacunarity();

	float* GetfbmGain();

	bool isUsingSmoothing;

	void GenerateSimplexNoiseNoise();

	void SmoothTerrain();

	bool generateSimplexNoiseTerrain;

	void GenerateFBM();

	void GenerateParticleDecomposition();

	void Reset();

	//check which terrain setting is true
	bool mode_RandomNoise;
	bool mode_PerlinNoise;
	bool mode_FBM;
	bool mode_ParticleDecomposition;
	
	enum Mode {RandomNoise, PerlinNoise, FBM, ParticleDecomposition};

	char* GetMode();
	Mode GenerationMode;

	void WaterWave();

	void setTime(float time);

	float* GetTime();

	int* GetGenModeNo();
	

private:
	bool CalculateNormals();
	void Shutdown();
	void ShutdownBuffers();
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	
	//Frequance of perlin noise ( used for both x and z)
	float perlinNoiseFrequancy;

	
	//Frequance of perlin noise ( used for both x and z)
	float perlinNoiseHeightRange;

	//to create smoothing form around a point
	float smoothingValue;
	
	int m_particleDepositIterations;
	float m_particleDisplacement;
	float m_noiseFrequency;
	float m_noiseTime;
	int m_fbmOctaves;
	float m_fbmLacunarity;
	float m_fbmGain;

	int GenModeNo;

	//check for water
	bool water;

	float m_time;
	

private:
	bool m_terrainGeneratedToggle;
	int m_terrainWidth, m_terrainHeight;
	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	float m_frequency, m_amplitude, m_wavelength;
	HeightMapType* m_heightMap;

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
};

