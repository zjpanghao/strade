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

#include <list>
#include <queue>
#include <semaphore.h>
#include <mysql/mysql.h>
#include <sstream>

#define MYSQL_RESULT_LIST               L"result_list"
#define MYSQL_QUERY_TYPE                L"query_type"
#define MYSQL_QUERY_PARAM               L"query_param"
#define MYSQL_QUERY_CONDITION           L"query_condition"
#define MYSQL_STORAGE_NAME              L"storage_name"

namespace base_logic {

typedef void (* MYSQL_HANDLER)(base_logic::DictionaryValue*, void* param);
typedef std::list<base_storage::DBStorageEngine*> MYSQL_ENGINE_POOL;

namespace {

class MysqlJob;
typedef std::queue<MysqlJob*> MYSQL_TASK_QUEUE;

struct MysqlThreadSharedInfo {
  MysqlThreadSharedInfo() {
    sem_init(&task_num_, 0, 0);
    pthread_mutex_init(&mutex_, NULL);
  }

  ~MysqlThreadSharedInfo() {
    sem_destroy(&task_num_);
    pthread_mutex_destroy(&mutex_);
  }

  base_storage::DBStorageEngine* popEngine(MYSQL_JOB_TYPE engine_type) {
    pthread_mutex_lock(&mutex_);
    base_storage::DBStorageEngine* engine = NULL;
    if (MYSQL_WRITE == engine_type || MYSQL_STORAGE == engine_type) {
      engine = db_write_pool_.front();
      db_write_pool_.pop_front();
    } else if (MYSQL_READ == engine_type) {
      engine = db_read_pool_.front();
      db_read_pool_.pop_front();
    } else {
      LOG_ERROR("mysql job type illegal");
    }
    pthread_mutex_unlock(&mutex_);
    return engine;
  }

  void pushEngine(MYSQL_JOB_TYPE engine_type,
                  base_storage::DBStorageEngine* engine) {
    assert(NULL != engine);
    pthread_mutex_lock(&mutex_);
    if (MYSQL_WRITE == engine_type || MYSQL_STORAGE == engine_type) {
      db_write_pool_.push_back(engine);
    } else if (MYSQL_READ == engine_type) {
      db_read_pool_.push_back(engine);
    } else {
      LOG_ERROR("mysql job type illegal");
    }
    pthread_mutex_unlock(&mutex_);
  }

  MysqlJob* popJob() {
    pthread_mutex_lock(&mutex_);
    MysqlJob* mysql_job = task_queue_.front();
    task_queue_.pop();
    pthread_mutex_unlock(&mutex_);
    return mysql_job;
  }

  void pushJob(MysqlJob* job) {
    pthread_mutex_lock(&mutex_);
    task_queue_.push(job);
    pthread_mutex_unlock(&mutex_);
    sem_post(&task_num_);
  }

  sem_t task_num_;
  pthread_mutex_t mutex_;
  MYSQL_TASK_QUEUE task_queue_;
  MYSQL_ENGINE_POOL db_read_pool_;
  MYSQL_ENGINE_POOL db_write_pool_;
};

class MysqlJob {
 public:
  MysqlJob(MYSQL_JOB_TYPE type,
           base_logic::DictionaryValue* dict,
           const MYSQL_HANDLER& mysql_handler,
           void* param)
      : type_(type),
        dict_(dict),
        mysql_handler_(mysql_handler),
        param_(param) {

  }

  ~MysqlJob() {
    if (NULL != dict_) {
      delete dict_;
      dict_ = NULL;
    }
  }

  void Excute(MysqlThreadSharedInfo* shared_info) {
    bool r = false;
    base_storage::DBStorageEngine* engine = shared_info->popEngine(type_);
    if (MYSQL_READ == type_ || MYSQL_WRITE == type_) {
      GenerateSql();
    } else {
      dict_->GetString(MYSQL_STORAGE_NAME, &sql_);
    }
    LOG_DEBUG2("excute sql=%s", sql_.c_str());
    r = engine->SQLExec(sql_.c_str());
    if (!r) {
      LOG_ERROR("exec sql error");
    }
    ReadData(engine);
    shared_info->pushEngine(type_, engine);
    if (NULL != mysql_handler_) {
      mysql_handler_(dict_, param_);
    }
  }

