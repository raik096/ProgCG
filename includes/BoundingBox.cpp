#include "BoundingBox.h"

BoundingBox::BoundingBox(glm::vec3 size, glm::vec3 center) : m_Size(size), m_Center(center)
{
}

BoundingBox::~BoundingBox()
{
}

glm::vec3 BoundingBox::Center()
{
	return m_Center;
}

float BoundingBox::Left()
{
	return m_Center.x - m_Size.x/2;
}

float BoundingBox::Rigth()
{
	return m_Center.x + m_Size.x/2;
}

float BoundingBox::Top()
{
	return m_Center.y + m_Size.y/2;
}

float BoundingBox::Bottom()
{
	return m_Center.y - m_Size.y / 2;
}

float BoundingBox::Front()
{
	return m_Center.z + m_Size.z / 2;
}

float BoundingBox::Back()
{
	return m_Center.z - m_Size.z / 2;
}

glm::vec3& BoundingBox::Size()
{
	return m_Size;
}
