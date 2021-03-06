#include "GameServer.hpp"
#include "NetworkProtocol.hpp"
#include <SFML/System.hpp>

#include <SFML/Network/Packet.hpp>

#include "Tank.hpp"
#include "PickupType.hpp"
#include "Utility.hpp"
#include <iostream>

//It is essential to set the sockets to non-blocking - m_socket.setBlocking(false)
//otherwise the server will hang waiting to read input from a connection

GameServer::RemotePeer::RemotePeer() :m_ready(false), m_timed_out(false)
{
	m_socket.setBlocking(false);
}

GameServer::GameServer(sf::Vector2f battlefield_size)
	: m_thread(&GameServer::ExecutionThread, this)
	, m_listening_state(false)
	, m_client_timeout(sf::seconds(1.f))
	, m_max_connected_players(15)
	, m_connected_players(0)
	, m_world_height(5000.f)
	, m_battlefield_rect(0.f, m_world_height - battlefield_size.y, battlefield_size.x, battlefield_size.y)
	, m_battlefield_scrollspeed(0.f)
	, m_tank_count(0)
	, m_peers(1)
	, m_tank_identifier_counter(1)
	, m_waiting_thread_end(false)
	, m_last_spawn_time(sf::Time::Zero)
	, m_time_for_next_spawn(sf::seconds(5.f))
	, m_can_succeed(false)
	, m_green_tanks(0)
	, m_yellow_tanks(0)
{
	m_listener_socket.setBlocking(false);
	m_peers[0].reset(new RemotePeer());
	m_thread.launch();
}

GameServer::~GameServer()
{
	m_waiting_thread_end = true;
	m_thread.wait();
}

//This is the same as SpawnSelf but indicate that an Tank from a different client is entering the world

void GameServer::NotifyPlayerSpawn(sf::Int32 tank_identifier)
{
	sf::Packet packet;
	//First thing for every packet is what type of packet it is
	packet << static_cast<sf::Int32>(Server::PacketType::PlayerConnect);
	packet << tank_identifier << m_tank_info[tank_identifier].m_position.x << m_tank_info[tank_identifier].m_position.y 
		<< m_tank_info[tank_identifier].m_tank_rotation << m_tank_info[tank_identifier].m_cannon_rotation;
	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}


	// Increment each team , green or yellow tank
	if ((m_tank_identifier_counter % 2) - 1 == 0)
	{
		m_yellow_tanks++;
	}
	else
	{
		m_green_tanks++;
	}

	//std::cout << "Green Tanks: " << m_green_tanks << std::endl;
	//std::cout << "Yellow Tanks: " << m_yellow_tanks << std::endl;

}

//This is the same as PlayerEvent, but for real-time actions. This means that we are changing an ongoing state to either true or false, so we add a Boolean value to the parameters

