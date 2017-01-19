//
// Created by Harvey on 2016/12/29.
//

#ifndef STRADE_SRC_PUB_STORAGE_MYSQL_THREAD_POOL_H
#define STRADE_SRC_PUB_STORAGE_MYSQL_THREAD_POOL_H

#include "config/config.h"
#include "storage/storage.h"
#include "thread/base_thread_lock.h"
#include "logic/logic_comm.h"
#include "logic/base_values.h"
#include "net/comm_head.h"
#include "dao/abstract_dao.h"
#include "basic/scoped_ptr.h"
#include "logic/comm_head.h"

#include <list>
#include <queue>
#include <semaphore.h>
#include <mysql/mysql.h>
#include <sstream>

namespace base_logic {

enum MYSQL_JOB_TYPE {
  MYSQL_READ = 1,
  MYSQL_WRITE = 2,
  MYSQL_STORAGE = 3,
};

// 读数据链接数 >= 2
const int MYSQL_READ_ENGINE_NUM = 3;

// 写连接数 >= 2
const int MYSQL_WRITE_ENGINE_NUM = 2;

// 异步回调函数指针
typedef void (* MysqlCallback)(std::vector<MYSQL_ROW>& rows_vec);

namespace  {

struct MySqlJobAdapter;

typedef std::list<base_storage::DBStorageEngine*> MYSQL_ENGINE_POOL;
typedef std::queue<MySqlJobAdapter*> MYSQL_TASK_QUEUE;

struct MysqlEngineSharedInfo {
  MysqlEngineSharedInfo() {

    sem_init(&task_num_, 0, 0);                                 // 当前任务数，初始为0
    sem_init(&read_engine_num_, 0, MYSQL_READ_ENGINE_NUM);            // 读连接数
    sem_init(&write_engien_num_, 0, MYSQL_WRITE_ENGINE_NUM);          // 写连接数
    pthread_mutex_init(&mutex_, NULL);
  }

  ~MysqlEngineSharedInfo() {
    pthread_mutex_destroy(&mutex_);
    sem_destroy(&read_engine_num_);
    sem_destroy(&write_engien_num_);
    if (!db_read_pool_.empty()) {
      base_storage::DBStorageEngine* engine = db_read_pool_.front();
      db_read_pool_.pop_front();
      if (engine) {
        engine->Release();
        delete engine;
        engine = NULL;
      }
    }
    if (!db_write_pool_.empty()) {
      base_storage::DBStorageEngine* engine = db_write_pool_.front();
      db_write_pool_.pop_front();
      if (engine) {
        engine->Release();
        delete engine;
        engine = NULL;
      }
    }
  }

  bool PopMysqlJob(MySqlJobAdapter*& job) {
    sem_wait(&task_num_);
    pthread_mutex_lock(&mutex_);
    bool r = false;
    do {
      if (task_queue_.empty()) {
        r = false;
        break;
      }
      job = task_queue_.front();
      if(NULL != job) {
        task_queue_.pop();
      }
      r = true;
    } while (0);
    pthread_mutex_unlock(&mutex_);
    return r;
  }

  bool PushMysqlJob(MySqlJobAdapter* job) {
    pthread_mutex_lock(&mutex_);
    task_queue_.push(job);
    sem_post(&task_num_);
    pthread_mutex_unlock(&mutex_);
    return true;
  }

  base_storage::DBStorageEngine* PopEngine(MYSQL_JOB_TYPE engine_type) {
    base_storage::DBStorageEngine* engine = NULL;
    switch (engine_type) {
      case MYSQL_WRITE:
      case MYSQL_STORAGE: {
        sem_wait(&write_engien_num_);
        pthread_mutex_lock(&mutex_);
        engine = db_write_pool_.front();
        db_write_pool_.pop_front();
        break;
      }
      case MYSQL_READ: {
        sem_wait(&read_engine_num_);
        pthread_mutex_lock(&mutex_);
        engine = db_read_pool_.front();
        db_read_pool_.pop_front();
        break;
      }
      default: {
        LOG_ERROR("mysql job type illegal");
        return NULL;
      }
    }
    pthread_mutex_unlock(&mutex_);
    return engine;
  }

  void PushEngine(MYSQL_JOB_TYPE engine_type,
                  base_storage::DBStorageEngine* engine) {
    assert(NULL != engine);
    pthread_mutex_lock(&mutex_);
    switch (engine_type) {
      case MYSQL_WRITE:
      case MYSQL_STORAGE: {
        db_write_pool_.push_back(engine);
        sem_post(&write_engien_num_);
        break;
      }
      case MYSQL_READ: {
        db_read_pool_.push_back(engine);
        sem_post(&read_engine_num_);
        break;
      }
      default: {
        LOG_ERROR("mysql job type illegal");
        break;
      }
    }
    pthread_mutex_unlock(&mutex_);
  }

  sem_t task_num_;
  sem_t read_engine_num_;
  sem_t write_engien_num_;

  pthread_mutex_t mutex_;
  MYSQL_ENGINE_POOL db_read_pool_;
  MYSQL_ENGINE_POOL db_write_pool_;

