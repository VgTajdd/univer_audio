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
	// std::cout << "FMOD all good" << std::endl;
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

	::FMOD::System* system;
	int nextChannelId;

	std::map< int, std::unique_ptr< USound > > sounds;
	std::map< int, std::unique_ptr< UChannel > > channels;

	int nextSoundId;
};

/////////////////// UChannel ///////////////////

struct UChannel
{
	UChannel( UAEImplementation& tImplementation,
			  int nSoundId,
			  const UVector3& vPosition,
			  float fVolumedB ) :
		mImplementation( tImplementation ),
		mpChannel( nullptr ),
		mbStopRequsted( false ),
		mSoundId( nSoundId ),
		mvPosition( vPosition ),
		mfSoundVolume( fVolumedB )
	{};
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
	bool mbStopRequsted;
	UAudioFader mStopFader;
	//UAudioFader mVirtualizeFader;
	void update( float fTimeDeltaSeconds );
	void updateChannelParameters();
	//bool shouldBeVirtual( bool bAllowOneShotVirtuals ) const;
	bool isPlaying() const;
	float getVolumedB() const;

	//bool isOneShot() const;
	float dBToVolume( const float db ) const;

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
			if ( mbStopRequsted )
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
				mImplementation.system->playSound( tSoundIt->second->mpSound,
												   nullptr, true, &mpChannel );
			}
			if ( mpChannel != nullptr )
			{
				//if ( meState == State::DEVIRTUALIZE )
				//	mVirtualizeFader.startFade( SILENCE_dB, 0.0f,
				//								VIRTUALIZE_FADE_TIME );
				meState = State::PLAYING;
				FMOD_VECTOR position = VectorToFmod( mvPosition );
				mpChannel->set3DAttributes( &position, nullptr );
				mpChannel->setVolume( dBToVolume( getVolumedB() ) );
				mpChannel->setPaused( false );
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
			if ( !isPlaying() || mbStopRequsted )
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
		//	if ( mbStopRequsted )
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

//bool UChannel::shouldBeVirtual( bool bAllowOneShotVirtuals ) const
//{
//
//}

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

//bool UChannel::isOneShot() const
//{
//	return false;
//}

float UChannel::dBToVolume( const float dB ) const
{
	return powf( 10.0f, 0.05f * dB );
}

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
	for ( const auto& [name, channel] : channels )
	{
		if ( channel->isPlaying() )
		{
			channel->mpChannel->stop();
		}
		channel->mpChannel = nullptr;
	}
	for ( const auto& [name, sound] : sounds )
	{
		checkErrors( sound->mpSound->release() );
		sound->mpSound = nullptr;
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
								 bool load )
{
	int nSoundId = implementationPtr->nextSoundId++;
	implementationPtr->sounds[nSoundId] = std::make_unique< USound >( _name,
																	  _defaultVolumeDB,
																	  _minDistance,
																	  _maxDistance,
																	  _is3d,
																	  _isLooping,
																	  _isStreaming );

	if ( load )
	{
		loadSound( nSoundId );
	}

	return nSoundId;
}

void UAudioEngine::unregisterSound( int soundId )
{
	if ( implementationPtr->soundIsLoaded( soundId ) )
	{
		unLoadSound( soundId );
	}
	implementationPtr->sounds.erase( soundId );
}

void UAudioEngine::loadSound( const int soundId, bool b3d, bool bLooping, bool bStream )
{
	implementationPtr->loadSound( soundId );
}

void UAudioEngine::unLoadSound( const int soundId )
{
	auto tFoundIt = implementationPtr->sounds.find( soundId );
	if ( tFoundIt == implementationPtr->sounds.end() )
	{
		return;
	}
	checkErrors( tFoundIt->second->mpSound->release() );
	tFoundIt->second->mpSound = nullptr;
}

int UAudioEngine::playSound( const int soundId, const UVector3& vPosition, float fVolumedB )
{
	int nChannelId = implementationPtr->nextChannelId++;
	auto tSoundIt = implementationPtr->sounds.find( soundId );
	if ( !implementationPtr->soundIsLoaded( soundId ) )
	{
		loadSound( soundId );
		if ( !implementationPtr->soundIsLoaded( soundId ) )
		{
			return nChannelId;
		}
	}
	implementationPtr->channels[nChannelId] = std::make_unique< UChannel >( *implementationPtr,
																			soundId,
																			vPosition,
																			fVolumedB );

	::FMOD::Channel* pChannel = nullptr;
	checkErrors( implementationPtr->system->playSound( tSoundIt->second->mpSound,
													   nullptr,
													   false,
													   &pChannel ) );

	if ( pChannel != nullptr )
	{
		FMOD_MODE currMode;
		tSoundIt->second->mpSound->getMode( &currMode );
		if ( currMode & FMOD_3D )
		{
			FMOD_VECTOR position = VectorToFmod( vPosition );
			FMOD_VECTOR velocity = { 0, 0, 0 };
			checkErrors( pChannel->set3DAttributes( &position, &velocity ) );
		}
		checkErrors( pChannel->setVolume( dBToVolume( fVolumedB ) ) );
		checkErrors( pChannel->setPaused( false ) );
		implementationPtr->channels[nChannelId]->mpChannel = pChannel;
	}
	return nChannelId;
}

void UAudioEngine::setChannel3dPosition( int nChannelId, const UVector3& vPosition )
{
	auto tFoundIt = implementationPtr->channels.find( nChannelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return;
	}

	FMOD_VECTOR position = VectorToFmod( vPosition );
	FMOD_VECTOR velocity = { 0, 0, 0 };
	checkErrors( tFoundIt->second->mpChannel->set3DAttributes( &position, &velocity ) );
}

void UAudioEngine::setChannelVolume( int nChannelId, float fVolumedB )
{
	auto tFoundIt = implementationPtr->channels.find( nChannelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return;
	}

	checkErrors( tFoundIt->second->mpChannel->setVolume( dBToVolume( fVolumedB ) ) );
}

void UAudioEngine::set3dListenerAndOrientation( const UVector3& vPosition, const UVector3& vLook, const UVector3& vUp )
{
	FMOD_VECTOR position = VectorToFmod( vPosition );
	FMOD_VECTOR speed = VectorToFmod( { 0, 0, 0 } );
	FMOD_VECTOR look = VectorToFmod( vLook );
	FMOD_VECTOR up = VectorToFmod( vUp );
	checkErrors( implementationPtr->system->set3DListenerAttributes( 0, &position, &speed, &look, &up ) );
}

void UAudioEngine::stopChannel( int nChannelId, float fFadeTimeSeconds )
{
	auto tFoundIt = implementationPtr->channels.find( nChannelId );
	if ( tFoundIt == implementationPtr->channels.end() )
	{
		return;
	}
	if ( fFadeTimeSeconds <= 0.0f )
	{
		checkErrors( tFoundIt->second->mpChannel->stop() );
	}
	else
	{
		tFoundIt->second->mbStopRequsted = true;
		tFoundIt->second->mStopFader.startFade( 0/*SILENCE_dB*/, fFadeTimeSeconds );
	}
}

void UAudioEngine::stopAllChannels()
{
	for ( const auto& [nChannel, channel] : implementationPtr->channels )
	{
		checkErrors( channel->mpChannel->stop() );
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
	checkErrors( tFoundIt->second->mpChannel->isPlaying( &isPlaying ) );
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
