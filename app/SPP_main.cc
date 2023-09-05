

#include "glog/logging.h"
#include <Eigen/Core>
#include <Eigen/Dense>


#include "utility/FileReader.h"
#include "utility/Option.h"
#include "gnss/SppEstimater.h"


using namespace spp;

#define ENABLE_CONSOLE

// 如果定义了 ENABLE_CONSOLE，主函数执行的时候向控制台输出信息
#ifdef ENABLE_CONSOLE
#define OUTMESSAGE(msg) (std::cout <<"[SPP_main: "<<__LINE__<<"]  "<< msg << std::endl)
#else
#define OUTMESSAGE(msg) 
#endif


void PrintHelp(){
    std::cout << "SPP_PRACTICE help:" << std::endl;

}


bool CreatFileReader(FileOptionPtr fileOption,std::map<FileType,FileReaderBasePtr> fileReaderMap){

    return true;
}


bool ReadFile(FileOptionPtr fileOption){
    std::map<FileType,FileReaderBasePtr> fileReaderMap;
    if (!CreatFileReader(fileOption,fileReaderMap)){
        std::cerr << "Creat FileReader Failed" << std::endl;
        return false;
    }
    
    for (auto reader:fileReaderMap){
        if(!reader.second->Read()){
            std::cerr << "Read File Failed" << std::endl;
            return false;
        }
    }

    return true;
}


int main(int argc, char **argv){
    
    if (argc >= 2 && strcmp(argv[1],"-help")){
        PrintHelp();
        return 0;
    }

    long t1,t2; // 用于计时
    t1=clock();
    
    OUTMESSAGE("SPP_PRACTICE strat running...");

    // 初始化 glog
    google::InitGoogleLogging(argv[0]);

    // 读取配置文件
    OUTMESSAGE("Start loading options...");
    OptionsPtr option= std::make_shared<Option>();
    if (argc != 2) {
        OUTMESSAGE("Use defaulte options");
        option->SetDefaults();
    }
    else{
        std::string config_file_path = argv[1];
        option->SetFromFile(config_file_path);
    }

    // 检查配置是否合理
    if (!option->Check()){   
        OUTMESSAGE("Invalid options!");
        return 0;
    }
    
    // 读取数据文件
    OUTMESSAGE("Start reading file...");
    if (!ReadFile(option->fileOption)){
        std::cerr << "Read file failed!" << std::endl;
        return 0;
    }


    
    // SPP 解算
    OUTMESSAGE("Start SPP slove...");
    SppEstimaterPtr sppEstimater = std::make_unique<SppEstimater>();
    if (!sppEstimater->slove())
    {
        OUTMESSAGE("Read file failed!");
        return 0;
    }
    
    // 关闭 glog
    google::ShutdownGoogleLogging();


    OUTMESSAGE("SPP_PRACTICE end running...");

    // 输出运行时间
    t2=clock();
    std::string time_information ="The total time for running the program: " 
        + std::to_string((double)(t2-t1)/CLOCKS_PER_SEC) + " seconds";
    OUTMESSAGE(time_information);

    return 0;
}