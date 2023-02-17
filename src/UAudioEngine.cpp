// ========================================================================= //
// Copyright (c) 2021 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the Apache License 2.0.                       //
// UAudioEngine.cpp                                                          //
// ========================================================================= //

#include <univer_audio/UAudioEngine.h>
#include "UAudioFader.h"
#include "UAEImplementation.h"
#include "UChannel.h"
#include "UAUtils.h"

using univer::audio::UAudioEngine;
using univer::audio::USound;
using univer::audio::UAEImplementation;

static UAEImplementation* implementationPtr = nullptr;

void UAudioEngine::init()
{
	implementationPtr = new UAEImplementation();
}

void UAudioEngine::update( const float dt )
{
	implementationPtr->update( dt );
}

void UAudioEngine::shutdown()
{
	delete implementationPtr;
}

int UAudioEngine::registerSound( const std::string name,
								 const float defaultVolumeDB,
								 const float minDistance,
								 const float maxDistance,
								 const bool is3d,
								 const bool isLooping,
								 const bool isStreaming,
								 const bool load,
								 const bool useBinary )
{
	int soundId = implementationPtr->nextSoundId++;
	implementationPtr->sounds[soundId] = std::make_unique< USound >( name,
																	 defaultVolumeDB,
																	 minDistance,
																	 maxDistance,
																	 is3d,
																	 isLooping,
																	 isStreaming,
																	 useBinary );

	if ( load && !useBinary )
	{
		loadSound( soundId );
	}

	return soundId;
}

void UAudioEngine::unregisterSound( const int soundId )
{
	if ( implementationPtr->soundIsLoaded( soundId ) )
	{
		unLoadSound( soundId );
	}
	implementationPtr->sounds.erase( soundId );
}

void UAudioEngine::loadSound( const int soundId, const bool b3d, const bool bLooping, const bool bStream, const void* data, const size_t dataSize )
{
	implementationPtr->loadSound( soundId, data, dataSize );
}

void UAudioEngine::unLoadSound( const int soundId )
{
	implementationPtr->unloadSound( soundId );
}

int UAudioEngine::playSound( const int soundId, const float vPosition[3], const float fVolumedB )
{
	int channelId = implementationPtr->nextChannelId++;
	// auto tSoundIt = implementationPtr->sounds.find( soundId );
	if ( !implementationPtr->soundIsLoaded( soundId ) )
	{
		loadSound( soundId );
		if ( !implementationPtr->soundIsLoaded( soundId ) )
		{
			return channelId;
		}
	}
	implementationPtr->channels[channelId] = std::make_unique< UChannel >( *implementationPtr,
																			soundId,
																			vPosition,
																			fVolumedB );
	implementationPtr->channels[channelId]->update( 0.f );
	return channelId;
}

void UAudioEngine::setChannel3dPosition( const int channelId, const float vPosition[3] )
{
	auto tFoundIt = implementationPtr->channels.find( channelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return;
	}

	FMOD_VECTOR position = { vPosition[0], vPosition[1], vPosition[2] };
	FMOD_VECTOR velocity = { 0, 0, 0 };
	tFoundIt->second->set3DAttributes( &position, &velocity );
}

void UAudioEngine::setChannelVolume( const int channelId, const float fVolumedB )
{
	auto tFoundIt = implementationPtr->channels.find( channelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return;
	}

	tFoundIt->second->setVolume( dBToVolume( fVolumedB ) );
}

void UAudioEngine::set3dListenerAndOrientation( const float vPosition[3], const float vLook[3], const float vUp[3] )
{
	FMOD_VECTOR position = { vPosition[0], vPosition[1], vPosition[2] };
	FMOD_VECTOR speed = { 0, 0, 0 };
	FMOD_VECTOR look = { vLook[0], vLook[1], vLook[2] };
	FMOD_VECTOR up = { vUp[0], vUp[1], vUp[2] };
	checkErrors( implementationPtr->system->set3DListenerAttributes( 0, &position, &speed, &look, &up ) );
}

void UAudioEngine::stopChannel( const int channelId, const float fadeTimeSeconds )
{
	auto tFoundIt = implementationPtr->channels.find( channelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return;
	}
	tFoundIt->second->stop( fadeTimeSeconds );
}

void UAudioEngine::stopAllChannels()
{
	for ( const auto& [channelId, channel] : implementationPtr->channels )
	{
		if ( isPlaying( channelId ) )
		{
			channel->stop();
		}
	}
}

bool UAudioEngine::isPlaying( const int channelId ) const
{
	auto tFoundIt = implementationPtr->channels.find( channelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return false;
	}

	return tFoundIt->second->isPlaying();
}

float UAudioEngine::dBToVolume( const float dB )
{
	return implementationPtr->dBToVolume( dB );
}

float UAudioEngine::volumeTodB( const float volume )
{
	return implementationPtr->volumeTodB( volume );
}
