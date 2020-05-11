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
					   bool load = true );

	void unregisterSound( int soundId );

	void loadSound( const int soundId, bool b3d = true, bool bLooping = false, bool bStream = false );
	void unLoadSound( const int soundId );

	int playSound( const int soundId, const UVector3& vPos, float fVolumedB = 0.0f );

	void setChannel3dPosition( int nChannelId, const UVector3& vPosition );
	void setChannelVolume( int nChannelId, float fVolumedB );

	void set3dListenerAndOrientation( const UVector3& vPosition, const UVector3& vLook, const UVector3& vUp );
	void stopChannel( int nChannelId, float fFadeTimeSeconds = 0.f );
	void stopAllChannels();
	bool isPlaying( int nChannelId ) const;

	float dBToVolume( float dB );
	float volumeTodB( float volume );
};
}

#endif // !U_AUDIO_ENGINE_H_