#pragma once

#include<memory>

class GameObject;
class Transform;

template<typename T>
using Ref = std::shared_ptr<T>;

class Component
{
public:
	Component(Ref<GameObject> gameObject, Ref<Transform> transform);
	virtual ~Component() = default;

protected:

	Ref<Transform> m_transform;
	Ref<GameObject> m_gameObject;
};

