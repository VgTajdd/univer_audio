// ========================================================================= //
// Copyright (c) 2021 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the Apache License 2.0.                       //
// UAEImplementation.cpp                                                     //
// ========================================================================= //

#include "UAEImplementation.h"
#include "UAUtils.h"

using univer::audio::UAEImplementation;

UAEImplementation::UAEImplementation() :
	system( nullptr ),
	nextChannelId( 0 ),
	nextSoundId( 0 )
{
	checkErrors( ::FMOD::System_Create( &system ) );
	checkErrors( system->init( 512, FMOD_INIT_NORMAL, nullptr ) );
}

UAEImplementation::~UAEImplementation()
{
	for ( const auto& [channelId, channel] : channels )
	{
		if ( channel->isPlaying() )
		{
			channel->stop();
		}
	}
	for ( const auto& [soundId, sound] : sounds )
	{
		if ( soundIsLoaded( soundId ) )
		{
			unloadSound( soundId );
		}
	}
	checkErrors( system->release() );
}

void UAEImplementation::update( const float dt )
{
	std::vector<std::map< int, std::unique_ptr< UChannel > >::iterator> pStoppedChannels;
	for ( auto it = channels.begin(), itEnd = channels.end(); it != itEnd; ++it )
	{
		it->second->update( dt );
		if ( it->second->m_state == UChannel::State::STOPPED )
		{
			pStoppedChannels.push_back( it );
		}
	}
	for ( auto& it : pStoppedChannels )
	{
		channels.erase( it );
	}
	checkErrors( system->update() );
}

bool UAEImplementation::soundIsLoaded( const int soundId )
{
	auto tFoundIt = sounds.find( soundId );
	if ( tFoundIt != sounds.end() )
	{
		if ( tFoundIt->second->m_fmodSound != nullptr )
		{
			return true;
		}
	}
	return false;
}

void UAEImplementation::loadSound( const int soundId, const void* data, const size_t dataSize )
{
	if ( soundIsLoaded( soundId ) )
	{
		return;
	}
	auto tFoundIt = sounds.find( soundId );
	if ( tFoundIt == sounds.end() )
	{
		return;
	}

	const auto& uSound = tFoundIt->second;

	FMOD_MODE eMode = /*FMOD_NONBLOCKING*/FMOD_DEFAULT;
	eMode |= uSound->is3d ? ( FMOD_3D/* | FMOD_3D_INVERSETAPEREDROLLOFF*/ ) : FMOD_2D;
	eMode |= uSound->isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= uSound->isStreaming ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	::FMOD::Sound* sound = nullptr;
	if ( uSound->useBinaryData )
	{
		int numChannels = 0;
		float frequency = 0;
		unsigned int lengthBytes = 0;
		FMOD_SOUND_FORMAT format = FMOD_SOUND_FORMAT::FMOD_SOUND_FORMAT_NONE;
		FMOD_SOUND_TYPE type = FMOD_SOUND_TYPE::FMOD_SOUND_TYPE_UNKNOWN;
		{
			FMOD_MODE dummyMode = eMode;
			dummyMode |= FMOD_OPENONLY; // Just open the file, dont prebuffer or read. Good for fast opens for info, or when sound::readData is to be used.
			::FMOD::Sound* dummy = nullptr;
			checkErrors( system->createStream( uSound->name.c_str(), dummyMode, nullptr, &dummy ) );
			checkErrors( dummy->getFormat( &type, &format, &numChannels, nullptr ) );
			checkErrors( dummy->getDefaults( &frequency, nullptr ) );
			checkErrors( dummy->getLength( &lengthBytes, FMOD_TIMEUNIT_RAWBYTES ) );
			dummy->release();
		}

		FMOD_CREATESOUNDEXINFO sndinfo = { 0 };
		sndinfo.format = format;
		sndinfo.numchannels = numChannels;
		sndinfo.defaultfrequency = (int) frequency;
		sndinfo.cbsize = sizeof( sndinfo );
		sndinfo.length = (unsigned int) dataSize;

		eMode |= FMOD_OPENMEMORY;
		checkErrors( system->createSound( (const char*) data, eMode, &sndinfo, &sound ) );
	}
	else
	{
		checkErrors( system->createSound( uSound->name.c_str(), eMode, nullptr, &sound ) );
	}

	if ( sound != nullptr )
	{
		checkErrors( sound->set3DMinMaxDistance( uSound->minDistance, uSound->maxDistance ) );
		uSound->m_fmodSound = sound;
	}
}

void UAEImplementation::unloadSound( const int soundId )
{
	auto tFoundIt = sounds.find( soundId );
	if ( tFoundIt == sounds.end() )
	{
		return;
	}
	const auto& uSound = tFoundIt->second;
	if ( uSound->m_fmodSound != nullptr )
	{
		checkErrors( uSound->m_fmodSound->release() );
	}
	uSound->m_fmodSound = nullptr;
}
