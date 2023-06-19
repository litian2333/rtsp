类：PyRtsp 接口介绍

    /**
     *  作用为注册一个python的接口作为c++内部的回调函数
     * @param fun  输入参数为python接口回调对象，接口参数为 1：py::bytes python字节流（原始h264码流） 2：int h264码流大小 3：long int 曝光时间戳（秒级×1000）4：unsigned int 曝光时间戳（毫秒） 5：long int 本地时间戳
     */
void registerPYCallBack(const std::function<void(py::bytes,const int &buffer_size,const long int& SecondTime,const unsigned int&MillisecondTime,const long int  &localTime)> &fun)
    
    /**
     * 作用是获取一帧数据的帧类型，当前只提供给库内部使用，python不可调用
     * @param frame 带0001标志位的h264码流
     * @return 帧类型：I、P、SPS、PPS
     */
std::string frameJudge(uint8_t *frame)

    /**
     * 作用是根据rtsp地址和相机建立连接实现拉流
     * @param url rtsp拉流地址
     * @param camera 建立相机连接的一个标志
     */
void OpenUrl(const std::string &url,const std::string &camera)

    /**
     *  获取当前库的版本
     * @return  当前rtsp拉流库版本号
     */
std::string version()

    /**
     *  作用是设置拉流返回数据只返回I帧use true:only use I Frame false :All Frame
     * @param use true:only use I Frame false :All Frame
     */
void onlyUseIFrame(const bool& use)

    /**
     * 作用是设置当相机曝光时间戳某帧出现错误值时，当前帧的输出曝光时间戳为上帧曝光时间戳加的某值 默认为2（如不使用默认，必须调用该接口修改增加值）
     * @param diff int类型时间累积值
     */
void setCameraTimeDiff(const int &diff)

    /**
     * 关闭拉流连接，外部调用程序退出的时候调用该接口
     */
void CloseConnect()

    /**
     * 不设置回调方式拉取数据，需要在python端主动调用的接口，根据结构体内部的接口获取对应数据
     * @return PyBufferInfo类型 
     */
PyBufferInfo getQueueInfo()

结构体：PyBufferInfo 接口介绍

    /**
     * 获取结构体中h264原始码流数据
     * @return py::bytes python字节流格式
     */
py::bytes getBuffer()

    /**
     * 获取结构体中曝光时间戳的秒级别时间
     * @return long int python中为正常的int类型
     */
const long int &getSecondTime()

    /**
     * 获取结构体中曝光时间戳的毫秒级别时间
     * @return unsigned int python中为正常的int类型
     */
const unsigned int &getMillisecondTime()

    /**
     * 获取结构体中落地时间戳
     * @return std::time_t python中为正常的int类型
     */
const std::time_t &getLocalTime()

    /**
     * 获取结构体中h264码流大小
     * @return int python中为正常的int类型
     */
const int & getBufferSize()

    /**
     * 获取结构体中h264码流类型
     * @return std::string python中为正常的str类型
     */
const std::string &getFrameType()