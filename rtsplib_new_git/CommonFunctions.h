/*******************************************************
 文件：CommonFunctions.h
 作者：
 描述：公共函数定义，存放多个模块共用的一些函数，但若非必要，尽量不要往该文件中放
 版本：v1.0
 日期：2020-03-02
 *******************************************************/

#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <iostream>
#include <sstream>
#include <limits>
#include <unistd.h>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <arpa/inet.h>

#include <chrono>
#include <dirent.h>
#include <thread>
#include <string.h>

#include <complex>

#ifndef F_OK
#define F_OK 0
#endif

/**
 * 获取当前ms UTC时间
 * 参数：
 * 返回值：ms UTC时间
 */
inline uint64_t get_utcTime_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t l_ulTimeStamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return l_ulTimeStamp;
}

/**
 * 将指定数据转化为字符串
 * 参数：p_tValue 待转为字符串的数据
 * 返回值：字符串
 */
template <class T>
std::string ToStr(T p_tValue)
{
    std::ostringstream l_outStream;
    l_outStream.precision(std::numeric_limits<T>::digits10);
    l_outStream << p_tValue;
    return l_outStream.str();
}

/**
 * 将数值转化为字符串
 * 参数：p_tValue 待转为字符串的数据
 * 参数：p_tRadix 数值进制
 * 返回值：字符串
 */
template <class T>
std::string ToStr(T p_tValue, int p_tRadix)
{
    std::ostringstream l_outStream;
    l_outStream.precision(std::numeric_limits<T>::digits10);
    l_outStream << std::setbase(p_tRadix) << p_tValue;
    return l_outStream.str();
}

/**
 * 将字符串转为数字
*/
template <class T>
static T &FromString(const std::string &s, T &p_tValue, int p_tRadix = 10)
{
    std::istringstream l_inStream(s);
    l_inStream >> std::setbase(p_tRadix) >> p_tValue;
    if (l_inStream.fail())
    {
        p_tValue = T();
    }
    return p_tValue;
}

/**
 * 循环创建目录
 * 参数：p_strDirPath 待创建的目录路径（支持一次性创建多级目录）
 * 返回值：创建是否成功
 * 
    调用循环创建文件夹系统函数mkdir
    函数原型：int mkdir(const char *pathname, mode_t mode); 　　
    函数说明：mkdir()函数以mode方式创建一个以参数pathname命名的目录，mode定义新创建目录的权限。 　　
    返回值：若目录创建成功，则返回0；否则返回-1，并将错误记录到全局变量errno中。 　　
    mode方式：
    S_IRWXU 00700权限，代表该文件所有者拥有读，写和执行操作的权限 
    S_IRUSR(S_IREAD) 00400权限，代表该文件所有者拥有可读的权限 
    S_IWUSR(S_IWRITE) 00200权限，代表该文件所有者拥有可写的权限 
    S_IXUSR(S_IEXEC) 00100权限，代表该文件所有者拥有执行的权限 
    S_IRWXG 00070权限，代表该文件用户组拥有读，写和执行操作的权限 
    S_IRGRP 00040权限，代表该文件用户组拥有可读的权限 
    S_IWGRP 00020权限，代表该文件用户组拥有可写的权限 
    S_IXGRP 00010权限，代表该文件用户组拥有执行的权限 
    S_IRWXO 00007权限，代表其他用户拥有读，写和执行操作的权限 
    S_IROTH 00004权限，代表其他用户拥有可读的权限 
    S_IWOTH 00002权限，代表其他用户拥有可写的权限 
    S_IXOTH 00001权限，代表其他用户拥有执行的权限
 */


// inline bool MyMakeDirs(const std::string &p_strDirPath)
// {
//     bool bIsExist = (0 == access(p_strDirPath.c_str(), F_OK));
//     if (!bIsExist)
//     {
//         int l_nIsCreateOk = mkdir(p_strDirPath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
//         if (0 != l_nIsCreateOk)
//         {
//             std::cout << "MyMakeDirs failed!" << std::endl;
//             return false;
//         }
//     }

//     return true;
// }


//字符串分割函数
/**
 * 字符串分割函数
 * 参数：p_str 待分割字符串
 * 参数：p_strPattern 分割字符串
 * 返回值：字符串
 */
