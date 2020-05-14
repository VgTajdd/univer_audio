#include "UAudioEngine.h"
#include "USound.h"
#include "UVector3.h"
#include "UAudioFader.h"

#include "fmod.hpp"

#include <iostream>
#include <map>
#include <vector>

using univer::audio::UAudioEngine;
using univer::audio::USound;

namespace univer::audio
{
FMOD_VECTOR VectorToFmod( const UVector3& vPosition )
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
	void loadSound( const int soundId );
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
			  const UVector3& vPosition,
			  const float fVolumedB ) :
		mImplementation( tImplementation ),
		mpChannel( nullptr ),
		m_stopRequested( false ),
		mSoundId( soundId ),
		mvPosition( vPosition ),
		mfSoundVolume( fVolumedB )
	{};

	~UChannel() { mpChannel = nullptr; }

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
	::FMOD::Channel* mpChannel;
	int mSoundId;
	UVector3 mvPosition;
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
			mpChannel = nullptr;
			auto tSoundIt = mImplementation.sounds.find( mSoundId );
			if ( tSoundIt != mImplementation.sounds.end() )
			{
				checkErrors( mImplementation.system->playSound( tSoundIt->second->mpSound,
																nullptr,
																true,
																&mpChannel ) );
			}
			if ( mpChannel != nullptr )
			{
				//if ( meState == State::DEVIRTUALIZE )
				//	mVirtualizeFader.startFade( SILENCE_dB, 0.0f,
				//								VIRTUALIZE_FADE_TIME );
				meState = State::PLAYING;

				FMOD_MODE currMode;
				checkErrors( tSoundIt->second->mpSound->getMode( &currMode ) );
				if ( currMode & FMOD_3D )
				{
					FMOD_VECTOR position = VectorToFmod( mvPosition );
					FMOD_VECTOR velocity = { 0, 0, 0 };
					checkErrors( mpChannel->set3DAttributes( &position, &velocity ) );
				}
				checkErrors( mpChannel->setVolume( mImplementation.dBToVolume( getVolumedB() ) ) );
				checkErrors( mpChannel->setPaused( false ) );
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
				mpChannel->stop();
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
		//		mpChannel->stop();
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

}

bool UChannel::isPlaying() const
{
	bool isPlaying = false;
	mpChannel->isPlaying( &isPlaying );
	return isPlaying;
}

float UChannel::getVolumedB() const
{
	float volumeDb = 0.f;
	mpChannel->getVolume( &volumeDb );
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
		checkErrors( mpChannel->stop() ); // TO FIX.
	}
}

void UChannel::set3DAttributes( const FMOD_VECTOR* position, const FMOD_VECTOR* velocity )
{
	checkErrors( mpChannel->set3DAttributes( position, velocity ) );
}

void UChannel::setVolume( const float volume )
{
	checkErrors( mpChannel->setVolume( volume ) );
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
		if ( tFoundIt->second->mpSound != nullptr )
		{
			return true;
		}
	}
	return false;
}

void UAEImplementation::loadSound( const int soundId )
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

	FMOD_MODE eMode = /*FMOD_NONBLOCKING*/FMOD_DEFAULT;
	eMode |= tFoundIt->second->is3d ? ( FMOD_3D/* | FMOD_3D_INVERSETAPEREDROLLOFF*/ ) : FMOD_2D;
	eMode |= tFoundIt->second->isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= tFoundIt->second->isStreaming ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	::FMOD::Sound* sound = nullptr;
	checkErrors( system->createSound( tFoundIt->second->name.c_str(), eMode, nullptr, &sound ) );
	if ( sound != nullptr )
	{
		checkErrors( sound->set3DMinMaxDistance( tFoundIt->second->minDistance, tFoundIt->second->maxDistance ) );
		tFoundIt->second->mpSound = sound;
	}
}

void UAEImplementation::unloadSound( const int soundId )
{
	auto tFoundIt = sounds.find( soundId );
	if ( tFoundIt == sounds.end() )
	{
		return;
	}
	if ( tFoundIt->second->mpSound != nullptr )
	{
		checkErrors( tFoundIt->second->mpSound->release() );
	}
	tFoundIt->second->mpSound = nullptr;
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

int UAudioEngine::registerSound( const std::string _name,
								 const float _defaultVolumeDB,
								 const float _minDistance,
								 const float _maxDistance,
								 const bool _is3d,
								 const bool _isLooping,
								 const bool _isStreaming,
								 const bool load )
{
	int soundId = implementationPtr->nextSoundId++;
	implementationPtr->sounds[soundId] = std::make_unique< USound >( _name,
																	  _defaultVolumeDB,
																	  _minDistance,
																	  _maxDistance,
																	  _is3d,
																	  _isLooping,
																	  _isStreaming );

	if ( load )
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

void UAudioEngine::loadSound( const int soundId, const bool b3d, const bool bLooping, const bool bStream )
{
	implementationPtr->loadSound( soundId );
}

void UAudioEngine::unLoadSound( const int soundId )
{
	implementationPtr->unloadSound( soundId );
}

int UAudioEngine::playSound( const int soundId, const UVector3& vPosition, const float fVolumedB )
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

void UAudioEngine::setChannel3dPosition( const int channelId, const UVector3& vPosition )
{
	auto tFoundIt = implementationPtr->channels.find( channelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return;
	}

	FMOD_VECTOR position = VectorToFmod( vPosition );
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

void UAudioEngine::set3dListenerAndOrientation( const UVector3& vPosition, const UVector3& vLook, const UVector3& vUp )
{
	FMOD_VECTOR position = VectorToFmod( vPosition );
	FMOD_VECTOR speed = VectorToFmod( { 0, 0, 0 } );
	FMOD_VECTOR look = VectorToFmod( vLook );
	FMOD_VECTOR up = VectorToFmod( vUp );
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