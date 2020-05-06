#ifndef U_AUDIO_ENGINE_H_
#define U_AUDIO_ENGINE_H_

#include <string>

namespace univer::audio
{
struct Vector3
{
	float x;
	float y;
	float z;
};

struct USound
{
	std::string name;
	float defaultVolumedB;
	float minDistance;
	float maxDistance;
	bool is3d;
	bool isLooping;
	bool isStreaming;
};

class UAudioEngine
{
public:
	void init();
	void update();
	void shutdown();

	void loadSound( const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false );
	void unLoadSound( const std::string& strSoundName );

	int playSound( const std::string& strSoundName, const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f );

	void setChannel3dPosition( int nChannelId, const Vector3& vPosition );
	void setChannelVolume( int nChannelId, float fVolumedB );

	void set3dListenerAndOrientation( const Vector3& vPosition, const Vector3& vLook, const Vector3& vUp );
	void stopChannel( int nChannelId );
	void stopAllChannels();
	bool isPlaying( int nChannelId ) const;

	float dBToVolume( float dB );
	float volumeTodB( float volume );
};
}

#endif // !U_AUDIO_ENGINE_H_