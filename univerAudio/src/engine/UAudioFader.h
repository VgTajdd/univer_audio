#ifndef U_AUDIO_FADER_H_
#define U_AUDIO_FADER_H_

#include <string>

namespace univer::audio
{
class UAudioFader
{
public:
	explicit UAudioFader() {}
public:
	void update( const float dt ) {}
	bool isFinished() const { return true; }
	void startFade( const float volume, const float a ) {}
	void startFade( const float volume, const float a, const float b ) {}
};
}

#endif // U_AUDIO_FADER_H_