#ifndef U_AUDIO_ENGINE_H_
#define U_AUDIO_ENGINE_H_

#include <string>

namespace univer::audio
{
class UAudioEngine
{
public:
	void init();
	void update( const float dt );
	void shutdown();

	int registerSound( const std::string name,
					   const float defaultVolumeDB,
					   const float minDistance,
					   const float maxDistance,
					   const bool is3d,
					   const bool isLooping,
					   const bool isStreaming,
					   const bool load = true,
					   const bool useBinary = false );

	void unregisterSound( const int soundId );

	void loadSound( const int soundId,
					const bool b3d = true,
					const bool bLooping = false,
					const bool bStream = false,
					const void* data = nullptr );

	void unLoadSound( const int soundId );

	int playSound( const int soundId, const float vPos[3], const float fVolumedB = 0.0f );

	void setChannel3dPosition( const int channelId, const float vPosition[3] );
	void setChannelVolume( const int channelId, float fVolumedB );

	void set3dListenerAndOrientation( const float vPosition[3], const float vLook[3], const float vUp[3] );
	void stopChannel( const int channelId, const float fadeTimeSeconds = 0.f );
	void stopAllChannels();
	bool isPlaying( const int channelId ) const;

	float dBToVolume( const float dB );
	float volumeTodB( const float volume );
};
}

#endif // !U_AUDIO_ENGINE_H_