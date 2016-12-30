#ifndef GEORGE_MASTER_PLUGINS_STOCK_SUBJECT_H_
#define GEORGE_MASTER_PLUGINS_STOCK_SUBJECT_H_

//#include "Observer.h"
#include <list>
#include <map>
#include <iterator>
#include <algorithm>

using namespace std;

namespace stock_logic {

class Observer;

class Subject {
 public:
  Subject();
  virtual ~Subject();

  virtual void Notify(int opcode);
  virtual void Attach(Observer*);
  virtual void Detach(Observer*);
  Observer* GetObserverByName(std::string& name);

  list<Observer*> m_lst;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_SUBJECT_H_ */
