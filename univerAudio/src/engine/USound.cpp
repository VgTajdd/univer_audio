#include "USound.h"

using univer::audio::USound;

USound::USound( const std::string& _name,
				const float _defaultVolumeDB,
				const float _minDistance,
				const float _maxDistance,
				const bool _is3d,
				const bool _isLooping,
				const bool _isStreaming ):
	name( _name ),
	defaultVolumedB( _defaultVolumeDB ),
	minDistance( _minDistance ),
	maxDistance( _maxDistance ),
	is3d( _is3d ),
	isLooping( _isLooping ),
	isStreaming( _isStreaming ),
	mpSound( nullptr )
{ }

USound::~USound()
{
	mpSound = nullptr;
}