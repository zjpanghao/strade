/*
 * StockAccountManager.cpp
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#include "StockAccountManager.h"
#include "stock_factory.h"
#include "ChaliStrategy.h"
#include "HotRankOneHundredStrategy.h"
#include "HotRankFiftyHundredStrategy.h"
#include "CycleStockAccount.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

namespace stock_logic {

StockAccountManager::StockAccountManager(stock_logic::Subject* subject) {
  subject_ = subject;
  name_ = "StockAccountManager";
  hold_days_.push_back(7);
  //hold_days_.push_back(15);
  //hold_days_.push_back(30);
  //hold_days_.push_back(60);
  stop_ratios_.push_back(-0.05);
  limited_ratios_.push_back(1);
  traded_days_.push_back(7);
}

StockAccountManager::~StockAccountManager() {
}

bool StockAccountManager::createAccount(std::string& account_name,
                                        std::string& birthday,
                                        Strategy* strategy,
                                        std::string& industry_name) {
  std::map<std::string, StockAccount>::iterator iter =
      stock_accounts_.find(account_name);
  if (iter != stock_accounts_.end())
    return false;
  else {
    stock_accounts_[account_name] = StockAccount(birthday, strategy, industry_name);
    stock_accounts_[account_name].setAccountName(account_name);
    stock_accounts_[account_name].run();
    return true;
  }
}

bool StockAccountManager::createCycleAccount(std::string& account_name,
                                        std::string& birthday,
                                        Strategy* strategy,
                                        std::string& industry_name) {
  std::map<std::string, StockAccount>::iterator iter =
      stock_accounts_.find(account_name);
  if (iter != stock_accounts_.end())
    return false;
  else {
    stock_accounts_[account_name] = StockAccount(birthday, strategy, industry_name);
    stock_accounts_[account_name].setAccountName(account_name);
    stock_accounts_[account_name].cycle_run();
    return true;
  }
}

StockAccount* StockAccountManager::getAccountByName(std::string& account_name) {
  std::map<std::string, StockAccount>::iterator iter =
        stock_accounts_.find(account_name);
    if (iter != stock_accounts_.end())
      return &(iter->second);
    else
      return NULL;
}

void StockAccountManager::Update(int opcode, stock_logic::Subject* subject) {
  //this->runHotRankOneHundredStrategy(opcode, subject);
  this->runHotRankFiftyHundredStrategy(opcode, subject);
}

void StockAccountManager::runChaliStrategy(
    int opcode,
    stock_logic::Subject* subject) {
  factory_ = (StockFactory*)subject;
  std::string strage_name = "ChaliStrategy";
  ChaliStrategy* chaliStrategy = (ChaliStrategy*)factory_->GetObserverByName(strage_name);
  if (NULL == chaliStrategy) {
    LOG_MSG("assert!");
  }

  switch(opcode){

    case 0: {
      std::string start_date = "2016-01-01";
      std::string end_date = "2016-12-01";
      int account_num = 0;
      for (int i = 0; i < hold_days_.size(); i++) {
        for (int j = 0; j < limited_ratios_.size(); j++) {
          for (int k = 0; k < stop_ratios_.size(); k++) {
            std::map<std::string, HistDataPerDay>& hs300_hist_data =
                factory_->GetHistDataByCode(HSSANBAI);
            std::map<std::string, HistDataPerDay>::iterator hs300_iter =
                hs300_hist_data.begin();
            for (; hs300_iter != hs300_hist_data.end(); hs300_iter++) {
              std::string date = hs300_iter->first;
              if (date < start_date || date > end_date)
                continue;
              std::map<std::string, BasicIndustryInfo>& industry_map =
                  factory_->GetIndustryMap();
              std::map<std::string, BasicIndustryInfo>::iterator industry_map_iter =
                  industry_map.begin();
              for (; industry_map_iter != industry_map.end(); industry_map_iter++) {
                if (industry_map_iter->second.type_ != 0)
                  continue;
                //if (industry_map_iter->second.stock_price_info_.size() < 100)
                //  continue;
                std::string industry_name = industry_map_iter->first;
                std::string account_name = date;
                account_name += industry_name;
                char hold_day_str[50];
                sprintf(hold_day_str, "%d_%f_%f", hold_days_[i], limited_ratios_[j], stop_ratios_[k]);
                account_name += hold_day_str;
                LOG_MSG2("account_name=%s,hold_day=%d,limited_ratio=%f,stop_ratio=%f,account_num=%d",
                         account_name.c_str(),
                         hold_days_[i],
                         limited_ratios_[j],
                         stop_ratios_[k],
                         account_num);
                account_num++;
                chaliStrategy->setMaxHoldDate(hold_days_[i]);
                chaliStrategy->setSellRatio(limited_ratios_[j]);
                chaliStrategy->setStopRatio(stop_ratios_[k]);
                chaliStrategy->max_traded_day_ = hold_days_[i];
                createAccount(account_name, date, chaliStrategy, industry_name);
              }

            }
          }
        }
      }
      this->serialize();
      break;
    }

    default:
      break;
    }
}

void StockAccountManager::runHotRankOneHundredStrategy(
    int opcode,
    stock_logic::Subject* subject) {
  factory_ = (StockFactory*)subject;
    std::string strage_name = "HotRankOneHundredStrategy";
    HotRankOneHundredStrategy* hotRankOneHundredStrategy =
        (HotRankOneHundredStrategy*)factory_->GetObserverByName(strage_name);
    if (NULL == hotRankOneHundredStrategy) {
      LOG_MSG("hotRankOneHundredStrategy = NULL assert!");
    }

    switch(opcode){

      case 0: {
        std::string start_date = "2016-01-01";
        std::string end_date = "2016-11-30";
        int account_num = 0;
        for (int i = 0; i < hold_days_.size(); i++) {
          for (int j = 0; j < limited_ratios_.size(); j++) {
            for (int k = 0; k < stop_ratios_.size(); k++) {
              std::map<std::string, HistDataPerDay>& hs300_hist_data =
                  factory_->GetHistDataByCode(HSSANBAI);
              std::map<std::string, HistDataPerDay>::iterator hs300_iter =
                  hs300_hist_data.begin();
              for (; hs300_iter != hs300_hist_data.end(); hs300_iter++) {
                std::string date = hs300_iter->first;
                if (date < start_date || date > end_date)
                  continue;
                int temp_index = 0;
                for (int start_index = 0; ; ) {
                  //if (industry_map_iter->second.stock_price_info_.size() < 100)
                  //  continue;
                  std::string industry_name = "_";
                  std::string account_name = date;
                  account_name += industry_name;
                  char str_index[20];
                  sprintf(str_index, "%d", start_index);
                  account_name += str_index;
                  char hold_day_str[50];
                  sprintf(hold_day_str, "%d", hold_days_[i]);
                  //account_name += hold_day_str;
                  LOG_MSG2("account_name=%s,hold_day=%d,limited_ratio=%f,stop_ratio=%f,account_num=%d,start_index=%d",
                           account_name.c_str(),
                           hold_days_[i],
                           limited_ratios_[j],
                           stop_ratios_[k],
                           account_num,
                           start_index);
                  account_num++;
                  hotRankOneHundredStrategy->setMaxHoldDate(hold_days_[i]);
                  hotRankOneHundredStrategy->setSellRatio(limited_ratios_[j]);
                  hotRankOneHundredStrategy->setStopRatio(stop_ratios_[k]);
                  hotRankOneHundredStrategy->max_traded_day_ = hold_days_[i];
                  hotRankOneHundredStrategy->rank_index_ = start_index;
                  createAccount(account_name, date, hotRankOneHundredStrategy, industry_name);
                  int stock_count = stock_accounts_[account_name].stock_count_;
                  if (stock_count < 1)
                    break;
                  if (start_index == 0) {
                    start_index = 10;
                  } else if (start_index == 10) {
                    start_index = 50;
                  } else if (start_index == 50) {
                    start_index = 100;
                  } else if (start_index >= 100) {
                    start_index += 100;
                  }

                }

              }
            }
          }
        }
        this->serialize();
        break;
      }

      default:
        break;
      }
}


void StockAccountManager::runHotRankFiftyHundredStrategy(
    int opcode,
    stock_logic::Subject* subject) {
  factory_ = (StockFactory*)subject;
    std::string strage_name = "HotRankFiftyHundredStrategy";
    HotRankFiftyHundredStrategy* hotRankFiftyHundredStrategy =
        (HotRankFiftyHundredStrategy*)factory_->GetObserverByName(strage_name);
    if (NULL == hotRankFiftyHundredStrategy) {
      LOG_MSG("hotRankFiftyHundredStrategy = NULL assert!");
    }

    switch(opcode){

      case 0: {
        std::string start_date = "2016-01-01";
        std::string end_date = "2016-01-01";
        int account_num = 0;
        for (int i = 0; i < hold_days_.size(); i++) {
          for (int j = 0; j < limited_ratios_.size(); j++) {
            for (int k = 0; k < stop_ratios_.size(); k++) {
              std::map<std::string, HistDataPerDay>& hs300_hist_data =
                  factory_->GetHistDataByCode(HSSANBAI);
              std::map<std::string, HistDataPerDay>::iterator hs300_iter =
                  hs300_hist_data.begin();
              for (; hs300_iter != hs300_hist_data.end(); hs300_iter++) {
                std::string date = hs300_iter->first;
                if (date < start_date || date > end_date)
                  continue;
                for (int start_index = 50; ; ) {
                  //if (industry_map_iter->second.stock_price_info_.size() < 100)
                  //  continue;
                  std::string industry_name = "_";
                  std::string account_name = date;
                  account_name += industry_name;
                  char str_index[20];
                  sprintf(str_index, "%d", start_index);
                  account_name += str_index;
                  char hold_day_str[50];
                  sprintf(hold_day_str, "%d", hold_days_[i]);
                  //account_name += hold_day_str;
                  LOG_MSG2("account_name=%s,hold_day=%d,limited_ratio=%f,stop_ratio=%f,account_num=%d,start_index=%d",
                           account_name.c_str(),
                           hold_days_[i],
                           limited_ratios_[j],
                           stop_ratios_[k],
                           account_num,
                           start_index);
                  account_num++;
                  hotRankFiftyHundredStrategy->setMaxHoldDate(hold_days_[i]);
                  hotRankFiftyHundredStrategy->setSellRatio(limited_ratios_[j]);
                  hotRankFiftyHundredStrategy->setStopRatio(stop_ratios_[k]);
                  hotRankFiftyHundredStrategy->max_traded_day_ = hold_days_[i];
                  hotRankFiftyHundredStrategy->rank_index_ = start_index;
                  createCycleAccount(account_name, date, hotRankFiftyHundredStrategy, industry_name);
                  int stock_count = stock_accounts_[account_name].stock_count_;
                  if (stock_count < 1)
                    break;
                  break;
                  /*if (start_index == 50) {
                    start_index = 100;
                  } else if (start_index == 100) {
                    break;
                  }*/

                }

              }
            }
          }
        }
        this->serialize();
        break;
      }

      default:
        break;
      }
}

