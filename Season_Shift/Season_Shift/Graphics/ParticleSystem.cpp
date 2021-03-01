#include "pch.h"
#include "ParticleSystem.h"

void ParticleSystem::emitt()
{
}

void ParticleSystem::simulate(float dt)
{
}

void ParticleSystem::draw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
}

ParticleSystem::ParticleSystem(std::shared_ptr<GfxRenderer> renderer, std::string simShader, std::string emittShader)
{
	m_renderer = renderer;
	auto dev = m_renderer->getDXDevice();
	m_simulationShader = dev->createShader(simShader, DXShader::Type::CS);
	m_emittShader = dev->createShader(emittShader, DXShader::Type::CS);
	m_vertexShader = dev->createShader("ParticleVS.cso", DXShader::Type::VS);
	m_geometryShader = dev->createShader("ParticleGS.cso", DXShader::Type::CS);
	m_pixelShader = dev->createShader("ParticlePS.cso", DXShader::Type::CS);




}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::SimulateAndDraw(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj, float dt)
{

}
