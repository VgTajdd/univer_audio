#include <iostream>

#include <thread>

#include "engine/UAudioEngine.h"

constexpr float TO_RADIANS = 3.1416f / 180.f;

int main()
{
	std::cout << "Demo 0" << std::endl;
	{
		univer::audio::UAudioEngine audioEngine;
		audioEngine.init();
		audioEngine.loadSound( "assets/deepbark.wav", true );
		audioEngine.playSound( "assets/deepbark.wav", univer::audio::Vector3{ 10, 0, 0 }, audioEngine.volumeTodB( 1.0f ) );
		std::cin.get();
		audioEngine.playSound( "assets/deepbark.wav", univer::audio::Vector3{ 0, 0, 0 }, audioEngine.volumeTodB( 1.0f ) );
		std::cin.get();
		audioEngine.playSound( "assets/deepbark.wav", univer::audio::Vector3{ 0, 0, 10 }, audioEngine.volumeTodB( 1.0f ) );
		std::cin.get();
		audioEngine.unLoadSound( "assets/deepbark.wav" );
		std::cin.get();
		audioEngine.shutdown();
	}

	std::cin.get();
	std::cout << "Demo 1" << std::endl;
	{
		univer::audio::UAudioEngine audioEngine;
		audioEngine.init();
		audioEngine.loadSound( "assets/deepbark.wav", true, true );
		audioEngine.playSound( "assets/deepbark.wav", univer::audio::Vector3{ 10, 10, 10 }, audioEngine.volumeTodB( 1.0f ) );
		std::cin.get();
		audioEngine.unLoadSound( "assets/deepbark.wav" );
		std::cin.get();
		audioEngine.shutdown();
	}

	std::cin.get();
	std::cout << "Demo 2" << std::endl;

	{
		univer::audio::UAudioEngine audioEngine;
		audioEngine.init();

		audioEngine.set3dListenerAndOrientation( { 0,0,0 }, { 0,0,1 }, { 0,1,0 } );

		audioEngine.loadSound( "assets/deepbark.wav", true, true );
		int channel = audioEngine.playSound( "assets/deepbark.wav", univer::audio::Vector3{ 0, 0, 0 }, audioEngine.volumeTodB( 1.0f ) );
		float angle = 0;

		while ( angle < 720 )
		{

			std::chrono::milliseconds delta( 16 );
			std::this_thread::sleep_for( delta );

			audioEngine.setChannel3dPosition( channel, univer::audio::Vector3{ 5 * sin( TO_RADIANS * angle ), 5 * cos( TO_RADIANS * angle ), 5 * cos( TO_RADIANS * angle ) } );
			std::cout << angle++ << std::endl;

			audioEngine.update();
		}

		//audioEngine.stopChannel( channel );
		audioEngine.stopAllChannels();
		audioEngine.shutdown();
	}

	std::cin.get();
	return 0;
}