void StockAccountManager::delStockAccount(std::string& account_name) {

}

#define HANDLER_EINTR(x) ({\
    typeof(x) __eintr_result__;\
    do{\
        __eintr_result__ = x;\
    }while(__eintr_result__==-1&&errno==EINTR);\
    __eintr_result__;\
})

int WriteFileDescriptor(const int fd, const char* data, int size) {
  ssize_t bytes_written_total = 0;
  for (ssize_t bytes_written_partial = 0; bytes_written_partial < size;
      bytes_written_total += bytes_written_partial) {
    bytes_written_partial = HANDLER_EINTR(
        write(fd, data + bytes_written_total, size - bytes_written_total));
    if (bytes_written_partial < 0)
      return -1;
  }

  return bytes_written_total;
}

int WriteFile(const std::string& filename, const char* data, int size) {
  int ret = 0;
  //int fd = creat(filename.value().c_str(),0666);
  int fd = open(filename.c_str(), O_CREAT | O_APPEND | O_WRONLY,
                S_IRWXU | S_IRWXG | S_IRWXO);
  if (fd < 0)
    return -1;
  int bytes_written = WriteFileDescriptor(fd, data, size);
  //if((ret =HANDLER_EINTR(close(fd)))<0);
  //return ret;
  ret = close(fd);
  return bytes_written;
}

