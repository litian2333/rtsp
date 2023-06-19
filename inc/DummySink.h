#ifndef DUMMYSINK_H
#define DUMMYSINK_H
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <chrono>
class MediaSink;
class UsageEnvironment;
class MediaSubsession;
class InitConnect;
struct  WJTimeStamp;
typedef unsigned char Boolean;
class DummySink: public MediaSink {
public:
    static DummySink* createNew(UsageEnvironment& env,
                                MediaSubsession& subsession, // identifies the kind of data that's being received
                                InitConnect *connectPtr,
                                char const* streamId = NULL); // identifies the stream itself (optional)

    void setCouldCreat(const bool &c );

private:
    DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId,InitConnect *connectPtr);
    // called only by "createNew()"
    virtual ~DummySink();

    static void afterGettingFrame(void* clientData, unsigned frameSize,
                                  unsigned numTruncatedBytes,
                                  struct timeval presentationTime,
                                  unsigned durationInMicroseconds,
                                  struct timeval wjTimeStamp);
    void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
                           struct timeval wjTimeStamp, unsigned durationInMicroseconds);

private:
    // redefined virtual functions:
    virtual Boolean continuePlaying();
    void saveOneFrame(unsigned char *buffer,const int &buffer_size,struct timeval wjTimeStamp,std::time_t localTime);

private:
    u_int8_t* fReceiveBuffer;
    MediaSubsession& fSubsession;
    InitConnect *connPtr;
    char* fStreamId;
};
#endif //DUMMYSINK_H;