/*
 * HexunVisitDataObserver.cpp
 *
 *  Created on: 2016年9月12日
 *      Author: Maibenben
 */

#include "HexunVisitDataObserver.h"
#include "stock_factory.h"
#include "stock_db.h"

namespace stock_logic {

HexunVisitDataObserver::HexunVisitDataObserver(stock_logic::Subject* subject) {
  attr_name_ = "HexunVisitDataObserver_attr";
  name_ = "HexunVisitDataObserver";
  factory_ = (StockFactory*)subject;
}

HexunVisitDataObserver::~HexunVisitDataObserver() {
  // YGTODO Auto-generated destructor stub
}

void HexunVisitDataObserver::Update(int opcode, stock_logic::Subject* subject) {
  LOG_MSG2("HexunVisitDataObserver::Update opcode=%d",
             opcode);
  switch(opcode){

    case 0: {
      break;
    }

    default:
    break;
  }
}



} /* namespace stock_logic */
