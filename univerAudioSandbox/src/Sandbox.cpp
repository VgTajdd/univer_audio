#include <iostream>

#include <thread>

#include "engine/UAudioEngine.h"
#include "engine/UVector3.h"

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
{ }

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

	m_audioEngine->set3dListenerAndOrientation( { 0,0,0 }, { 0,0,1 }, { 0,1,0 } );

	wauwauId = m_audioEngine->registerSound( "assets/deepbark.wav",
										   10.f,
										   1.f,
										   360.f,
										   true,
										   true,
										   false,
										   true );

	channelId = m_audioEngine->playSound( wauwauId,
										  univer::audio::UVector3{ 0, 0, 0 },
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
			m_audioEngine->setChannel3dPosition( channelId, univer::audio::UVector3{ 5 * sin( TO_RADIANS * angle ), 0, 5 * cos( TO_RADIANS * angle ) } );
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

		audioEngine.set3dListenerAndOrientation( { 0,0,0 }, { 0,0,1 }, { 0,1,0 } );

		int wauwauId = audioEngine.registerSound( "assets/deepbark.wav",
												  1.f,
												  1.f,
												  100.f,
												  true,
												  true,
												  false,
												  true );

		int channelId1 = audioEngine.playSound( wauwauId, univer::audio::UVector3{ 10, 0, 0 }, audioEngine.volumeTodB( 1.0f ) );
		std::cin.get();
		int channelId2 = audioEngine.playSound( wauwauId, univer::audio::UVector3{ -1, 0, 0 }, audioEngine.volumeTodB( 1.0f ) );
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

		audioEngine.playSound( wauwauId, univer::audio::UVector3{ 10, 10, 10 }, audioEngine.volumeTodB( 1.0f ) );
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
			int dt = int( std::chrono::duration_cast<std::chrono::milliseconds>( now - lastTime ).count() );

			lastTime = now;

			app.update( dt );
			if ( !app.isRunning() )
			{
				break;
			}
		}
	}

	std::cin.get();
	return 0;
}