//
// Created by Harvey on 2016/12/29.
//

#include "mysql_engine.h"

namespace base_logic {

MysqlEngine::MysqlEngine(
    base::ConnAddr& read_addr,
    base::ConnAddr& write_addr)
    : read_addr_(read_addr),
      write_addr_(write_addr) {
  Initialize();
}

MysqlEngine::~MysqlEngine() {
  pthread_join(mysql_thread_, NULL);
}

void MysqlEngine::Initialize() {

  CreateMysqlReadEnginePool(MYSQL_READ_ENGINE_NUM);
  CreateMysqlWriteEnginePool(MYSQL_WRITE_ENGINE_NUM);

  int ret = -1;
  ret = pthread_create(&mysql_thread_, NULL, MysqlThread::Run, &shared_info_);
  if(0 != ret) {
    LOG_ERROR("mysql async thread create error");
    assert(0);
  }
}

bool MysqlEngine::CreateMysqlReadEnginePool(int32 pool_num) {
  std::list<base::ConnAddr> read_addr_list;
  read_addr_list.push_back(read_addr_);
  for (int i = 0; i < pool_num; ++i) {
    bool r = false;
    base_storage::DBStorageEngine* read_engine =
        base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
    if (read_engine == NULL) {
      LOG_ERROR("create db conntion error");
      assert(0);
    }

    // Create read engine
    r = read_engine->Connections(read_addr_list);
    if (!r) {
      LOG_ERROR("db conntion error");
      assert(0);
    }
    shared_info_.PushEngine(MYSQL_READ, read_engine);
  }
  return true;
}

bool MysqlEngine::CreateMysqlWriteEnginePool(int32 pool_num) {
  std::list<base::ConnAddr> write_addr_list;
  write_addr_list.push_back(write_addr_);

  // Create write engine
  for (int i = 0; i < pool_num; ++i) {
    bool r = false;
    base_storage::DBStorageEngine* write_engine =
        base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
    if (write_engine == NULL) {
      LOG_ERROR("create db conntion error");
      assert(0);
    }
    r = write_engine->Connections(write_addr_list);
    if (!r) {
      LOG_ERROR("db conntion error");
      assert(0);
    }
    shared_info_.PushEngine(MYSQL_WRITE, write_engine);
  }
  return true;
}

bool MysqlEngine::WriteData(const std::string& sql) {
  MySqlJobAdapter mysql_job(base_logic::MYSQL_WRITE, sql);
  mysql_job.WriteData(&shared_info_);
  return false;
}

bool MysqlEngine::ReadDataRows(const std::string& sql,
                               std::vector<MYSQL_ROW>& rows_vec) {
  MySqlJobAdapter mysql_job(base_logic::MYSQL_READ, sql);
  return mysql_job.ReadDataRows(&shared_info_, rows_vec);
}

bool MysqlEngine::ExcuteStorage(const std::string& sql,
                                std::vector<MYSQL_ROW>& rows_vec) {
  MySqlJobAdapter mysql_job(base_logic::MYSQL_STORAGE, sql);
  return mysql_job.ReadDataRows(&shared_info_, rows_vec);
}

bool MysqlEngine::AddAsyncMysqlJob(const std::string& sql,
                                   MysqlCallback callback,
                                   MYSQL_JOB_TYPE type) {
  MySqlJobAdapter* job = new MySqlJobAdapter(type, sql, callback);
  shared_info_.PushMysqlJob(job);
  return true;
}

} /* namespace base_logic */