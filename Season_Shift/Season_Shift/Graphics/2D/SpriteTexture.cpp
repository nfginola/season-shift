#include "pch.h"
#include "SpriteTexture.h"

SpriteTexture::SpriteTexture(std::shared_ptr<DXTexture> texture, float rotation, float depth, std::function<void()> callback)
{
	m_callback = callback;
	m_show = true;
	m_texture = texture;
	m_rect = { 0 };
	m_rotation = rotation;
	m_depth = depth;
}

void SpriteTexture::draw(const std::shared_ptr<DirectX::SpriteBatch>& spriteBatch)
{
	if (m_show)
	{
		m_rect.right = m_texture->getDesc().desc2D.Width;
		m_rect.bottom = m_texture->getDesc().desc2D.Height;
		m_correctedScale = getCorrectScaleVector(m_scale);//{ m_scale.x *  m_windowSize.x / m_baseWindowSize.x, m_scale.y * m_windowSize.y / m_baseWindowSize.y };
		m_correctedPosition = getCorrectScaleVector(m_position);//{ m_position.x * m_windowSize.x / m_baseWindowSize.x, m_position.y * m_windowSize.y / m_baseWindowSize.y };
		spriteBatch->Draw(m_texture->getSRV().Get(), m_correctedPosition, &m_rect, DirectX::Colors::White, m_rotation, {0,0}, m_correctedScale, DirectX::SpriteEffects_None, m_depth);
	}
}

void SpriteTexture::setColor(const DirectX::SimpleMath::Color& col)
{
	
}

void SpriteTexture::checkForClick(int mouseX, int mouseY, bool isClicked) const
{
	if (isClicked && m_callback &&
		mouseX > m_correctedPosition.x && mouseX < (m_correctedPosition.x + getWidth()) &&
		mouseY > m_correctedPosition.y && mouseY < (m_correctedPosition.y + getHeight()))
	{
		m_callback();
	}
}

float SpriteTexture::getWidth() const
{
	return m_correctedScale.x * m_texture->getDesc().desc2D.Width;
}

float SpriteTexture::getHeight() const
{
	return m_correctedScale.y * m_texture->getDesc().desc2D.Height;
}

const DirectX::SimpleMath::Vector2& SpriteTexture::getPosition()
{
	return m_correctedPosition;
}

const DirectX::SimpleMath::Vector2& SpriteTexture::getScale()
{
	return m_correctedScale;
}