#include "../UAudioEngine.h"
#include "USound.h"
#include "UAudioFader.h"

#include "fmod.hpp"

#include <iostream>
#include <map>
#include <vector>

using univer::audio::UAudioEngine;
using univer::audio::USound;

namespace univer::audio
{

int checkErrors( FMOD_RESULT result )
{
	if ( result != FMOD_OK )
	{
		std::cout << "FMOD ERROR: " << result << std::endl;
		return 1;
	}
	/*std::cout << "FMOD all good" << std::endl;*/
	return 0;
}

/////////////////// UAEImplementation ///////////////////

struct UChannel;
class UAEImplementation
{
public:
	UAEImplementation();
	~UAEImplementation();

	void update( const float fTimeDeltaSeconds );

	bool soundIsLoaded( const int soundId );
	void loadSound( const int soundId, const void* data = nullptr );
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

/////////////////// UChannel ///////////////////

struct UChannel
{
	UChannel( UAEImplementation& tImplementation,
			  const int soundId,
			  const float vPosition[3],
			  const float fVolumedB ) :
		mImplementation( tImplementation ),
		m_fmodChannel( nullptr ),
		m_stopRequested( false ),
		mSoundId( soundId ),
		mfSoundVolume( fVolumedB )
	{
		std::copy( vPosition, vPosition + 3, mvPosition );
		mStopFader.setInitialVolume( mImplementation.dBToVolume( getVolumedB() ) );
	};

	~UChannel() { m_fmodChannel = nullptr; }

	enum class State
	{
		INITIALIZE,
		TOPLAY,
		LOADING,
		PLAYING,
		STOPPING,
		STOPPED,
		//VIRTUALIZING,
		//VIRTUAL,
		//DEVIRTUALIZE,
	};

	UAEImplementation& mImplementation;
	::FMOD::Channel* m_fmodChannel;
	int mSoundId;
	float mvPosition[3];
	float mfVolumedB = 0.0f;
	float mfSoundVolume = 0.0f;
	State meState = State::INITIALIZE;
	bool m_stopRequested;
	UAudioFader mStopFader;
	//UAudioFader mVirtualizeFader;

	void update( float fTimeDeltaSeconds );
	void updateChannelParameters();
	bool isPlaying() const;
	float getVolumedB() const;
	void stop( const float fadeTimeSeconds = 0.f );
	void set3DAttributes( const FMOD_VECTOR* pos, const FMOD_VECTOR* vel );
	void setVolume( const float volume );

	//bool shouldBeVirtual( bool bAllowOneShotVirtuals ) const;
	//bool isOneShot() const;