  MYSQL_TASK_QUEUE task_queue_;
};

struct MySqlJobAdapter {
  MySqlJobAdapter(MYSQL_JOB_TYPE type,
                  const std::string& sql,
                  MysqlCallback callback = NULL)
      : type_(type),
        sql_(sql),
        callback_(callback) {
  }

  template<class T>
  bool ReadData(MysqlEngineSharedInfo* shared_info,
                std::vector<T>& result) {
    bool r = false;
    base_storage::DBStorageEngine* engine =
        shared_info->PopEngine(type_);
    if (NULL == engine) {
      LOG_DEBUG2("excute sql=%s error, engine NULL",
                 sql_.c_str());
      return false;
    }
    do {
      r = engine->SQLExec(sql_.c_str());
      if (!r) {
        r = false;
        LOG_ERROR("exec sql error");
        break;
      }
      MYSQL_ROW rows;
      int32 num = engine->RecordCount();
      if (engine->RecordCount() > 0) {
        while ((rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc))) {
          T obj;
          obj(rows);
          result.push_back(obj);
        }
      }
      r = true;
    } while (0);
    // 归还链接
    shared_info->PushEngine(type_, engine);
    return r;
  }

  bool ReadDataRows(MysqlEngineSharedInfo* shared_info,
                    std::vector<MYSQL_ROW>& rows_vec) {
    bool r = false;
    base_storage::DBStorageEngine* engine =
        shared_info->PopEngine(type_);
    if (NULL == engine) {
      LOG_DEBUG2("excute sql=%s error, engine NULL",
                 sql_.c_str());
      return false;
    }
    do {
      r = engine->SQLExec(sql_.c_str());
      if (!r) {
        r = false;
        LOG_ERROR("exec sql error");
        break;
      }
      MYSQL_ROW rows;
      int32 num = engine->RecordCount();
      if (engine->RecordCount() > 0) {
        while ((rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc))) {
          rows_vec.push_back(rows);
        }
      }
      r = true;
    } while (0);
    // 归还链接
    shared_info->PushEngine(type_, engine);
    return r;
  }

  bool WriteData(MysqlEngineSharedInfo* shared_info) {
    bool r = false;
    base_storage::DBStorageEngine* engine =
        shared_info->PopEngine(type_);
    if (NULL == engine) {
      LOG_DEBUG2("excute sql=%s error, engine NULL",
                 sql_.c_str());
      return false;
    }
    do {
      r = engine->SQLExec(sql_.c_str());
      if (!r) {
        r = false;
        LOG_ERROR("exec sql error");
        break;
      }
      r = true;
    } while (0);
    // 归还链接
    shared_info->PushEngine(type_, engine);
    return r;
  }

  std::string sql_;
  MYSQL_JOB_TYPE type_;
  MysqlCallback callback_;
};

struct MysqlThread {
  static void* Run(void* param) {
    LOG_DEBUG("mysql async thread run");
    MysqlEngineSharedInfo* shared_info =
        static_cast<MysqlEngineSharedInfo*>(param);
    base_storage::DBStorageEngine* db_engine = NULL;
    bool r = false;
    while (true) {
      MySqlJobAdapter* job = NULL;
      r = shared_info->PopMysqlJob(job);
      if (NULL == job) {
        LOG_DEBUG("mysql async PopMysqlJob error");
        continue;
      }
      scoped_ptr<MySqlJobAdapter> mysql_job(job);
      std::vector<MYSQL_ROW> rows_vec;
      r = mysql_job->ReadDataRows(shared_info, rows_vec);
      if (r && mysql_job->callback_) {
        mysql_job->callback_(rows_vec);
        continue;
      }
    }
    return NULL;
  }
};

} /* namespace */

class MysqlEngine {
 public:
  MysqlEngine(base::ConnAddr& read_addr,
              base::ConnAddr& write_addr);
  ~MysqlEngine();
  void Initialize();

 private:
  bool CreateMysqlReadEnginePool(int32 pool_num = 3);
  bool CreateMysqlWriteEnginePool(int32 pool_num = 2);

 public:
  // 获取对象
  template<class T>
  bool ReadData(const std::string& sql,
                std::vector<T>& result) {
    MySqlJobAdapter mysql_job(base_logic::MYSQL_READ, sql);
    return mysql_job.ReadData<T>(&shared_info_, result);
  }

  // 获取原始 MYSQL_ROW， 用于特殊处理
  bool ReadDataRows(const std::string& sql,
                    std::vector<MYSQL_ROW>& rows_vec);

  // 更新数据
  bool WriteData(const std::string& sql);

  // 执行存储过程
  bool ExcuteStorage(const std::string& sql,
                     std::vector<MYSQL_ROW>& rows_vec);

  // 异步添加查询任务
  bool AddAsyncMysqlJob(const std::string& sql,
                        MysqlCallback callback,
                        MYSQL_JOB_TYPE type = MYSQL_WRITE);

 private:
  base::ConnAddr read_addr_;
  base::ConnAddr write_addr_;
  MysqlEngineSharedInfo shared_info_;
  pthread_t mysql_thread_;
};

} /* namespace base_logic */

#endif //STRADE_SRC_PUB_STORAGE_MYSQL_THREAD_POOL_H