  void ReadData(base_storage::DBStorageEngine* engine) {
    MYSQL_ROW rows;
    int32 num = engine->RecordCount();
    if (engine->RecordCount() > 0) {
      base_logic::ListValue* result_list = new base_logic::ListValue();
      ListValue* column_list = NULL;
      dict_->GetList(MYSQL_QUERY_PARAM, &column_list);
      if (NULL == column_list) {
        LOG_ERROR("mysql query not column name define");
        return;
      }
      while ((rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc))) {
        DictionaryValue* dict_record = new DictionaryValue();
        for (int i = 0; i < column_nums_; ++i) {
          std::string column_name;
          column_list->GetString(i, &column_name);
          if (NULL != rows[i]) {
            dict_record->SetString(column_name, rows[i]);
          }
        }
        result_list->Append((Value*) dict_record);
      }
      dict_->Set(MYSQL_RESULT_LIST, result_list);
    }
  }

  void GenerateSql() {
    bool r = false;
    std::stringstream ss;
    std::string query_type;
    r = dict_->GetString(MYSQL_QUERY_TYPE, &query_type);
    if (r) {
      ss << query_type << " ";
    }
    ListValue* column_list = NULL;
    dict_->GetList(MYSQL_QUERY_PARAM, &column_list);
    if (NULL != column_list) {
      column_nums_ = column_list->GetSize();
      ListValue::iterator iter(column_list->begin());
      int index = 0;
      for (; iter != column_list->end(); ++iter) {
        std::string temp_column_name;
        (StringValue*) (*iter)->GetAsString(&temp_column_name);
        ss << temp_column_name;
        if (++index < column_nums_) {
          ss << ", ";
        }
      }
      ss << " ";
    } else {
      LOG_MSG("query param empty");
    }
    std::string query_condition;
    r = dict_->GetString(MYSQL_QUERY_CONDITION, &query_condition);
    if (r) {
      ss << query_condition;
    }
    sql_ = ss.str();
  }

  MYSQL_JOB_TYPE type() const {
    return type_;
  }
  const std::string& sql() const {
    return sql_;
  }

 private:
  size_t column_nums_;
  std::string sql_;
  MYSQL_JOB_TYPE type_;
  base_logic::DictionaryValue* dict_;
  void* param_;
  MYSQL_HANDLER mysql_handler_;
};

class MySqlThread {
 public:
  static void* run(void* param) {
    MysqlThreadSharedInfo* shared_info =
        static_cast<MysqlThreadSharedInfo*>(param);
    assert(NULL != shared_info);
    for (;;) {
      sem_wait(&shared_info->task_num_);
      MysqlJob* mysql_job = shared_info->popJob();
      if (NULL != mysql_job) {
        mysql_job->Excute(shared_info);
      }
      delete mysql_job;
      mysql_job = NULL;
    }
    return NULL;
  }
};

} /* namespace */


class MysqlThreadPool {
 public:
  MysqlThreadPool(base::ConnAddr& read_addr,
                  base::ConnAddr& write_addr,
                  uint8 thread_num = 1);
  ~MysqlThreadPool();
 private:
  void Initialize();

  void CreateSqlThread();
  void CreateEnginePool();

 public:
  void QueryAsync(MYSQL_JOB_TYPE type,
                  base_logic::DictionaryValue* dict,
                  MYSQL_HANDLER call_back,
                  void* param);

  bool QuerySync(MYSQL_JOB_TYPE type,
                 base_logic::DictionaryValue* dict,
                 MYSQL_HANDLER call_back,
                 void* param);

 private:
  uint8 thread_num_;
  base::ConnAddr read_addr_;
  base::ConnAddr write_addr_;
  std::list<pthread_t> threads_list_;
  MysqlThreadSharedInfo shared_info_;
};

} /* namespace base_logic */

#endif //STRADE_SRC_PUB_STORAGE_MYSQL_THREAD_POOL_H
