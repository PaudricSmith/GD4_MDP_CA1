#include "SceneNode.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "Utility.hpp"

SceneNode::SceneNode(Category::Type category):m_children(), m_parent(nullptr), m_default_category(category)
{
}

void SceneNode::AttachChild(Ptr child)
{
	child->m_parent = this;
	//Todo - Why is emplace_back more efficient than push_back
	m_children.emplace_back(std::move(child));
}

SceneNode::Ptr SceneNode::DetachChild(const SceneNode& node)
{
	auto found = std::find_if(m_children.begin(), m_children.end(), [&](Ptr& p) {return p.get() == &node; });
	assert(found != m_children.end());

	Ptr result = std::move(*found);
	result->m_parent = nullptr;
	m_children.erase(found);
	return result;
}

void SceneNode::Update(sf::Time dt, CommandQueue& commands)
{
	UpdateCurrent(dt, commands);
	UpdateChildren(dt, commands);
}

sf::Vector2f SceneNode::GetWorldPosition() const
{
	return GetWorldTransform() * sf::Vector2f();
}

sf::Transform SceneNode::GetWorldTransform() const
{
	sf::Transform transform = sf::Transform::Identity;
	for(const SceneNode* node = this; node != nullptr; node = node->m_parent)
	{
		transform = node->getTransform() * transform;
	}
	return transform;
}

void SceneNode::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	//Do nothing by default
}

void SceneNode::UpdateChildren(sf::Time dt, CommandQueue& commands)
{
	for(Ptr& child : m_children)
	{
		child->Update(dt, commands);
	}
}

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//Apply transform of the current node
	states.transform *= getTransform();

	//Draw the node and children with changed transform
	DrawCurrent(target, states);
	DrawChildren(target, states);
}

void SceneNode::DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const
{
	//Do nothing by default
}

void SceneNode::DrawChildren(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (const Ptr& child : m_children)
	{
		child->draw(target, states);
	}
}

void SceneNode::OnCommand(const Command& command, sf::Time dt)
{
	//Is this command for me?
	if(command.category & GetCategory())
	{
		command.action(*this, dt);
	}

	//Pass command on to children
	for(Ptr& child : m_children)
	{
		child->OnCommand(command, dt);
	}
}

unsigned int SceneNode::GetCategory() const
{
	return static_cast<int>(m_default_category);
}

float distance(const SceneNode& lhs, const SceneNode& rhs)
{
	return Utility::Length(lhs.GetWorldPosition() - rhs.GetWorldPosition());
}