inline std::vector<std::string> strSplit(std::string p_str, std::string p_strPattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    p_str += p_strPattern; //扩展字符串以方便操作
    int size = p_str.size();
    for (int i = 0; i < size; i++)
    {
        pos = p_str.find(p_strPattern, i);
        if (pos < size)
        {
            std::string s = p_str.substr(i, pos - i);
            result.push_back(s);
            i = pos + p_strPattern.size() - 1;
        }
    }
    return result;
}
inline std::vector<std::string>m_split(std::string s,char token)
{
    std::stringstream iss(s);
    std::string word;
    std::vector<std::string> vs;
    while(getline(iss,word,token))
    {
        vs.push_back(word);
    }
    return  vs;
}
inline void rotate(double x, double y, double angle, double *x_res, double *y_res)
{
    constexpr double kPi = 3.141592653589793;
    auto cosa = cos(angle * kPi / 180);
    auto sina = sin(angle * kPi / 180);

    auto _x = x * cosa - y * sina;
    auto _y = x * sina + y * cosa;
    *x_res = _x;
    *y_res = _y;
}

/**
 * 获取目标8角点
 * 参数：8角点数组，x,y,z,w,l,h,angle
 * 返回值：无
 */
inline void getConners(double *box_info, double x, double y, double z, double w, double l, double h, double angle)
{
    double d[8][3] = {
        {w / 2, -l / 2, -h / 2},
        {w / 2, -l / 2, h / 2},
        {w / 2, l / 2, h / 2},
        {w / 2, l / 2, -h / 2},
        {-w / 2, -l / 2, -h / 2},
        {-w / 2, -l / 2, h / 2},
        {-w / 2, l / 2, h / 2},
        {-w / 2, l / 2, -h / 2},
    };
    for (auto i = 0; i < 8; ++i)
    {
        double x_delta;
        double y_delta;

        rotate(d[i][0], d[i][1], -angle, &x_delta, &y_delta);
        d[i][0] = x_delta + x;
        d[i][1] = y_delta + y;
        d[i][2] += z;
    }
    memcpy(box_info, d, sizeof(d));
}

/**
 * 经纬度局部坐标转换
 * 参数：
 * 返回值：
 */
class LonLatTransform
{
public:
    inline LonLatTransform(const double p_fdLongitude, const double p_fdLatitude, const double p_fdRotateAngle)
    {
        m_fdLonggitude0 = p_fdLongitude;   //基准点经度
        m_fdLatitude0 = p_fdLatitude;      //基准点纬度
        m_fdRotateAngle = p_fdRotateAngle; //基准点北向夹角
        m_fdMathPi = 3.141592653589793;    //PI
        m_fdLongRa = 6378137.;             //地球半径长轴
        m_fdShortRb = 6356752.3142;        //地球半径短轴
        m_fdCosA = cos(p_fdRotateAngle * m_fdMathPi / 180);
        m_fdSinA = sin(p_fdRotateAngle * m_fdMathPi / 180);
    }

    inline std::pair<double, double> lonlat2xy(const double p_fdLongitude, const double p_fdLatitude, const double p_fdRotateAngle = 0)
    {

        auto l_fdX = (p_fdLongitude - m_fdLonggitude0) * m_fdMathPi / 180 * m_fdLongRa * cos(m_fdLatitude0 * m_fdMathPi / 180);
        auto l_fdY = (p_fdLatitude - m_fdLatitude0) * m_fdMathPi / 180 * m_fdShortRb;
        if (p_fdRotateAngle != 0)
        {
            auto m_fdCosA = cos(p_fdRotateAngle * m_fdMathPi / 180);
            auto m_fdSinA = sin(p_fdRotateAngle * m_fdMathPi / 180);
        }

        return std::pair<double, double>(l_fdX * m_fdCosA - l_fdY * m_fdSinA, l_fdX * m_fdSinA + l_fdY * m_fdCosA);
    }
    inline std::pair<double, double> xy2lonlat(const double p_fdX, const double p_fdY, const double p_fdRotateAngle = 0)
    {
        if (p_fdRotateAngle != 0)
        {
            auto m_fdCosA = cos(p_fdRotateAngle * m_fdMathPi / 180);
            auto m_fdSinA = sin(p_fdRotateAngle * m_fdMathPi / 180);
        }
        auto l_fdX = p_fdX * m_fdCosA - p_fdY * m_fdSinA;
        auto l_fdY = p_fdX * m_fdSinA + p_fdY * m_fdCosA;

        auto p_fdLongitude = l_fdX / (m_fdMathPi / 180 * m_fdLongRa * cos(m_fdLatitude0 * m_fdMathPi / 180)) + m_fdLonggitude0;
        auto p_fdLatitude = l_fdY / (m_fdMathPi / 180 * m_fdShortRb) + m_fdLatitude0;
        return std::pair<double, double>(p_fdLongitude, p_fdLatitude);
    }
    inline double getAngle()
    {
        return m_fdRotateAngle;
    }

private:
    double m_fdLonggitude0;
    double m_fdLatitude0;
    double m_fdRotateAngle;
    double m_fdMathPi;
    double m_fdLongRa;
    double m_fdShortRb;
    double m_fdCosA;
    double m_fdSinA;
};