void StockAccountManager::serialize() {
  LOG_MSG("serialize account info");
  std::string result = "";
  result += "账户名称,";
  result += "持有期,";
  result += "卖出条件,";
  //result += "止盈线,";
  //result += "止损线,";
  result += "行业名称,";
  result += "策略开始日期,";
  result += "策略结束日期,";
  result += "本金,";
  result += "当前总资产,";
  result += "当前收益,";
  result += "当前收益率,";
  result += "股票列表";
  result += "\n";
  StockAccount total_account;
  total_account.setAccountName("账户汇总");
  total_account.max_hold_day_ = 7;
  double total_principal = 0;
  double total_total_asset = 0;
  double total_current_profit = 0;
  std::map<std::string, StockAccount>::iterator iter =
      stock_accounts_.begin();
  for (; iter != stock_accounts_.end(); iter++) {
    result += iter->second.serialize();
    total_principal += iter->second.principal_;
    total_total_asset += iter->second.total_asset_;
    total_current_profit += iter->second.current_profit_;
  }
  total_account.principal_ = total_principal;
  total_account.setTotalAsset(total_total_asset);
  total_account.current_profit_ = total_current_profit;
  total_account.current_profit_ratio_ = total_current_profit / total_principal;
  total_account.holding_stocks_["test"] = HoldingStock();
  result += total_account.serialize();

  this->serializeByIndustry(result);

  std::string file_name = "/home/yg/george-0527/george-master/bin/result.txt";
  //file::FilePath file_path(file_name);
  WriteFile(file_name, result.c_str(), (int)result.size());
}

