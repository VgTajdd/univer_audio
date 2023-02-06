// ========================================================================= //
// Copyright (c) 2021 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the Apache License 2.0.                       //
// UAudioFader.cpp                                                           //
// ========================================================================= //

#include "UAudioFader.h"

using univer::audio::UAudioFader;

UAudioFader::UAudioFader() :
	m_isFinished( false ),
	m_isStarted( false ),
	m_fadeTime( 0.f ),
	m_initialVolume( 0.f ),
	m_finalVolume( 0.f ),
	m_maxFadeTime( 0.f )
{}

UAudioFader::~UAudioFader()
{}

void UAudioFader::update( const float dt )
{
	if ( m_isFinished )
	{
		return;
	}
	m_fadeTime -= dt;
	if ( m_fadeTime < 0 )
	{
		m_isFinished = true;
		m_fadeTime = 0.f;
	}
}

void UAudioFader::startFade( const float volume, const float fadeTime )
{
	m_maxFadeTime = m_fadeTime = fadeTime;
	m_isStarted = true;
	m_isFinished = false;
	m_finalVolume = volume;
}

const float UAudioFader::getVolume() const
{
	float factor = m_fadeTime / m_maxFadeTime;
	float ans = ( 1 - factor ) * ( m_initialVolume - m_finalVolume );
	ans = m_initialVolume - ans;
	return ans;
}
