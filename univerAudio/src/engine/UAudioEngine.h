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

	float dBToVolume( float dB );
	float volumeTodB( float volume );

	// ----------- TODO ------------
	//void set3dListenerAndOrientation(const Vector3& vPosition, const Vector3& vLook, const Vector3& vUp);
	//void stopChannel(int nChannelId);
	//void stopAllChannels();
	//bool isPlaying(int nChannelId) const;
	// ----------- TODO ------------
};
}

#endif // !U_AUDIO_ENGINE_H_