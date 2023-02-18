// ========================================================================= //
// Copyright (c) 2023 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the Apache License 2.0.                       //
// Sandbox.cpp                                                               //
// ========================================================================= //

#include <iostream>
#include <thread>
#include <cmath>

#include "utils/FileLoaderHelper.h"

#include <univer_audio/UAudioEngine.h>

constexpr float TO_RADIANS = 3.1416f / 180.f;

class Application
{
public:
	Application();
	~Application();
	void init();
	void update( const float dt );
	const bool isRunning() const { return m_isRunning; }
private:
	int wauwauId = -1;
	int channelId = -1;
	float angle = 0;
private:
	bool m_isRunning;
	univer::audio::UAudioEngine* m_audioEngine;
};

Application::Application() :
	m_audioEngine( nullptr ),
	m_isRunning( false )
{}

Application::~Application()
{
	if ( m_audioEngine != nullptr )
	{
		//audioEngine->stopChannel( channelId );
		//audioEngine->stopAllChannels();
		m_audioEngine->shutdown();
		delete m_audioEngine;
	}
}

void Application::init()
{
	m_audioEngine = new univer::audio::UAudioEngine();
	m_audioEngine->init();

	float position[3] = { 0, 0, 0 };
	float look[3] = { 0, 0, 1 };
	float up[3] = { 0, 1, 0 };

	m_audioEngine->set3dListenerAndOrientation( position, look, up );

	wauwauId = m_audioEngine->registerSound( "assets/deepbark.wav",
										   10.f,
										   1.f,
										   360.f,
										   true,
										   true,
										   false,
										   true );

	channelId = m_audioEngine->playSound( wauwauId,
										  position,
										  m_audioEngine->volumeTodB( 1.0f ) );

	m_isRunning = true;
}

void Application::update( const float dt )
{
	if ( m_audioEngine != nullptr )
	{
		m_audioEngine->update( dt );

		if ( angle < 360 )
		{
			float position[3] = { 5 * std::sin( TO_RADIANS * angle ), 0, 5 * std::cos( TO_RADIANS * angle ) };
			m_audioEngine->setChannel3dPosition( channelId, position );
			std::cout << angle++ << std::endl;
			if ( angle == 360 )
			{
				m_audioEngine->stopChannel( channelId, 1000 );
			}
		}

		if ( !m_audioEngine->isPlaying( channelId ) )
		{
			m_isRunning = false;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
	std::cout << "Demo 0" << std::endl;
	{
		univer::audio::UAudioEngine audioEngine;
		audioEngine.init();

		float position[3] = { 0, 0, 0 };
		float look[3] = { 0, 0, 1 };
		float up[3] = { 0, 1, 0 };

		audioEngine.set3dListenerAndOrientation( position, look, up );

		int wauwauId = audioEngine.registerSound( "assets/deepbark.wav",
												  1.f,
												  1.f,
												  100.f,
												  true,
												  true,
												  false,
												  true );

		float position1[3] = { 0, 0, 0 };
		int channelId1 = audioEngine.playSound( wauwauId, position1, audioEngine.volumeTodB( 1.0f ) );
		std::cin.get();
		position1[0] = -1;
		int channelId2 = audioEngine.playSound( wauwauId, position1, audioEngine.volumeTodB( 1.0f ) );
		std::cin.get();
		audioEngine.stopAllChannels();
		std::cin.get();
		audioEngine.unLoadSound( wauwauId );
		std::cin.get();
		audioEngine.shutdown();
	}

	std::cin.get();
	std::cout << "Demo 1" << std::endl;
	{
		univer::audio::UAudioEngine audioEngine;
		audioEngine.init();

		int wauwauId = audioEngine.registerSound( "assets/deepbark.wav",
												  1.f,
												  1.f,
												  100.f,
												  false,
												  false,
												  false,
												  true );

		float position[3] = { 10, 10, 10 };
		audioEngine.playSound( wauwauId, position, audioEngine.volumeTodB( 1.0f ) );
		std::cin.get();
		audioEngine.unLoadSound( wauwauId );
		std::cin.get();
		audioEngine.shutdown();
	}

	std::cin.get();
	std::cout << "Demo 2" << std::endl;

	{
		Application app;
		app.init();

		std::chrono::steady_clock::time_point lastTime;
		lastTime = std::chrono::steady_clock::now();

		while ( true )
		{

			std::chrono::milliseconds delta( 16 );
			std::this_thread::sleep_for( delta );

			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
			float dt = float( std::chrono::duration_cast<std::chrono::milliseconds>( now - lastTime ).count() );

			lastTime = now;

			app.update( dt );
			if ( !app.isRunning() )
			{
				break;
			}
		}
	}

	std::cin.get();
	std::cout << "Demo 3" << std::endl;
	{
		auto binary_data = univer::read_file( "assets/deepbark.wav" );

		univer::audio::UAudioEngine audioEngine;
		audioEngine.init();

		float position[3] = { 0, 0, 0 };
		float look[3] = { 0, 0, 1 };
		float up[3] = { 0, 1, 0 };

		audioEngine.set3dListenerAndOrientation( position, look, up );

		int wauwauId = audioEngine.registerSound( "assets/deepbark.wav",
												  1.f,
												  1.f,
												  100.f,
												  true,
												  true,
												  false,
												  true,
												  true );

		audioEngine.loadSound( wauwauId, true, true, false,
							   (const char*) binary_data->data,
							   binary_data->size );

		float position1[3] = { 0, 0, 0 };
		int channelId1 = audioEngine.playSound( wauwauId, position1, audioEngine.volumeTodB( 1.0f ) );
		std::cin.get();
	}

	std::cin.get();
	return 0;
}
