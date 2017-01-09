//
// Created by Harvey on 2017/1/7.
//

#include "test_share_init.h"

#include "core/common.h"
#include "core/plugins.h"
#include "logic/logic_comm.h"
#include "test_share/test_share_logic.h"

#include <signal.h>

typedef struct hotsuggestplugin {
  char* id;
  char* name;
  char* version;
  char* provider;
} TestSharePlugin;

static void* OnTestShareInit() {
  signal(SIGPIPE, SIG_IGN);
  TestSharePlugin* testshare = (TestSharePlugin*) calloc(1, sizeof(TestSharePlugin));
  if (!testshare) {
    LOG_ERROR2("OnHostSuggestStart::calloc error, errno = %s", strerror(errno));
    assert(0);
  }
  testshare->id = "testshare";
  testshare->name = "testshare";
  testshare->version = "1.0.0";
  testshare->provider = "harvey";
  if(NULL == TestShareLogic::GetInstance()) {
    assert(0);
  }
  return testshare;
}

static handler_t OnIniTimer(struct server* srv) {

  return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server* srv, char* id, int opcode, int time) {

  return HANDLER_GO_ON;
}

int testshare_plugin_init(struct plugin* pl) {
  pl->init = OnTestShareInit;
  pl->clean_up = NULL;
  pl->connection = NULL;
  pl->connection_close = NULL;
  pl->connection_close_srv = NULL;
  pl->connection_srv = NULL;
  pl->handler_init_time = OnIniTimer;
  pl->handler_read = NULL;
  pl->handler_read_srv = NULL;
  pl->handler_read_other = NULL;
  pl->time_msg = OnTimeOut;
  pl->data = NULL;
  return 0;
}