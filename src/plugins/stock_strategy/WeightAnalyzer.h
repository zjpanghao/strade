/*
 * WeightAnalyzer.h
 *
 *  Created on: 2016年7月12日
 *      Author: YangGe
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_WEIGHTANALYZER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_WEIGHTANALYZER_H_
#include <string>

using namespace std;

namespace stock_logic {
//TODO 设置股票组合内股票权重
class WeightAnalyzer {
 public:
  WeightAnalyzer();
  virtual ~WeightAnalyzer();

  virtual void GetWeightByStock(std::string stock_name);

  WeightAnalyzer* Create(std::string weight_name, std::string weight_str);
};

}

#endif /* GEORGE_MASTER_PLUGINS_STOCK_WEIGHTANALYZER_H_ */
