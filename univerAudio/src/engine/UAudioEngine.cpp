#include "UAudioEngine.h"

#include "fmod.hpp"

#include <iostream>
#include <map>
#include <vector>

using univer::audio::Vector3;
using univer::audio::UAudioEngine;

namespace univer::audio
{
FMOD_VECTOR VectorToFmod( const Vector3& vPosition )
{
	FMOD_VECTOR fVec;
	fVec.x = vPosition.x;
	fVec.y = vPosition.y;
	fVec.z = vPosition.z;
	return fVec;
}

int checkErrors( FMOD_RESULT result )
{
	if ( result != FMOD_OK )
	{
		std::cout << "FMOD ERROR: " << result << std::endl;
		return 1;
	}
	// std::cout << "FMOD all good" << std::endl;
	return 0;
}

struct UAEImplementation
{
	UAEImplementation();
	~UAEImplementation();

	void update();

	::FMOD::System* system;
	int nextChannelId;

	typedef std::map<std::string, ::FMOD::Sound*> SoundMap;
	typedef std::map<int, ::FMOD::Channel*> ChannelMap;

	SoundMap sounds;
	ChannelMap channels;
};

UAEImplementation::UAEImplementation() :
	system( nullptr ),
	nextChannelId( 0 )
{
	checkErrors( ::FMOD::System_Create( &system ) );
	checkErrors( system->init( 512, FMOD_INIT_NORMAL, nullptr ) );
}

UAEImplementation::~UAEImplementation()
{
	for ( const auto& [name, sound] : sounds )
	{
		sound->release();
	}
	for ( const auto& [name, channel] : sounds )
	{
		channel->release();
	}
	system->release();
}

void UAEImplementation::update()
{
	std::vector<ChannelMap::iterator> pStoppedChannels;
	for ( auto it = channels.begin(), itEnd = channels.end(); it != itEnd; ++it )
	{
		bool bIsPlaying = false;
		it->second->isPlaying( &bIsPlaying );
		if ( !bIsPlaying )
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

UAEImplementation* implementationPtr = nullptr;
}

/////////////////// UAudioEngine ///////////////////

void UAudioEngine::init()
{
	implementationPtr = new UAEImplementation();
}

void UAudioEngine::update()
{
	implementationPtr->update();
}

void UAudioEngine::shutdown()
{
	delete implementationPtr;
}

void UAudioEngine::loadSound( const std::string& strSoundName, bool b3d, bool bLooping, bool bStream )
{
	auto tFoundIt = implementationPtr->sounds.find( strSoundName );
	if ( tFoundIt != implementationPtr->sounds.end() )
	{
		return;
	}
	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= b3d ? FMOD_3D : FMOD_2D;
	eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
	FMOD::Sound* pSound = nullptr;
	checkErrors( implementationPtr->system->createSound( strSoundName.c_str(), eMode, nullptr, &pSound ) );
	if ( pSound )
	{
		implementationPtr->sounds[strSoundName] = pSound;
	}
}

void UAudioEngine::unLoadSound( const std::string& strSoundName )
{
	auto tFoundIt = implementationPtr->sounds.find( strSoundName );
	if ( tFoundIt == implementationPtr->sounds.end() )
	{
		return;
	}
	checkErrors( tFoundIt->second->release() );
	implementationPtr->sounds.erase( tFoundIt );
}

int UAudioEngine::playSound( const std::string& strSoundName, const Vector3& vPosition, float fVolumedB )
{
	int nChannelId = implementationPtr->nextChannelId++;
	auto tFoundIt = implementationPtr->sounds.find( strSoundName );
	if ( tFoundIt == implementationPtr->sounds.end() )
	{
		loadSound( strSoundName );
		tFoundIt = implementationPtr->sounds.find( strSoundName );
		if ( tFoundIt == implementationPtr->sounds.end() )
		{
			return nChannelId;
		}
	}
	FMOD::Channel* pChannel = nullptr;
	checkErrors( implementationPtr->system->playSound( tFoundIt->second, nullptr, false, &pChannel ) );
	if ( pChannel )
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode( &currMode );
		if ( currMode & FMOD_3D )
		{
			FMOD_VECTOR position = VectorToFmod( vPosition );
			FMOD_VECTOR velocity = { 0, 0, 0 };
			checkErrors( pChannel->set3DAttributes( &position, &velocity ) );
		}
		checkErrors( pChannel->setVolume( dBToVolume( fVolumedB ) ) );
		checkErrors( pChannel->setPaused( false ) );
		implementationPtr->channels[nChannelId] = pChannel;
	}
	return nChannelId;
}

void UAudioEngine::setChannel3dPosition( int nChannelId, const Vector3& vPosition )
{
	auto tFoundIt = implementationPtr->channels.find( nChannelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return;
	}

	FMOD_VECTOR position = VectorToFmod( vPosition );
	FMOD_VECTOR velocity = { 0, 0, 0 };
	checkErrors( tFoundIt->second->set3DAttributes( &position, &velocity ) );
}

void UAudioEngine::setChannelVolume( int nChannelId, float fVolumedB )
{
	auto tFoundIt = implementationPtr->channels.find( nChannelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return;
	}

	checkErrors( tFoundIt->second->setVolume( dBToVolume( fVolumedB ) ) );
}

void UAudioEngine::set3dListenerAndOrientation( const Vector3& vPosition, const Vector3& vLook, const Vector3& vUp )
{
	FMOD_VECTOR position = VectorToFmod( vPosition );
	FMOD_VECTOR speed = VectorToFmod( { 0, 0, 0 } );
	FMOD_VECTOR look = VectorToFmod( vLook );
	FMOD_VECTOR up = VectorToFmod( vUp );
	checkErrors( implementationPtr->system->set3DListenerAttributes( 0, &position, &speed, &look, &up ) );
}

void UAudioEngine::stopChannel( int nChannelId )
{
	auto tFoundIt = implementationPtr->channels.find( nChannelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return;
	}

	checkErrors( tFoundIt->second->stop() );
}

void UAudioEngine::stopAllChannels()
{
	for ( const auto& [nChannel, channel] : implementationPtr->channels )
	{
		checkErrors( channel->stop() );
	}
}

bool UAudioEngine::isPlaying( int nChannelId ) const
{
	auto tFoundIt = implementationPtr->channels.find( nChannelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return false;
	}

	bool isPlaying = false;
	checkErrors( tFoundIt->second->isPlaying( &isPlaying ) );
	return isPlaying;
}

float UAudioEngine::dBToVolume( float dB )
{
	return powf( 10.0f, 0.05f * dB );
}

float UAudioEngine::volumeTodB( float volume )
{
	return 20.0f * log10f( volume );
}