void GameServer::NotifyPlayerRealtimeChange(sf::Int32 tank_identifier, sf::Int32 action, bool action_enabled)
{
	sf::Packet packet;
	//First thing for every packet is what type of packet it is
	packet << static_cast<sf::Int32>(Server::PacketType::PlayerRealtimeChange);
	packet << tank_identifier;
	packet << action;
	packet << action_enabled;

	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

//This takes two sf::Int32 variables, the Tank identifier and the action identifier
//as declared in the Player class. This is used to inform all peers that Tank X has
//triggered an action

void GameServer::NotifyPlayerEvent(sf::Int32 tank_identifier, sf::Int32 action)
{
	sf::Packet packet;
	//First thing for every packet is what type of packet it is
	packet << static_cast<sf::Int32>(Server::PacketType::PlayerEvent);
	packet << tank_identifier;
	packet << action;

	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

void GameServer::SetListening(bool enable)
{
	//Check if the server listening socket is already listening
	if (enable)
	{
		if (!m_listening_state)
		{
			m_listening_state = (m_listener_socket.listen(SERVER_PORT) == sf::TcpListener::Done);
		}
	}
	else
	{
		m_listener_socket.close();
		m_listening_state = false;
	}
}


void GameServer::ExecutionThread()
{
	SetListening(true);

	sf::Time frame_rate = sf::seconds(1.f / 60.f);
	sf::Time frame_time = sf::Time::Zero;
	sf::Time tick_rate = sf::seconds(1.f / 20.f);
	sf::Time tick_time = sf::Time::Zero;
	sf::Clock frame_clock, tick_clock;

	while (!m_waiting_thread_end)
	{
		HandleIncomingConnections();
		HandleIncomingPackets();

		frame_time += frame_clock.getElapsedTime();
		frame_clock.restart();

		tick_time += tick_clock.getElapsedTime();
		tick_clock.restart();

		//Fixed update step
		while (frame_time >= frame_rate)
		{
			m_battlefield_rect.top += m_battlefield_scrollspeed * frame_rate.asSeconds();
			frame_time -= frame_rate;
		}

		//Fixed tick step
		while (tick_time >= tick_rate)
		{
			Tick();
			tick_time -= tick_rate;
		}

		//sleep
		sf::sleep(sf::milliseconds(100));

	}
}

void GameServer::Tick()
{
	

	std::cout << "Peer Size = " << m_peers.size() << std::endl;
	for (int i = 0; i < m_peers.size(); i++)
	{
		std::cout << "Peer = " << m_peers[i].get() << std::endl;
	}

	if (m_tank_count == 2)
	{
		m_can_succeed = true;
	}

	UpdateClientState();


	if (m_tank_count == 1 && m_can_succeed)
	{
		sf::Packet mission_success_packet;
		
		// If last Tank is Green then show "Success Green" packetType, else show Yellow
		if (m_peers[0]->m_tank_identifiers[0] % 2 == 0)
		{
			mission_success_packet << static_cast<sf::Int32>(Server::PacketType::SuccessYellow);
			SendToAll(mission_success_packet);
		}
		else
		{
			mission_success_packet << static_cast<sf::Int32>(Server::PacketType::SuccessGreen);
			SendToAll(mission_success_packet);
		}
	}

	//Remove Tank that have been destroyed
	for (auto itr = m_tank_info.begin(); itr != m_tank_info.end();)
	{
		if (itr->second.m_hitpoints <= 0)
		{
			m_tank_info.erase(itr++);
		}
		else
		{
			++itr;
		}
	}

}

sf::Time GameServer::Now() const
{
	return m_clock.getElapsedTime();
}

void GameServer::HandleIncomingPackets()
{
	bool detected_timeout = false;

	for (PeerPtr& peer : m_peers)
	{
		if (peer->m_ready)
		{
			sf::Packet packet;
			while (peer->m_socket.receive(packet) == sf::Socket::Done)
			{
				//Interpret the packet and react to it
				HandleIncomingPacket(packet, *peer, detected_timeout);

				peer->m_last_packet_time = Now();
				packet.clear();
			}

			if (Now() > peer->m_last_packet_time + m_client_timeout)
			{
				peer->m_timed_out = true;
				detected_timeout = true;
			}

		}
	}

	if (detected_timeout)
	{
		HandleDisconnections();
	}

}

void GameServer::HandleIncomingPacket(sf::Packet& packet, RemotePeer& receiving_peer, bool& detected_timeout)
{
	sf::Int32 packet_type;
	packet >> packet_type;

	switch (static_cast<Client::PacketType> (packet_type))
	{
		case Client::PacketType::Quit:
		{
			receiving_peer.m_timed_out = true;
			detected_timeout = true;
		}
		break;

		case Client::PacketType::PlayerEvent:
		{
			sf::Int32 tank_identifier;
			sf::Int32 action;
			packet >> tank_identifier >> action;
			NotifyPlayerEvent(tank_identifier, action);
		}
		break;

		case Client::PacketType::PlayerRealtimeChange:
		{
			sf::Int32 tank_identifier;
			sf::Int32 action;
			bool action_enabled;
			packet >> tank_identifier >> action >> action_enabled;
			NotifyPlayerRealtimeChange(tank_identifier, action, action_enabled);
		}
		break;

		case Client::PacketType::RequestCoopPartner:
		{
			receiving_peer.m_tank_identifiers.emplace_back(m_tank_identifier_counter);
			m_tank_info[m_tank_identifier_counter].m_position = sf::Vector2f(m_battlefield_rect.width / 2, m_battlefield_rect.top + m_battlefield_rect.height / 2);
			m_tank_info[m_tank_identifier_counter].m_hitpoints = 100;
			m_tank_info[m_tank_identifier_counter].m_missile_ammo = 2;
			m_tank_info[m_tank_identifier_counter].m_tank_rotation = 0;
			m_tank_info[m_tank_identifier_counter].m_cannon_rotation = 0;

			sf::Packet request_packet;
			request_packet << static_cast<sf::Int32>(Server::PacketType::AcceptCoopPartner);
			request_packet << m_tank_identifier_counter;
			request_packet << m_tank_info[m_tank_identifier_counter].m_position.x;
			request_packet << m_tank_info[m_tank_identifier_counter].m_position.y;
			request_packet << m_tank_info[m_tank_identifier_counter].m_tank_rotation;
			request_packet << m_tank_info[m_tank_identifier_counter].m_cannon_rotation;

			receiving_peer.m_socket.send(request_packet);
			m_tank_count++;

			// Tell everyone else about the new Tank
			sf::Packet notify_packet;
			notify_packet << static_cast<sf::Int32>(Server::PacketType::PlayerConnect);
			notify_packet << m_tank_identifier_counter;
			notify_packet << m_tank_info[m_tank_identifier_counter].m_position.x;
			notify_packet << m_tank_info[m_tank_identifier_counter].m_position.y;
			notify_packet << m_tank_info[m_tank_identifier_counter].m_tank_rotation;
			notify_packet << m_tank_info[m_tank_identifier_counter].m_cannon_rotation;

			for (PeerPtr& peer : m_peers)
			{
				if (peer.get() != &receiving_peer && peer->m_ready)
				{

					peer->m_socket.send(notify_packet);
				}
			}

			m_tank_identifier_counter++;
		}
		break;

		case Client::PacketType::PositionUpdate:
		{
			sf::Int32 num_tank;
			packet >> num_tank;

			for (sf::Int32 i = 0; i < num_tank; ++i)
			{
				sf::Int32 tank_identifier;
				sf::Int32 tank_hitpoints;
				sf::Int32 missile_ammo;
				sf::Vector2f tank_position;
				float tank_rotation;
				float cannon_rotation;
				packet >> tank_identifier >> tank_position.x >> tank_position.y >> tank_hitpoints >> missile_ammo >> tank_rotation >> cannon_rotation;
				m_tank_info[tank_identifier].m_position = tank_position;
				m_tank_info[tank_identifier].m_hitpoints = tank_hitpoints;
				m_tank_info[tank_identifier].m_missile_ammo = missile_ammo;
				m_tank_info[tank_identifier].m_tank_rotation = tank_rotation;
				m_tank_info[tank_identifier].m_cannon_rotation = cannon_rotation;

			}
		}
		break;

		case Client::PacketType::GameEvent:
		{
			sf::Int32 action;
			float x;
			float y;

			packet >> action;
			packet >> x;
			packet >> y;

			//Drop pickup at start of game
			//To avoid multiple messages only listen to the first peer (host)
			if (action == GameActions::GameStartPickup && &receiving_peer == m_peers[0].get())
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Server::PacketType::SpawnPickup);
				packet << static_cast<sf::Int32>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
				packet << x;
				packet << y;

				SendToAll(packet);
			}
		}
	}
}

