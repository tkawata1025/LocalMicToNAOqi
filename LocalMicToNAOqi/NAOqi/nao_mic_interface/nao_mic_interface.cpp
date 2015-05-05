/**
 * @author Takuji Kawata
 * Updated 2015/05/05
 */

#define DLLAPI __declspec(dllexport)

#include "nao_mic_interface.h"

#include <signal.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include "audiocaptureremote.h"

#ifdef AVCAPTURE_IS_REMOTE
# define ALCALL
#else
# ifdef _WIN32
#  define ALCALL __declspec(dllexport)
# else
#  define ALCALL
# endif
#endif

#include <iostream>
#include <alcommon/alproxy.h>
#include <boost/shared_ptr.hpp>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include <alproxies/alaudiodeviceproxy.h>
#include <pthread.h>

static AL::ALAudioDeviceProxy *s_audiodeviceProxy = NULL;
static std::string s_robotIpAddress = "";

static pthread_mutex_t	s_mutex;

class ThreadLockHelper
{
	pthread_mutex_t *d_mutex;
public:
	ThreadLockHelper(pthread_mutex_t &mutex) : d_mutex(&mutex)
	{
		pthread_mutex_lock(d_mutex);
	}

	~ThreadLockHelper()
	{
		pthread_mutex_unlock(d_mutex);
	}
};

#define LOCKER(mutex) ThreadLockHelper __locker(mutex);((void)__locker);

//static
NaoMicInterface* NaoMicInterface::instance()
{
	static NaoMicInterface sInstance;

	return &sInstance;
}

NaoMicInterface::NaoMicInterface()
{
}

NaoMicInterface::~NaoMicInterface()
{
	disconnect();
}

void NaoMicInterface::setNaoIp(const std::string ipAddress)
{

	disconnect();

	LOCKER(s_mutex);

	static boost::shared_ptr<AL::ALBroker> broker;

	if (s_robotIpAddress != ipAddress)
	{

		int parentBrokerPort = 9559;

		// Need this to for SOAP serialization of floats to work
		setlocale(LC_NUMERIC, "C");

		// A broker needs a name, an IP and a port:
		const std::string brokerName = "mybroker";
		// FIXME: would be a good idea to look for a free port first
		int brokerPort = 54000;
		const std::string brokerIp   = "0.0.0.0";  // listen to anything

		try
		{
			broker = AL::ALBroker::createBroker(
				brokerName,
				brokerIp,
				brokerPort,
				ipAddress,
				parentBrokerPort,
				0    // you can pass various options for the broker creation,
					// but default is fine
			);
		}
		catch(const AL::ALError& /* e */)
		{
			std::cerr << "Faild to connect broker to: "
			<< ipAddress
			<< ":"
			<< parentBrokerPort
			<< std::endl;
			AL::ALBrokerManager::getInstance()->killAllBroker();
			AL::ALBrokerManager::kill();
			return;
		}

		// Deal with ALBrokerManager singleton:
		AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
		AL::ALBrokerManager::getInstance()->addBroker(broker);

        try
        {
			s_audiodeviceProxy = new AL::ALAudioDeviceProxy();
			s_audiodeviceProxy->setParameter(std::string("outputSampleRate"),(int)SAMPLERATE);  

		}
		catch( AL::ALError e)
		{
			std::string msg = e.what();
			throw msg;
		}
		s_robotIpAddress = ipAddress;

    }
}

void NaoMicInterface::disconnect()
{
	LOCKER(s_mutex);

	if (s_audiodeviceProxy)
	{
		delete s_audiodeviceProxy;
	}
	s_audiodeviceProxy = NULL;

	AL::ALBrokerManager::getInstance()->killAllBroker();
	AL::ALBrokerManager::kill();

	s_robotIpAddress = "0.0.0.0";

}

bool NaoMicInterface::isConnected() const
{
	LOCKER(s_mutex);

    return s_audiodeviceProxy != NULL;
}

bool NaoMicInterface::writeAudioBuffer(signed short *buffer, int samples)
{
	bool r = false;
	if (s_audiodeviceProxy)
	{
		AL::ALValue pDataBin;
		pDataBin.SetBinary(buffer, samples*sizeof(signed short)*NBOFOUTPUTCHANNELS);
		r = s_audiodeviceProxy->sendRemoteBufferToOutput(samples, pDataBin);
	}
	return r;
}

void NaoMicInterface::clearBuffer()
{
	if (s_audiodeviceProxy)
	{
		s_audiodeviceProxy->flushAudioOutputs();
	}
}

int  NaoMicInterface::getSpeakerVolume()
{
	int v = 0;
	if (s_audiodeviceProxy)
	{
		v = s_audiodeviceProxy->getOutputVolume();
	}
	return v;
}

void NaoMicInterface::setSpeakerVolume(int v)
{
	if (s_audiodeviceProxy)
	{
		s_audiodeviceProxy->setOutputVolume(v);
	}
}


