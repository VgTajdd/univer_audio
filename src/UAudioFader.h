// ========================================================================= //
// Copyright (c) 2021 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the Apache License 2.0.                       //
// UAudioFader.h                                                             //
// ========================================================================= //

#ifndef U_AUDIO_FADER_H_
#define U_AUDIO_FADER_H_

#include <string>

namespace univer::audio
{
class UAudioFader
{
public:
	explicit UAudioFader();
	~UAudioFader();
public:
	void update( const float dt );
	bool isFinished() const { return m_isFinished; }
	bool isStarted() const { return m_isStarted; }
	void startFade( const float volume, const float fadeTime );
	const float getVolume() const;
	void setInitialVolume( const float initialVolume ) { m_initialVolume = initialVolume; }
private:
	bool m_isFinished;
	bool m_isStarted;
	float m_fadeTime;
	float m_initialVolume;
	float m_finalVolume;
	float m_maxFadeTime;
};
}

#endif // U_AUDIO_FADER_H_
