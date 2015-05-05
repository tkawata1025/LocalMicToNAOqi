/**
 * @author Takuji Kawata
 * Updated 2015/05/05
 */

#ifndef NAO_MIC_INTERFACE_H
#define NAO_MIC_INTERFACE_H

#include <string>
#include <vector>

const int SAMPLERATE = 22050;      // 22050 Hz
const int CHANNELBYTES = 2;        // 16 bit signed short
const int NBOFOUTPUTCHANNELS = 2;  // stereo
const int BUFFERSAMPLESIZE = 16384;  // Sample size. (This value shouldn't exceed 16384)

class NaoMicInterface
{
	NaoMicInterface();

public:
	static NaoMicInterface* instance();

	~NaoMicInterface();
	void setNaoIp(const std::string ipAddress);
	void disconnect();
	bool isConnected() const;
	bool writeAudioBuffer(signed short *buffer, int samples);
	void clearBuffer();
	int  getSpeakerVolume();
	void setSpeakerVolume(int v);


};

#endif // NAO_MIC_INTERFACE_H
