#ifndef U_AUDIO_ENGINE_H_
#define U_AUDIO_ENGINE_H_

#include <string>

namespace univer::audio { struct UVector3; }

namespace univer::audio
{
class UAudioEngine
{
public:
	void init();
	void update( const float dt );
	void shutdown();

	int registerSound( const std::string _name,
					   const float _defaultVolumeDB,
					   const float _minDistance,
					   const float _maxDistance,
					   const bool _is3d,
					   const bool _isLooping,
					   const bool _isStreaming,
					   const bool load = true );

	void unregisterSound( const int soundId );

	void loadSound( const int soundId, const bool b3d = true, const bool bLooping = false, const bool bStream = false );
	void unLoadSound( const int soundId );

	int playSound( const int soundId, const UVector3& vPos, const float fVolumedB = 0.0f );

	void setChannel3dPosition( const int channelId, const UVector3& vPosition );
	void setChannelVolume( const int channelId, float fVolumedB );

	void set3dListenerAndOrientation( const UVector3& vPosition, const UVector3& vLook, const UVector3& vUp );
	void stopChannel( const int channelId, const float fadeTimeSeconds = 0.f );
	void stopAllChannels();
	bool isPlaying( const int channelId ) const;

	float dBToVolume( const float dB );
	float volumeTodB( const float volume );
};
}

#endif // !U_AUDIO_ENGINE_H_