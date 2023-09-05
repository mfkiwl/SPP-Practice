#include <Eigen/Core>
#include <Eigen/Dense>
#include <memory>


namespace spp{

// 等价权函数枚举
enum WeightFunctionType{
    None,
};


class Filter{
public:

// 等价权函数基类
class WeightFunctionBase{
public:
    // 虚函数重载()
    virtual Eigen::MatrixXd  operaator(Eigen::MatrixXd);
};
using WeightFunctionPtr = std::shared_ptr<WeightFunctionBase>;

// 无等价权函数
class NoneWeightFunction:public WeightFunctionBase{
    Eigen::MatrixXd  operaator(Eigen::MatrixXd);
};
using NoneWeightFunctionPtr = std::shared_ptr<NoneWeightFunction>;


    virtual void filter();

};
using  FilterPtr= std::shared_ptr<Filter>;







}