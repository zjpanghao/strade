//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#ifndef GEORGE_VIP_PROTO_BUF_H__
#define GEORGE_VIP_PROTO_BUF_H__

#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "net/proto_buf.h"
#include <list>
#include <string>

namespace stock_logic {

namespace net_request {
class VIPNews : public george_logic::PacketHead {
 public:
  VIPNews() {
    uid_ = pos_ = count_ = NULL;
    token_ = NULL;
  }

  ~VIPNews() {
    if (uid_) {
      delete uid_;
      uid_ = NULL;
    }
    if (token_) {
      delete token_;
      token_ = NULL;
    }
    if (pos_) {
      delete pos_;
      pos_ = NULL;
    }
    if (count_) {
      delete count_;
      count_ = NULL;
    }
  }

  virtual void set_http_packet(base_logic::DictionaryValue* value);

  void set_uid(const int64 uid) {
    uid_ = new base_logic::FundamentalValue(uid);
  }

  void set_token(const std::string& token) {
    token_ = new base_logic::StringValue(token);
  }

  void set_pos(const int32 pos) {
    pos_ = new base_logic::FundamentalValue(pos);
  }

  void set_count(const int32 count) {
    count_ = new base_logic::FundamentalValue(count);
  }

 public:
  base_logic::FundamentalValue* uid_;
  base_logic::StringValue* token_;
  base_logic::FundamentalValue* pos_;
  base_logic::FundamentalValue* count_;
};
}

namespace net_reply {
class VIPNews {
 public:
  VIPNews() {
    article_type_ = NULL;
    vid_ = aid_ = article_time_ = NULL;
    name_ = title_ = article_source_ = NULL;
    value_ = NULL;

  }
  void set_vid(const int64 vid) {
    vid_ = new base_logic::FundamentalValue(vid);
  }
  void set_aid(const int64 aid) {
    aid_ = new base_logic::FundamentalValue(aid);
  }
  void set_name(const std::string& name) {
    name_ = new base_logic::StringValue(name);
  }
  void set_title(const std::string& title) {
    title_ = new base_logic::StringValue(title);
  }
  void set_article_source(const std::string& article_source) {
    article_source_ = new base_logic::StringValue(article_source);
  }
  void set_article_time(const int64 article_time) {
    article_time_ = new base_logic::FundamentalValue(article_time);
  }

  ~VIPNews() {
    if (vid_) {
      delete vid_;
      vid_ = NULL;
    }
    if (aid_) {
      delete aid_;
      aid_ = NULL;
    }
    if (name_) {
      delete name_;
      name_ = NULL;
    }
    if (title_) {
      delete title_;
      title_ = NULL;
    }
    if (article_source_) {
      delete article_source_;
      article_source_ = NULL;
    }
    if (article_type_) {
      delete article_type_;
      article_type_ = NULL;
    }
    if (article_time_) {
      delete article_time_;
      article_time_ = NULL;
    }
  }

  base_logic::DictionaryValue* get() {
    value_ = new base_logic::DictionaryValue();
    if (vid_ != NULL)
      value_->Set(L"vid", vid_);
    if (aid_ != NULL)
      value_->Set(L"aid", aid_);
    if (name_ != NULL)
      value_->Set(L"name", name_);
    if (title_ != NULL)
      value_->Set(L"title", title_);
    if (article_source_ != NULL)
      value_->Set(L"article_source", article_source_);
    if (article_type_ != NULL)
      value_->Set(L"article_type", article_type_);
    if (article_time_ != NULL)
      value_->Set(L"article_time", article_time_);
    return value_;
  }
 private:
  base_logic::FundamentalValue* vid_;
  base_logic::FundamentalValue* aid_;
  base_logic::StringValue* name_;
  base_logic::StringValue* title_;
  base_logic::StringValue* article_source_;
  base_logic::ListValue* article_type_;
  base_logic::FundamentalValue* article_time_;
  base_logic::DictionaryValue* value_;
};

class VIPLimitData {
 public:
  VIPLimitData() {
    surged_stock_num_ = NULL;
    decline_stock_num_ = NULL;
    time_ = NULL;
    value_ = NULL;
  }
  void set_surged_stock_num(const int64 surged_stock_num) {
    surged_stock_num_ = new base_logic::FundamentalValue(surged_stock_num);
  }
  void set_decline_stock_num(const int64 decline_stock_num) {
    decline_stock_num_ = new base_logic::FundamentalValue(decline_stock_num);
  }
  void set_time(const int64 current_time) {
    time_ = new base_logic::FundamentalValue(current_time);
  }

  ~VIPLimitData() {
    /*if(surged_stock_num_) {delete surged_stock_num_; surged_stock_num_ = NULL;}
     if(decline_stock_num_) {delete decline_stock_num_; decline_stock_num_ = NULL;}
     if(time_) {delete time_; time_ = NULL;}
     if(value_) {delete value_; value_ = NULL;}*/
  }

