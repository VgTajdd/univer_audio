#ifndef U_SOUND_H_
#define U_SOUND_H_

#include <string>

#include <fmod.hpp>

namespace univer::audio
{
class USound
{
public:
	explicit USound( const std::string& _name,
					 const float _defaultVolumeDB,
					 const float _minDistance,
					 const float _maxDistance,
					 const bool _is3d,
					 const bool _isLooping,
					 const bool _isStreaming,
					 const bool _useBinaryData );

	~USound();

	std::string name;
	float defaultVolumedB;
	float minDistance;
	float maxDistance;
	bool is3d;
	bool isLooping;
	bool isStreaming;
	bool useBinaryData;

	::FMOD::Sound* m_fmodSound;
};
}

#endif // U_SOUND_H_