void GameServer::HandleIncomingConnections()
{
	if (!m_listening_state)
	{
		return;
	}

	if (m_listener_socket.accept(m_peers[m_connected_players]->m_socket) == sf::TcpListener::Done)
	{
		//Order the new client to spawn its player 1
		m_tank_info[m_tank_identifier_counter].m_position = sf::Vector2f(m_battlefield_rect.width / 2, m_battlefield_rect.top + m_battlefield_rect.height / 2);
		m_tank_info[m_tank_identifier_counter].m_hitpoints = 100;
		m_tank_info[m_tank_identifier_counter].m_missile_ammo = 2;
		m_tank_info[m_tank_identifier_counter].m_tank_rotation = 0;
		m_tank_info[m_tank_identifier_counter].m_cannon_rotation = 0;

		sf::Packet packet;
		packet << static_cast<sf::Int32>(Server::PacketType::SpawnSelf);
		packet << m_tank_identifier_counter;
		packet << m_tank_info[m_tank_identifier_counter].m_position.x;
		packet << m_tank_info[m_tank_identifier_counter].m_position.y;
		packet << m_tank_info[m_tank_identifier_counter].m_tank_rotation;
		packet << m_tank_info[m_tank_identifier_counter].m_cannon_rotation;

		m_peers[m_connected_players]->m_tank_identifiers.emplace_back(m_tank_identifier_counter);

		BroadcastMessage("New player");
		InformWorldState(m_peers[m_connected_players]->m_socket);
		NotifyPlayerSpawn(m_tank_identifier_counter++);

		m_peers[m_connected_players]->m_socket.send(packet);
		m_peers[m_connected_players]->m_ready = true;
		m_peers[m_connected_players]->m_last_packet_time = Now();

		m_tank_count++;
		m_connected_players++;

		if (m_connected_players >= m_max_connected_players)
		{
			SetListening(false);
		}
		else
		{
			m_peers.emplace_back(PeerPtr(new RemotePeer()));
		}
	}
}