	//const int SILENCE_dB = 10;
	//const int VIRTUALIZE_FADE_TIME = 10;
};

void UChannel::update( float fTimeDeltaSeconds )
{
	switch ( meState )
	{
		case UChannel::State::INITIALIZE:
			[[fallthrough]];
		//case UChannel::State::DEVIRTUALIZE:
		case UChannel::State::TOPLAY:
		{
			if ( m_stopRequested )
			{
				meState = State::STOPPING;
				return;
			}
			//if ( shouldBeVirtual( true ) )
			//{
			//	if ( isOneShot() )
			//	{
			//		meState = State::STOPPING;
			//	}
			//	else
			//	{
			//		meState = State::VIRTUAL;
			//	}
			//	return;
			//}
			if ( !mImplementation.soundIsLoaded( mSoundId ) )
			{
				mImplementation.loadSound( mSoundId );
				meState = State::LOADING;
				return;
			}
			m_fmodChannel = nullptr;
			auto tSoundIt = mImplementation.sounds.find( mSoundId );
			if ( tSoundIt != mImplementation.sounds.end() )
			{
				checkErrors( mImplementation.system->playSound( tSoundIt->second->m_fmodSound,
																nullptr,
																true,
																&m_fmodChannel ) );
			}
			if ( m_fmodChannel != nullptr )
			{
				//if ( meState == State::DEVIRTUALIZE )
				//	mVirtualizeFader.startFade( SILENCE_dB, 0.0f,
				//								VIRTUALIZE_FADE_TIME );
				meState = State::PLAYING;

				FMOD_MODE currMode;
				checkErrors( tSoundIt->second->m_fmodSound->getMode( &currMode ) );
				if ( currMode & FMOD_3D )
				{
					FMOD_VECTOR position = { mvPosition[0], mvPosition[1], mvPosition[2] };
					FMOD_VECTOR velocity = { 0, 0, 0 };
					checkErrors( m_fmodChannel->set3DAttributes( &position, &velocity ) );
				}
				checkErrors( m_fmodChannel->setVolume( mImplementation.dBToVolume( getVolumedB() ) ) );
				checkErrors( m_fmodChannel->setPaused( false ) );
			}
			else
			{
				meState = State::STOPPING;
			}
		}
		break;

		case UChannel::State::LOADING:
			if ( mImplementation.soundIsLoaded( mSoundId ) )
			{
				meState = State::TOPLAY;
			}
			break;

		case UChannel::State::PLAYING:
			//mVirtualizeFader.update( fTimeDeltaSeconds );
			updateChannelParameters();
			if ( !isPlaying() || m_stopRequested )
			{
				meState = State::STOPPING;
				return;
			}
			//if ( shouldBeVirtual( false ) )
			//{
			//	mVirtualizeFader.startFade( SILENCE_dB, VIRTUALIZE_FADE_TIME );
			//	meState = State::VIRTUALIZING;
			//}
			break;

		case UChannel::State::STOPPING:
			mStopFader.update( fTimeDeltaSeconds );
			updateChannelParameters();
			if ( mStopFader.isFinished() )
			{
				m_fmodChannel->stop();
			}
			if ( !isPlaying() )
			{
				meState = State::STOPPED;
				return;
			}
			break;

		case UChannel::State::STOPPED:
			break;

		//case UChannel::State::VIRTUALIZING:
		//	mVirtualizeFader.update( fTimeDeltaSeconds );
		//	updateChannelParameters();
		//	if ( !shouldBeVirtual( false ) )
		//	{
		//		mVirtualizeFader.startFade( 0.0f, VIRTUALIZE_FADE_TIME );
		//		meState = State::PLAYING;
		//		break;
		//	}
		//	if ( mVirtualizeFader.isFinished() )
		//	{
		//		m_fmodChannel->stop();
		//		meState = State::VIRTUAL;
		//	}
		//	break;

		//case UChannel::State::VIRTUAL:
		//	if ( m_stopRequested )
		//	{
		//		meState = State::STOPPING;
		//	}
		//	else if ( !shouldBeVirtual( false ) )
		//	{
		//		meState = State::DEVIRTUALIZE;
		//	}
		//	break;
	}
}

void UChannel::updateChannelParameters()
{
	if ( !mStopFader.isFinished() && mStopFader.isStarted() )
	{
		m_fmodChannel->setVolume( mStopFader.getVolume() );
	}
}

bool UChannel::isPlaying() const
{
	bool isPlaying = false;
	m_fmodChannel->isPlaying( &isPlaying );
	return isPlaying;
}

float UChannel::getVolumedB() const
{
	float volumeDb = 0.f;
	m_fmodChannel->getVolume( &volumeDb );
	return volumeDb;
}

void UChannel::stop( const float fadeTimeSeconds )
{
	m_stopRequested = true;
	if ( fadeTimeSeconds > 0.f )
	{
		mStopFader.startFade( 0/*SILENCE_dB*/, fadeTimeSeconds );
	}
	else
	{
		checkErrors( m_fmodChannel->stop() );
	}
}

void UChannel::set3DAttributes( const FMOD_VECTOR* position, const FMOD_VECTOR* velocity )
{
	checkErrors( m_fmodChannel->set3DAttributes( position, velocity ) );
}

void UChannel::setVolume( const float volume )
{
	checkErrors( m_fmodChannel->setVolume( volume ) );
	mStopFader.setInitialVolume( volume );
}

//bool UChannel::shouldBeVirtual( bool bAllowOneShotVirtuals ) const
//{
//
//}

//bool UChannel::isOneShot() const
//{
//	return false;
//}

/////////////////// UAEImplementation ///////////////////

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
		if ( it->second->meState == UChannel::State::STOPPED )
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

void UAEImplementation::loadSound( const int soundId, const void* data )
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
		checkErrors( system->createSound( (const char*) data, eMode, nullptr, &sound ) );
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

UAEImplementation* implementationPtr = nullptr;
}

/////////////////// UAudioEngine ///////////////////

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

void UAudioEngine::loadSound( const int soundId, const bool b3d, const bool bLooping, const bool bStream, const void* data )
{
	implementationPtr->loadSound( soundId, data );
}

void UAudioEngine::unLoadSound( const int soundId )
{
	implementationPtr->unloadSound( soundId );
}

int UAudioEngine::playSound( const int soundId, const float vPosition[3], const float fVolumedB )
{
	int channelId = implementationPtr->nextChannelId++;
	auto tSoundIt = implementationPtr->sounds.find( soundId );
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
		channel->stop();
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