#define VMRSS_LINE 22
#define VMSIZE_LINE 13
#define PROCESS_ITEM 14

typedef struct
{
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
} Total_Cpu_Occupy_t;

typedef struct
{
    unsigned int pid;
    unsigned long utime;  //user time
    unsigned long stime;  //kernel time
    unsigned long cutime; //all user time
    unsigned long cstime; //all dead time
} Proc_Cpu_Occupy_t;

static const char *get_items(const char *buffer, unsigned int item)
{
    // read from buffer by offset
    const char *p = buffer;

    int len = strlen(buffer);
    int count = 0;

    for (int i = 0; i < len; i++)
    {
        if (' ' == *p)
        {
            count++;
            if (count == item - 1)
            {
                p++;
                break;
            }
        }
        p++;
    }

    return p;
}

static inline unsigned long get_cpu_total_occupy()
{
    // get total cpu use time

    // different mode cpu occupy time
    unsigned long user_time;
    unsigned long nice_time;
    unsigned long system_time;
    unsigned long idle_time;

    FILE *fd;
    char buff[1024] = {0};

    fd = fopen("/proc/stat", "r");
    if (nullptr == fd)
        return 0;

    fgets(buff, sizeof(buff), fd);
    char name[64] = {0};
    sscanf(buff, "%s %ld %ld %ld %ld", name, &user_time, &nice_time, &system_time, &idle_time);
    fclose(fd);

    return (user_time + nice_time + system_time + idle_time);
}

static inline unsigned long get_cpu_proc_occupy(int pid)
{
    // get specific pid cpu use time
    unsigned int tmp_pid;
    unsigned long utime;  // user time
    unsigned long stime;  // kernel time
    unsigned long cutime; // all user time
    unsigned long cstime; // all dead time

    char file_name[64] = {0};
    FILE *fd;
    char line_buff[1024] = {0};
    sprintf(file_name, "/proc/%d/stat", pid);

    fd = fopen(file_name, "r");
    if (nullptr == fd)
        return 0;

    fgets(line_buff, sizeof(line_buff), fd);

    sscanf(line_buff, "%u", &tmp_pid);
    const char *q = get_items(line_buff, PROCESS_ITEM);
    sscanf(q, "%ld %ld %ld %ld", &utime, &stime, &cutime, &cstime);
    fclose(fd);

    return (utime + stime + cutime + cstime);
}

inline float get_proc_cpu(int pid)
{

    unsigned long totalcputime1, totalcputime2;
    unsigned long procputime1, procputime2;

    totalcputime1 = get_cpu_total_occupy();
    procputime1 = get_cpu_proc_occupy(pid);

    // FIXME: the 200ms is a magic number, works well
    usleep(200000); // sleep 200ms to fetch two time point cpu usage snapshots sample for later calculation

    totalcputime2 = get_cpu_total_occupy();
    procputime2 = get_cpu_proc_occupy(pid);

    float pcpu = 0.0;
    if (0 != totalcputime2 - totalcputime1)
        pcpu = (procputime2 - procputime1) / float(totalcputime2 - totalcputime1); // float number

    int cpu_num = get_nprocs();
    pcpu *= cpu_num; // should multiply cpu num in multiple cpu machine

    return 100 * pcpu;
}

//获取进程占用内存
inline unsigned int get_proc_mem(unsigned int pid)
{

    char file_name[64] = {0};
    FILE *fd;
    char line_buff[512] = {0};
    sprintf(file_name, "/proc/%d/status", pid);

    fd = fopen(file_name, "r");
    if (NULL == fd)
    {
        //printf("没打开文件\n");
        return 0;
    }

    char name[64];
    int vmrss;
    for (int i = 0; i < VMRSS_LINE - 1; i++)
    {
        fgets(line_buff, sizeof(line_buff), fd);
    }

    fgets(line_buff, sizeof(line_buff), fd);
    sscanf(line_buff, "%s %d", name, &vmrss);
    fclose(fd);

    return int(vmrss);
}

inline int get_proc_thNum(int p_niPid)
{
    char szFileName[64] = {0};
    FILE *fd = NULL;
    char szLineBuff[512] = {0};
    sprintf(szFileName, "/proc/%ld/status", p_niPid);
    fd = fopen(szFileName, "r");
    if (fd == NULL)
    {
        printf("open file %s failed", szFileName);
        return -1;
    }
    int iThread = -1;
    while (fgets(szLineBuff, sizeof(szLineBuff), fd) != NULL)
    {
        char szName[64] = {0};
        int iValue = 0;
        sscanf(szLineBuff, "%s", szName); //%s遇空格停止
        // 进程使用的线程数
        if (strcmp(szName, "Threads:") == 0)
        {
            sscanf(szLineBuff, "%s %d", szName, &iValue);
            iThread = iValue;
            break;
        }
    }
    fclose(fd);
    return iThread;
}

