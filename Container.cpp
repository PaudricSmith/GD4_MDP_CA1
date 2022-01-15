#include "Container.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <iostream>

namespace GUI
{
	Container::Container()
	: m_selected_child(-1)
	{
	}
	//TODO pass by reference as resharper is suggesting?
	void Container::Pack(Component::Ptr component)
	{
		m_children.emplace_back(component);
		if(!HasSelection() && component->IsSelectable())
		{
			Select(m_children.size() - 1);
		}
	}

	bool Container::IsSelectable() const
	{
		return false;
	}

	void Container::HandleEvent(const sf::Event& event)
	{
		if(HasSelection() && m_children[m_selected_child]->IsActive())
		{
			m_children[m_selected_child]->HandleEvent(event);
		}
		else if(event.type == sf::Event::KeyReleased)
		{
			if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up)
			{
				SelectPrevious();
			}
			else if(event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)
			{
				SelectNext();
			}
			else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right)
			{
				SelectRight();
			}
			else if(event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left)
			{
				SelectLeft();
			}
			else if(event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space)
			{
				if(HasSelection())
				{
					m_children[m_selected_child]->Activate();

				}
			}
		}
	}

	void Container::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		for(const Component::Ptr& child : m_children)
		{
			target.draw(*child, states);
		}

	}

	bool Container::HasSelection() const
	{
		return m_selected_child >= 0;
	}

	void Container::Select(std::size_t index)
	{
		if(index < m_children.size() && m_children[index]->IsSelectable())
		{
			if(HasSelection())
			{
				m_children[m_selected_child]->Deselect();
			}
			m_children[index]->Select();
			m_selected_child = index;
		}
	}

	void Container::SelectNext()
	{
		if(!HasSelection())
		{
			return;
		}
		//Search for the next component that is selectable and wrap around if necessary
		int next = m_selected_child;
		do
		{
			if (m_children.size() > 4) // If we are in a menu not the main menu
			{
				if (next == (m_children.size() / 2) - 1)
				{
					next = m_children.size() - 1;
				}
				else
				{
					next = (next + 1) % m_children.size();
				}
			}
			else
			{
				next = (next + 1) % m_children.size();
			}
			
		} while (!m_children[next]->IsSelectable());

		Select(next);
	}

	void Container::SelectPrevious()
	{
		if (!HasSelection())
		{
			return;
		}
		//Search for the next component that is selectable and wrap around if necessary
		int prev = m_selected_child;
		do
		{
			if (m_children.size() > 4) // If we are in a menu not the main menu
			{
				if (prev == (m_children.size() / 2))
				{
					prev = m_children.size() - 1;
				}
				else
				{
					prev = (prev + m_children.size() - 1) % m_children.size();
				}
			}
			else
			{
				prev = (prev + m_children.size() - 1) % m_children.size();
			}
			
		} while (!m_children[prev]->IsSelectable());

		Select(prev);
	}

	void Container::SelectRight()
	{
		if (!HasSelection())
		{
			return;
		}
		//Search for the next component that is selectable and wrap around if necessary
		int right = m_selected_child;
		do
		{
			right = (right + m_children.size() / 2) % (m_children.size() - 1);

		} while (!m_children[right]->IsSelectable());

		Select(right);
	}

	void Container::SelectLeft()
	{
		if (!HasSelection())
		{
			return;
		}
		//Search for the next component that is selectable and wrap around if necessary
		int left = m_selected_child;
		do
		{
			left = (left - m_children.size() / 2) % (m_children.size() - 1);

		} while (!m_children[left]->IsSelectable());

		Select(left);
	}
}
