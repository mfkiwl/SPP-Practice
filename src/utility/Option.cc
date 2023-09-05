#include "utility/Option.h"

namespace spp{

void Option::SetDefaults(){
    
    

}

void Option::SetFromFile(std::string filename){
    try{
        YAML::Node root_node = YAML::LoadFile(filename);
    }
    catch(YAML::BadFile &e){
        std::cerr << "Unable to load option file!" << std::endl;
        SetDefaults();
        return;
    }


}

bool Option::Check(){
    if (!fileOption){
        LOG(ERROR) << "NO fileOption";
        return false;
    }
    if (fileOption->ObsFile==""){
        LOG(ERROR) << "NO ObsFile";
        return false;
    }
    if (fileOption->NavFile==""){
        LOG(ERROR) << "NO NavFile";
        return false;
    }
    
    return true;
}


FileOption::FileOption(){
    NavFile = "";
    ObsFile = "";
    OutputDir = "";
    LogDir= "";
}

FileOption::~FileOption(){

}


Option::Option(){
    fileOption = std::make_shared<FileOption>();
}

Option::~Option(){

}


    
}