// ========================================================================= //
// Copyright (c) 2023 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the Apache License 2.0.                       //
// UChannel.h                                                                //
// ========================================================================= //

#pragma once

#include "UAudioFader.h"

#include <fmod/fmod.hpp>

namespace univer::audio
{

class UAEImplementation;

struct UChannel
{
	UChannel( UAEImplementation& tImplementation,
			  const int soundId,
			  const float vPosition[3],
			  const float fVolumedB );

	~UChannel() { m_fmodChannel = nullptr; }

	enum class State
	{
		INITIALIZE,
		TOPLAY,
		LOADING,
		PLAYING,
		STOPPING,
		STOPPED
	};

	UAEImplementation& m_implementation;
	::FMOD::Channel* m_fmodChannel;
	int m_soundId;
	float m_position[3];
	float m_volumedB;
	float m_soundVolume;
	State m_state = State::INITIALIZE;
	bool m_stopRequested;
	UAudioFader m_stopFader;

	void update( float fTimeDeltaSeconds );
	void updateChannelParameters();
	bool isPlaying() const;
	float getVolumedB() const;
	void stop( const float fadeTimeSeconds = 0.f );
	void set3DAttributes( const FMOD_VECTOR* pos, const FMOD_VECTOR* vel );
	void setVolume( const float volume );
};
}
