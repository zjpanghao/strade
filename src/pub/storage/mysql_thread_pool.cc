//
// Created by Harvey on 2016/12/29.
//

#include "mysql_thread_pool.h"

namespace base_logic {

MysqlThreadPool::MysqlThreadPool(
    base::ConnAddr& read_addr,
    base::ConnAddr& write_addr,
    uint8 thread_num)
    : read_addr_(read_addr),
      write_addr_(write_addr),
      thread_num_(thread_num) {
  Initialize();
}

MysqlThreadPool::~MysqlThreadPool() {
  std::list<pthread_t>::iterator iter(threads_list_.begin());
  for (; iter != threads_list_.end(); ++iter) {
    pthread_join(*iter, NULL);
  }
}

void MysqlThreadPool::Initialize() {
  CreateSqlThread();
  CreateEnginePool();
}

void MysqlThreadPool::CreateSqlThread() {
  for (int i = 0; i < thread_num_; ++i) {
    pthread_t pid;
    pthread_create(&pid, NULL, MySqlThread::run, &shared_info_);
    threads_list_.push_back(pid);
  }
}

void MysqlThreadPool::CreateEnginePool() {
  std::list<base::ConnAddr> read_addr_list;
  read_addr_list.push_back(read_addr_);
  std::list<base::ConnAddr> write_addr_list;
  write_addr_list.push_back(write_addr_);
  for (int i = 0; i < thread_num_; ++i) {
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
    shared_info_.pushEngine(MYSQL_READ, read_engine);

    // Create write engine
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
    shared_info_.pushEngine(MYSQL_WRITE, write_engine);
  }
}

// 异步执行
void MysqlThreadPool::QueryAsync(MYSQL_JOB_TYPE type,
                                 base_logic::DictionaryValue* dict,
                                 MYSQL_HANDLER call_back,
                                 void* param) {
  MysqlJob* mysql_job = new MysqlJob(type, dict, call_back, param);
  pthread_mutex_lock(&shared_info_.mutex_);
  shared_info_.task_queue_.push(mysql_job);
  pthread_mutex_unlock(&shared_info_.mutex_);
  sem_post(&shared_info_.task_num_);
}

// 同步执行
bool MysqlThreadPool::QuerySync(MYSQL_JOB_TYPE type,
                                base_logic::DictionaryValue* dict,
                                MYSQL_HANDLER call_back,
                                void* param) {

  MysqlJob mysql_job(type, dict, call_back, param);
  mysql_job.Excute(&shared_info_);
  return true;
}

} /* namespace base_logic */