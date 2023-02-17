// ========================================================================= //
// Copyright (c) 2021 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the Apache License 2.0.                       //
// UChannel.cpp                                                              //
// ========================================================================= //

#include "UChannel.h"
#include "UAEImplementation.h"
#include "UAUtils.h"

using univer::audio::UChannel;

UChannel::UChannel( UAEImplementation& tImplementation,
		  const int soundId,
		  const float vPosition[3],
		  const float fVolumedB ) :
	m_implementation( tImplementation ),
	m_fmodChannel( nullptr ),
	m_soundId( soundId ),
	m_soundVolume( fVolumedB ),
	m_state( State::INITIALIZE ),
	m_stopRequested( false )
{
	std::copy( vPosition, vPosition + 3, m_position );
	m_stopFader.setInitialVolume( m_implementation.dBToVolume( getVolumedB() ) );
};

void UChannel::update( float fTimeDeltaSeconds )
{
	switch ( m_state )
	{
		case UChannel::State::INITIALIZE:
			[[fallthrough]];
		case UChannel::State::TOPLAY:
		{
			if ( m_stopRequested )
			{
				m_state = State::STOPPING;
				return;
			}
			if ( !m_implementation.soundIsLoaded( m_soundId ) )
			{
				m_implementation.loadSound( m_soundId );
				m_state = State::LOADING;
				return;
			}
			m_fmodChannel = nullptr;
			auto tSoundIt = m_implementation.sounds.find( m_soundId );
			if ( tSoundIt != m_implementation.sounds.end() )
			{
				checkErrors( m_implementation.system->playSound( tSoundIt->second->m_fmodSound,
																 nullptr,
																 true,
																 &m_fmodChannel ) );
			}
			if ( m_fmodChannel != nullptr )
			{
				m_state = State::PLAYING;

				FMOD_MODE currMode;
				checkErrors( tSoundIt->second->m_fmodSound->getMode( &currMode ) );
				if ( currMode & FMOD_3D )
				{
					FMOD_VECTOR position = { m_position[0], m_position[1], m_position[2] };
					FMOD_VECTOR velocity = { 0, 0, 0 };
					checkErrors( m_fmodChannel->set3DAttributes( &position, &velocity ) );
				}
				checkErrors( m_fmodChannel->setVolume( m_implementation.dBToVolume( getVolumedB() ) ) );
				checkErrors( m_fmodChannel->setPaused( false ) );
			}
			else
			{
				m_state = State::STOPPING;
			}
		}
		break;

		case UChannel::State::LOADING:
			if ( m_implementation.soundIsLoaded( m_soundId ) )
			{
				m_state = State::TOPLAY;
			}
			break;

		case UChannel::State::PLAYING:
			updateChannelParameters();
			if ( !isPlaying() || m_stopRequested )
			{
				m_state = State::STOPPING;
				return;
			}
			break;

		case UChannel::State::STOPPING:
			m_stopFader.update( fTimeDeltaSeconds );
			updateChannelParameters();
			if ( m_stopFader.isFinished() )
			{
				m_fmodChannel->stop();
			}
			if ( !isPlaying() )
			{
				m_state = State::STOPPED;
				return;
			}
			break;

		case UChannel::State::STOPPED:
			break;
	}
}

void UChannel::updateChannelParameters()
{
	if ( !m_stopFader.isFinished() && m_stopFader.isStarted() )
	{
		m_fmodChannel->setVolume( m_stopFader.getVolume() );
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
		m_stopFader.startFade( 0, fadeTimeSeconds );
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
	m_stopFader.setInitialVolume( volume );
}
