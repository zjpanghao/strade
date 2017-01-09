//
// Created by Harvey on 2017/1/7.
//

#ifndef STRADE_TEST_SHARE_LOGIC_H
#define STRADE_TEST_SHARE_LOGIC_H

#include "strade_share/strade_share_engine.h"

class TestShareLogic {
 private:
  TestShareLogic() {
    Init();
  }

  void Init();
 public:
  static TestShareLogic* GetInstance() {
    static TestShareLogic instance;
    return &instance;
  }

  bool OnIniTimer(struct server* srv);
  bool OnTimeout(struct server* srv, char* id, int opcode, int time);

 private:
  strade_share::SSEngine* ssEngine_;

};

#endif //STRADE_TEST_SHARE_LOGIC_H
