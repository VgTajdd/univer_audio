#include <iostream>

#include <thread>

#include "engine/UAudioEngine.h"
#include "engine/UVector3.h"

constexpr float TO_RADIANS = 3.1416f / 180.f;

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
		univer::audio::UAudioEngine audioEngine;
		audioEngine.init();

		audioEngine.set3dListenerAndOrientation( { 0,0,0 }, { 0,0,1 }, { 0,1,0 } );

		int wauwauId = audioEngine.registerSound( "assets/deepbark.wav",
												  10.f,
												  1.f,
												  360.f,
												  true,
												  true,
												  false,
												  true );

		int channel = audioEngine.playSound( wauwauId, univer::audio::UVector3{ 0, 0, 0 }, audioEngine.volumeTodB( 1.0f ) );
		float angle = 0;

		std::chrono::steady_clock::time_point lastTime;
		lastTime = std::chrono::steady_clock::now();

		while ( angle <= 360 )
		{

			std::chrono::milliseconds delta( 16 );
			std::this_thread::sleep_for( delta );

			audioEngine.setChannel3dPosition( channel, univer::audio::UVector3{ 5 * sin( TO_RADIANS * angle ), 0, 5 * cos( TO_RADIANS * angle ) } );
			std::cout << angle++ << std::endl;

			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
			int dt = int( std::chrono::duration_cast<std::chrono::milliseconds>( now - lastTime ).count() );

			audioEngine.update( (float) dt );
			lastTime = now;
		}

		//audioEngine.stopChannel( channel );
		audioEngine.stopAllChannels();
		std::cin.get();
		audioEngine.shutdown();
	}

	std::cin.get();
	return 0;
}