void GameServer::HandleDisconnections()
{
	for (auto itr = m_peers.begin(); itr != m_peers.end();)
	{
		if ((*itr)->m_timed_out)
		{
			//Inform everyone of a disconnection, erase
			for (sf::Int32 identifer : (*itr)->m_tank_identifiers)
			{
				SendToAll((sf::Packet() << static_cast<sf::Int32>(Server::PacketType::PlayerDisconnect) << identifer));
				m_tank_info.erase(identifer);
			}

			m_connected_players--;
			m_tank_count -= (*itr)->m_tank_identifiers.size();

			itr = m_peers.erase(itr);

			//If the number of peers has dropped below max_connections
			if (m_connected_players < m_max_connected_players)
			{
				m_peers.emplace_back(PeerPtr(new RemotePeer()));
				SetListening(true);
			}

			BroadcastMessage("A player has disconnected");

		}
		else
		{
			++itr;
		}
	}


}

void GameServer::InformWorldState(sf::TcpSocket& socket)
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(Server::PacketType::InitialState);
	packet << m_world_height << m_battlefield_rect.top + m_battlefield_rect.height;
	packet << static_cast<sf::Int32>(m_tank_count);

	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			for (sf::Int32 identifier : m_peers[i]->m_tank_identifiers)
			{
				packet << identifier << m_tank_info[identifier].m_position.x << m_tank_info[identifier].m_position.y 
					<< m_tank_info[identifier].m_hitpoints << m_tank_info[identifier].m_missile_ammo 
					<< m_tank_info[identifier].m_tank_rotation << m_tank_info[identifier].m_cannon_rotation;
			}
		}
	}

	socket.send(packet);
}

void GameServer::BroadcastMessage(const std::string& message)
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(Server::PacketType::BroadcastMessage);
	packet << message;
	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

void GameServer::SendToAll(sf::Packet& packet)
{
	for (PeerPtr& peer : m_peers)
	{
		if (peer->m_ready)
		{
			peer->m_socket.send(packet);
		}
	}
}

void GameServer::UpdateClientState()
{
	sf::Packet update_client_state_packet;
	update_client_state_packet << static_cast<sf::Int32>(Server::PacketType::UpdateClientState);
	update_client_state_packet << static_cast<float>(m_battlefield_rect.top + m_battlefield_rect.height);
	update_client_state_packet << static_cast<sf::Int32>(m_tank_count);

	for (const auto& tank : m_tank_info)
	{
		update_client_state_packet << tank.first << tank.second.m_position.x << tank.second.m_position.y << tank.second.m_hitpoints 
			<< tank.second.m_missile_ammo << tank.second.m_tank_rotation << tank.second.m_cannon_rotation;
	}

	SendToAll(update_client_state_packet);
}