  base_logic::DictionaryValue* get() {
    value_ = new base_logic::DictionaryValue();
    if (surged_stock_num_ != NULL)
      value_->Set(L"surged_stock_num", surged_stock_num_);
    if (decline_stock_num_ != NULL)
      value_->Set(L"decline_stock_num", decline_stock_num_);
    if (time_ != NULL)
      value_->Set(L"time", time_);
    return value_;
  }
 private:
  base_logic::FundamentalValue* surged_stock_num_;
  base_logic::FundamentalValue* decline_stock_num_;
  base_logic::FundamentalValue* time_;
  base_logic::DictionaryValue* value_;
};

class STOIndustryData {
 public:
  STOIndustryData() {
    code_ = NULL;
    name_ = NULL;
    change_percent_ = NULL;
    volume_ = NULL;
    current_trade_ = NULL;
    date_ = NULL;
    open_ = NULL;
    high_ = NULL;
    close_ = NULL;
    low_ = NULL;
    support_ = NULL;
    pressure_ = NULL;
    day_yield_ = NULL;
    adjusted_day_yield_ = NULL;
    hs300_day_yield_ = NULL;
    hs300_adjusted_day_yield_ = NULL;
    month_init_price_ = NULL;
    month_init_date_ = NULL;
    qfq_close_ = NULL;
    time_ = NULL;
    visit_= NULL;
    value_ = NULL;
  }

  ~STOIndustryData() {
    /*if (NULL != code_) {
     delete code_;
     code_ = NULL;
     }
     if (NULL != name_) {
     delete name_;
     name_ = NULL;
     }
     if (NULL != change_percent_) {
     delete change_percent_;
     change_percent_ = NULL;
     }
     if (NULL != volume_) {
     delete volume_;
     volume_ = NULL;
     }
     if (NULL != current_trade_) {
     delete current_trade_;
     current_trade_ = NULL;
     }
     if (NULL != date_) {
     delete date_;
     date_ = NULL;
     }
     if (NULL != open_) {
     delete open_;
     open_ = NULL;
     }
     if (NULL != high_) {
     delete high_;
     high_ = NULL;
     }
     if (NULL != close_) {
     delete close_;
     close_ = NULL;
     }
     if (NULL != low_) {
     delete low_;
     low_ = NULL;
     }
     if (NULL != support_) {
     delete support_;
     support_ = NULL;
     }
     if (NULL != pressure_) {
     delete pressure_;
     pressure_ = NULL;
     }
     if (NULL != value_) {
     delete value_;
     value_ = NULL;
     }*/
  }

  void set_code(std::string code) {
    code_ = new base_logic::StringValue(code);
  }

  void set_name(std::string name) {
    name_ = new base_logic::StringValue(name);
  }

  void set_change_percent(const double change_percent) {
    change_percent_ = new base_logic::FundamentalValue(change_percent);
  }

  void set_volume(double volume) {
    volume_ = new base_logic::FundamentalValue(volume);
  }

  void set_current_trade(double current_trade) {
    current_trade_ = new base_logic::FundamentalValue(current_trade);
  }

  base_logic::DictionaryValue* get() {
    value_ = new base_logic::DictionaryValue();
    if (NULL != code_)
      value_->Set(L"code", code_);
    if (NULL != name_)
      value_->Set(L"name", name_);
    if (change_percent_ != NULL)
      value_->Set(L"change_percent", change_percent_);
    if (volume_ != NULL)
      value_->Set(L"volume", volume_);
    if (current_trade_ != NULL)
      value_->Set(L"current_trade", current_trade_);
    if (date_ != NULL)
      value_->Set(L"date", date_);
    if (open_ != NULL)
      value_->Set(L"open", open_);
    if (high_ != NULL)
      value_->Set(L"high", high_);
    if (close_ != NULL)
      value_->Set(L"close", close_);
    if (low_ != NULL)
      value_->Set(L"low", low_);
    if (support_ != NULL)
      value_->Set(L"support", support_);
    if (pressure_ != NULL)
      value_->Set(L"pressure", pressure_);
    if (day_yield_ != NULL)
      value_->Set(L"day_yield", day_yield_);
    if (visit_ != NULL)
      value_->Set(L"visit", visit_);
    if (hs300_day_yield_ != NULL)
      value_->Set(L"hs300_day_yield", hs300_day_yield_);
    if (hs300_adjusted_day_yield_ != NULL)
      value_->Set(L"hs300_adjusted_day_yield", hs300_adjusted_day_yield_);
    if (adjusted_day_yield_ != NULL)
      value_->Set(L"adjusted_day_yield", adjusted_day_yield_);
    if (month_init_price_ != NULL)
      value_->Set(L"month_init_price", month_init_price_);
    if (month_init_date_ != NULL)
      value_->Set(L"month_init_date", month_init_date_);
    if (qfq_close_ != NULL)
      value_->Set(L"qfq_close", qfq_close_);
    if (time_ != NULL)
      value_->Set(L"trade_time", time_);
    return value_;
  }

