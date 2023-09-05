
#include "gnss/GnssCommon.h"

namespace spp
{
    class FileReaderBase
    {
    private:
        /* data */
    public:
        FileReaderBase(/* args */);
        ~FileReaderBase();
        virtual bool Read();
    };
    using FileReaderBasePtr = std::shared_ptr<FileReaderBase>;


    class ObsReader : public FileReaderBase{
    public:    
        bool Read();


    };
    using ObsReaderPtr = std::shared_ptr<ObsReader>;


    class NavReader : public FileReaderBase{
    public:
        bool Read();

    };
    using NavReaderPtr = std::shared_ptr<NavReader>;



} // namespace spp
