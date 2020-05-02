#include <iostream>

#include "engine/UAudioEngine.h"

int main()
{
	{
		univer::audio::UAudioEngine audioEngine;
		audioEngine.init();
		audioEngine.loadSound( "assets/deepbark.wav", true, true );
		audioEngine.playSound( "assets/deepbark.wav", univer::audio::Vector3{ 10, 0, 0 }, audioEngine.volumeTodB( 1.0f ) );
		std::cin.get();
		audioEngine.unLoadSound( "assets/deepbark.wav" );
		std::cin.get();
		audioEngine.shutdown();
	}

	std::cin.get();
	return 0;
}