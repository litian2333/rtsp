#ifndef WJRTSPCLIENT_H
#define WJRTSPCLIENT_H
#include "string.h"
class StreamClientState {
public:
    StreamClientState();
    virtual ~StreamClientState();

public:
    MediaSubsessionIterator* iter;
    MediaSession* session;
    MediaSubsession* subsession;
    TaskToken streamTimerTask;
    double duration;
};
class RTSPClient;
class UsageEnvironment;
typedef unsigned short portNumBits;
class WJRTSPClient: public RTSPClient {
public:
    static WJRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
                                    int verbosityLevel = 0,
                                    char const* applicationName = NULL,
                                    portNumBits tunnelOverHTTPPortNum = 0);

protected:
    WJRTSPClient(UsageEnvironment& env, char const* rtspURL,
                  int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
    // called only by createNew();
    virtual ~WJRTSPClient();

public:
    StreamClientState scs;
};

#endif //WJRTSPCLIENT_H