  void setClose(double close) {
    close_ = new base_logic::FundamentalValue(close);
  }

  void setDate(std::string date) {
    date_ = new base_logic::StringValue(date);
  }

  void setHigh(double high) {
    high_ = new base_logic::FundamentalValue(high);
  }

  void setLow(double low) {
    low_ = new base_logic::FundamentalValue(low);
  }

  void setOpen(double open) {
    open_ = new base_logic::FundamentalValue(open);
  }

  void setPressure(double pressure) {
    pressure_ = new base_logic::FundamentalValue(pressure);
  }

  void setSupport(double support) {
    support_ = new base_logic::FundamentalValue(support);
  }

  void setDayYield(double day_yield) {
    day_yield_ = new base_logic::FundamentalValue(day_yield);
  }

  void setVisit(int visit) {
    visit_ = new base_logic::FundamentalValue(visit);
  }

  void setAdjustedDayYield(double adjusted_day_yield) {
    adjusted_day_yield_ = new base_logic::FundamentalValue(adjusted_day_yield);
  }

  void setHS300DayYield(double hs300_day_yield) {
    hs300_day_yield_ = new base_logic::FundamentalValue(hs300_day_yield);
  }

  void setHS300AdjustedDayYield(double hs300_adjusted_day_yield) {
    hs300_adjusted_day_yield_ = new base_logic::FundamentalValue(
        hs300_adjusted_day_yield);
  }

  void setMonthInitPrice(double month_init_price) {
    month_init_price_ = new base_logic::FundamentalValue(month_init_price);
  }

  void setMonthInitDate(std::string month_init_date) {
    month_init_date_ = new base_logic::StringValue(month_init_date);
  }

  void setQfqClose(double qfq_close) {
    qfq_close_ = new base_logic::FundamentalValue(qfq_close);
  }

  void setTime(int32 trade_time) {
    time_ = new base_logic::FundamentalValue(trade_time);
  }

 private:
  base_logic::StringValue* code_;
  base_logic::StringValue* name_;
  base_logic::FundamentalValue* change_percent_;
  base_logic::FundamentalValue* volume_;
  base_logic::FundamentalValue* current_trade_;
  base_logic::StringValue* date_;
  base_logic::StringValue* month_init_date_;
  base_logic::FundamentalValue* open_;
  base_logic::FundamentalValue* high_;
  base_logic::FundamentalValue* close_;
  base_logic::FundamentalValue* low_;
  base_logic::FundamentalValue* support_;
  base_logic::FundamentalValue* pressure_;
  base_logic::FundamentalValue* day_yield_;
  base_logic::FundamentalValue* adjusted_day_yield_;
  base_logic::FundamentalValue* hs300_day_yield_;
  base_logic::FundamentalValue* hs300_adjusted_day_yield_;
  base_logic::FundamentalValue* month_init_price_;
  base_logic::FundamentalValue* qfq_close_;
  base_logic::FundamentalValue* time_;
  base_logic::FundamentalValue* visit_;
  base_logic::DictionaryValue* value_;
};

class VIPNewsList : public george_logic::PacketHead {
 public:
  VIPNewsList() {
    list_ = new base_logic::ListValue;
  }
  virtual ~VIPNewsList() {
    //if (list_) {delete list_; list_ = NULL;}

    //if (body_value_) {delete body_value_; body_value_ = NULL;}
  }

  void set_vip_news(base_logic::Value* value) {
    list_->Append(value);
  }

  base_logic::ListValue*& get_list() {
    return list_;
  }

  base_logic::DictionaryValue* body() {
    if (NULL == body_value_)
      body_value_ = new base_logic::DictionaryValue();
    body_value_->SetWithoutPathExpansion(L"list", list_);
    body_value_->SetInteger(L"count", list_->GetSize());
    return body_value_;
  }

  void set_string_value(std::wstring key, std::string value) {
    if (NULL == body_value_)
      body_value_ = new base_logic::DictionaryValue();
    body_value_->SetString(key, value);
  }

  void set_integer_value(std::wstring key, int32 value) {
    if (NULL == body_value_)
      body_value_ = new base_logic::DictionaryValue();
    body_value_->SetInteger(key, value);
  }

  void set_double_value(std::wstring key, double value) {
    if (NULL == body_value_)
      body_value_ = new base_logic::DictionaryValue();
    body_value_->SetReal(key, value);
  }

  void set_http_packet(base_logic::DictionaryValue* value) {
  }
 private:
  base_logic::ListValue* list_;
};

}
}
#endif
