/*
 * subject.cpp
 *
 *  Created on: 2016年8月1日
 *      Author: Maibenben
 */

#include "Subject.h"
#include "Observer.h"

namespace stock_logic {

Subject::Subject() {
  // YGTODO Auto-generated constructor stub

}

Subject::~Subject() {
  // YGTODO Auto-generated destructor stub
}

void Subject::Attach(Observer* pObserver)
{
    this->m_lst.push_back(pObserver);
}

void Subject::Detach(Observer* pObserver)
{
    list<Observer*>::iterator iter;
    iter = find(m_lst.begin(),m_lst.end(),pObserver);
    if(iter != m_lst.end())
    {
        m_lst.erase(iter);
    }
}

void Subject::Notify(int opcode)
{
    list<Observer*>::iterator iter = this->m_lst.begin();
    for(;iter != m_lst.end();iter++)
    {
        (*iter)->Update(opcode, this);
    }
}

Observer* Subject::GetObserverByName(std::string& name) {
  list<Observer*>::iterator iter;
  iter = m_lst.begin();
  for (; iter != m_lst.end(); iter++) {
    if (name == (*iter)->name())
      return *iter;
  }
  return NULL;
}

} /* namespace stock_logic */
