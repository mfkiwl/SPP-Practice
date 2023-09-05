
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "rtklib.h"
#include "glog/logging.h"



namespace spp {



// 数据处理选项类
class ProcessOption{
public:
    ProcessOption();
    ~ProcessOption();

    int nf;             // 频率数：单频、双频、三频
    int navsys;         // 卫星系统
    double elmin;       // 最低卫星高度角
    double snrmin;      // 最低信噪比
    double std[3];      // 初始协方差：[0]bias [1]iono [2]trop
    double prn[6];      // 过程噪声：[0]bias [1]iono [2]trop [3]acch [4]accv [5] pos
    double maxgdop;     // GDOP 阈值

};
using ProcessOptionPtr = std::shared_ptr<ProcessOption>;


// 结果选项类
class SolutionOption{
public:
    SolutionOption();
    ~SolutionOption();

    int posf;           // 结果格式 (SOLF_???) 
    int times;          // 时间系统 (TIMES_???) 
    int timef;          // 时间格式 (0:sssss.s,1:yyyy/mm/dd hh:mm:ss.s) 
    int timeu;          // time digits under decimal point 
    int degf;           // 经纬度格式 (0:ddd.ddd,1:ddd mm ss) 
    int outhead;        // 是否输出文件头 (0:no,1:yes)

};
using SolutionOptionPtr = std::shared_ptr<SolutionOption>;


enum FileType {
    Obs,Nav
};

// 文件选项类
class FileOption
{
public:
    FileOption();
    ~FileOption();

    std::string ObsFile;
    std::string NavFile;
    std::string LogDir;
    std::string OutputDir;
};
using FileOptionPtr =std::shared_ptr<FileOption>;





// 选项类
class Option {
public:

    Option();

    ~Option();

    void SetDefaults();

    void SetFromFile(std::string filename);

    bool Check();


    FileOptionPtr fileOption;
    ProcessOptionPtr processOption;
    SolutionOptionPtr solutionOption;
   

};
using OptionsPtr = std::shared_ptr<Option>;


// 第一个参数为 Node，第二个参数为关键字
// 判断配置文件的 node 里有没有你要的那个关键字，有的话再把对应的值作为第三个参数返回。
template<typename ValueType>
bool safeGet(const YAML::Node& node, const std::string& key, ValueType* value) {
    CHECK_NOTNULL(value);
    bool success = false;
    if(!node.IsMap()) {
        // 如果节点不是一个映射（即不是一个包含键值对的结构），则打印一条日志信息。 
        // LOG(INFO) << "Unable to get Node[\"" << key << "\"] because the node is not a map";
    } else {
        const YAML::Node sub_node = node[key];
        if(sub_node) {
            try {
            // 如果键存在于节点中，尝试将该键对应的值转换为指定的类型，并将结果存储到传递的指针中。
            *value = sub_node.as<ValueType>();
            success = true;
        } catch(const YAML::Exception& e) {
            // 如果在尝试将值转换为指定类型时抛出异常，则打印一条包含错误信息的日志信息。 
            // LOG(INFO) << "Error getting key \"" << key << "\" as type "
            //     << typeid(ValueType).name() << ": " << e.what();
        }
        } else {
            // // 如果键不存在于节点中，则打印一条日志信息。
        // LOG(INFO) << "Key \"" << key << "\" does not exist";
        }
    }
    return success;
}


// 检查参一中是否存在参二子配置选项，如果不存在有两种处理：
// 当参三为 true 时，LOG(FATAL) 退出程序
// 当参三为 false 时，LOG(INFO) 输出错误到日志文件
inline bool checkSubOption(
    YAML::Node& node, std::string subname, bool fatal = false)
{
    if (!node[subname].IsDefined()) {
        if (fatal) {
            LOG(FATAL) << "Unable to load " << subname << "!";
        }
        else {
            LOG(INFO) << "Unable to load " << subname << ". Using default instead.";
        }
        return false;
    }
    return true;
}



} // namespace SPP_PRACTICE