/*******************************************************
 功能：时间转换为格式化毫秒时间戳
 输入：
    ms:ms时间
    ms:ms时间
 输出：
    格式化字符串时间戳
 版本：v1.0
 日期：2021-11-04
 *******************************************************/
inline std::string get_format_time(int64_t ms, std::string format)
{
    int microsec = ms % 1000;

    time_t t(ms / 1000);
    struct tm *p = localtime(&t);
    char s[64];
    int pos = strftime(s, sizeof(s), format.c_str(), p);

    sprintf(&s[pos], "%03d", microsec);

    return std::string(s);
}

/*******************************************************
 功能：获取当前时间的格式化毫秒时间戳
 输入：
    ms:ms时间
 输出：
    格式化字符串时间戳
 版本：v1.0
 日期：2021-11-04
 *******************************************************/
inline std::string
get_curr_format_time()
{
    struct timeval tv;
    gettimeofday(&tv, 0x0);

    int microsec = tv.tv_usec / 1000;
    time_t t(tv.tv_sec);
    struct tm *p = localtime(&t);
    char s[64];
    int pos = strftime(s, sizeof(s), "%Y.%m.%d-%H.%M.%S", p);

    sprintf(&s[pos], ".%03d", microsec);

    return std::string(s);
}

//判断是否是一个文件
inline bool IsFile(std::string p_strFilePath)
{
    struct stat buffer;
    return (stat(p_strFilePath.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

//判断是否是一个文件夹,
inline bool IsDir(std::string p_strFilefodler)
{
    struct stat buffer;
    return (stat(p_strFilefodler.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

//判断一个文件（或目录）路径是否存在
inline bool IsPathExist(std::string p_strFilePath)
{
    bool bIsExist = (0 == access(p_strFilePath.c_str(), F_OK));
    return bIsExist;
}

//可执行程序所有路径获取
inline std::string getExePath()
{
    char *p = NULL;
    const int len = 1024;
    /// to keep the absolute path of executable's path
    char l_pExecutionPath[len];
    memset(l_pExecutionPath, 0, len);

    int n = readlink("/proc/self/exe", l_pExecutionPath, len);
    if (NULL != (p = strrchr(l_pExecutionPath, '/')))
    {
        *p = '\0';
    }
    return l_pExecutionPath;
}
//获取文件夹下所有文件夹
inline bool get_all_files(const std::string &dir_in, std::vector<std::string> &files)
{
    if (dir_in.empty())
    {
        return false;
    }
    if (!IsDir(dir_in))
    {
        return false;
    }
    DIR *open_dir = opendir(dir_in.c_str());
    if (NULL == open_dir)
    {
        std::exit(EXIT_FAILURE);
    }
    dirent *p = nullptr;
    while ((p = readdir(open_dir)) != nullptr)
    {
        if (p->d_name[0] != '.')
        {
            std::string name = dir_in + std::string("/") + std::string(p->d_name);
            if (IsDir(name))
            {
                files.push_back(std::string(p->d_name));
            }
        }
    }
    closedir(open_dir);
    return true;
}

inline bool HEX2STR(const char *psrc, int srclen, char *pdst, int dstlen, bool bspace = false)
{
    int bytesize = 2 + (int)bspace;

    if (dstlen < srclen * bytesize)
        return false;

    for (int i = 0; i < srclen; i++)
        sprintf(pdst + i * bytesize, "%02X", (uint8_t)psrc[i]);

    if (bspace)
    {
        for (int i = 0; i < srclen - 1; i++)
            pdst[i * bytesize + 2] = 0x20;
    }

    return true;
}

//主机序转网络序
inline uint64_t htonll(uint64_t val)
{
    if (__BYTE_ORDER == __LITTLE_ENDIAN)
    {
        return (((uint64_t)htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
    }

    else if (__BYTE_ORDER == __BIG_ENDIAN)
    {
        return val;
    }
}

//网络序转主机序
inline uint64_t ntohll(uint64_t val)
{
    if (__BYTE_ORDER == __LITTLE_ENDIAN)
    {
        return (((uint64_t)ntohl((int)((val << 32) >> 32))) << 32) | (unsigned int)ntohl((int)(val >> 32));
    }

    else if (__BYTE_ORDER == __BIG_ENDIAN)
    {
        return val;
    }
}

#endif