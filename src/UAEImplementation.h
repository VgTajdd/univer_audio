// ========================================================================= //
// Copyright (c) 2021 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the Apache License 2.0.                       //
// UAEImplementation.h                                                       //
// ========================================================================= //

#pragma once

#include "UAudioFader.h"
#include "UChannel.h"
#include "USound.h"

#include <fmod/fmod.hpp>

#include <map>
#include <vector>
#include <memory>
#include <iostream>

namespace univer::audio
{
struct UChannel;

class UAEImplementation
{
public:
	UAEImplementation();
	~UAEImplementation();

	void update( const float fTimeDeltaSeconds );

	bool soundIsLoaded( const int soundId );
	void loadSound( const int soundId, const void* data = nullptr, const size_t dataSize = 0 );
	void unloadSound( const int soundId );

	float dBToVolume( const float dB )
	{
		return powf( 10.0f, 0.05f * dB );
	}

	float volumeTodB( const float volume )
	{
		return 20.0f * log10f( volume );
	}

public:
	::FMOD::System* system;

	std::map< int, std::unique_ptr< USound > > sounds;
	std::map< int, std::unique_ptr< UChannel > > channels;

	int nextChannelId;
	int nextSoundId;
};
}
