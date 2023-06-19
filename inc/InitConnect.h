#ifndef INITCONNECT_H
#define INITCONNECT_H
#include <functional>
#include <chrono>
//#include <pybind11/pybind11.h>
//#include <pybind11/functional.h>
//namespace py = pybind11;
class RTSPClient;
class UsageEnvironment;
class MediaSubsession;
class MediaSubsessionIterator;
class MediaSession;
class TaskScheduler;
//MediaSubsession* subsession;
typedef void* TaskToken;
static unsigned rtspClientCount = 0;

typedef void (*CallFunc)(unsigned char *buffer, const int &buffer_size,const struct timeval &receive_time,const std::time_t &localTime);
class InitConnect
{
public:
    InitConnect();
    ~InitConnect();
    // RTSP 'response handlers':
    static void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
    static void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
    static void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);
//    void CallBackFrame(std::function<void(unsigned char * buffer, const int &buffer_size,const unsigned int& SecondTime,const unsigned int&MillisecondTime)>& callfunc,unsigned char * buffer, const int &buffer_size,const unsigned int& SecondTime,const unsigned int&MillisecondTime);
    //void PyCallBackFrame(std::function<void(py::bytes buffer, const int &buffer_size,const unsigned int& SecondTime,const unsigned int&MillisecondTime)>& callfunc,unsigned char*  buffer, const int &buffer_size,const unsigned int& SecondTime,const unsigned int&MillisecondTime);
    void saveOneFrame(unsigned char *buffer, const int &buffer_size,struct timeval wjTimeStamp,std::time_t localTime);
    void setCallBackFun(std::function<void(unsigned char * buffer, const int &buffer_size,const unsigned int& SecondTime,const unsigned int&MillisecondTime)>&func
            ,const bool &ok);
    void setCallBackFun(CallFunc fun);
//    void setPyCallBackFun(std::function<void(py::bytes buffer, const int &buffer_size,const unsigned int& SecondTime,const unsigned int&MillisecondTime)>&func
//            ,const bool &ok);
// Other event handler functions:
    static void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
    static void subsessionByeHandler(void* clientData, char const* reason);
    // called when a RTCP "BYE" is received for a subsession
    static void streamTimerHandler(void* clientData);
    // called at the end of a stream's expected duration (if the stream has not already signaled its end using a RTCP "BYE")

// The main streaming routine (for each "rtsp://" URL):
    void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL);

    void OpenUrl(char const* rtspURL,char const* proName);

    void CloseCamera();

// Used to iterate through each stream's 'subsessions', setting up each one:
    static void setupNextSubsession(RTSPClient* rtspClient);

// Used to shut down and close a stream (including its "RTSPClient" object):
    static void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);
// A function that outputs a string that identifies each stream (for debugging output).  Modify this if you wish:
//    UsageEnvironment& operator<<(UsageEnvironment& env, const RTSPClient& rtspClient); {
//        return env << "[URL:\"" << rtspClient.url() << "\"]: ";
//    }
//// A function that outputs a string that identifies each subsession (for debugging output).  Modify this if you wish:
//    UsageEnvironment& operator<<(UsageEnvironment& env, const MediaSubsession& subsession) {
//        return env << subsession.mediumName() << "/" << subsession.codecName();
//    }
//    void usage(UsageEnvironment& env, char const* progName) {
//        env << "Usage: " << progName << " <rtsp-url-1> ... <rtsp-url-N>\n";
//        env << "\t(where each <rtsp-url-i> is a \"rtsp://\" URL)\n";
//    }
public:
    bool getCurrentCould(){return could_back;}
    void setTimeDiff(const int &diff){time_diff = diff;}
private:
    char eventLoopWatchVariable = 0;
    TaskScheduler* scheduler;
    static InitConnect *conn_ptr;
    UsageEnvironment* env ;
    struct timeval last_time;
    int time_diff = 20;
    std::function<void(unsigned char * buffer, const int &buffer_size,const unsigned int& SecondTime,const unsigned int&MillisecondTime)> curr_caback = nullptr;
    CallFunc callFunc;
    //std::function<void(py::bytes,const int &buffer_size,const unsigned int& SecondTime,const unsigned int&MillisecondTime)> py_fun = nullptr;
    bool could_back = false;
    RTSPClient *current_client_ = nullptr;
};

#endif //INITCONNECT_H