void StockAccountManager::serializeByIndustry(std::string& result) {
  std::map<std::string, StockAccount> industry_accounts;
  std::map<std::string, StockAccount>::iterator iter =
      stock_accounts_.begin();
  for (; iter != stock_accounts_.end(); iter++) {
    StockAccount& the_account = iter->second;
    std::string industry_name = iter->second.industry_name_;
    StockAccount& industry_account = industry_accounts[industry_name];
    industry_account.account_name_ = industry_name;
    industry_account.principal_ += the_account.principal_;
    industry_account.current_profit_ += the_account.current_profit_;
    industry_account.total_asset_ += the_account.total_asset_;
    industry_account.current_profit_ratio_ = industry_account.current_profit_ /
        industry_account.principal_;
    std::map<std::string, HoldingStock>::iterator hold_stock_iter =
        the_account.holding_stocks_.begin();
    for (; hold_stock_iter != the_account.holding_stocks_.end(); hold_stock_iter++) {
      std::string stock_code = hold_stock_iter->first;
      industry_account.holding_stocks_[stock_code] = HoldingStock();
    }
  }
  std::map<std::string, StockAccount>::iterator industry_iter =
      industry_accounts.begin();
  for (; industry_iter != industry_accounts.end(); industry_iter++) {
    result += industry_iter->second.serialize();
  }
}
void StockAccountManager::serializeByBuyDate() {

}

} /* namespace stock_logic */
