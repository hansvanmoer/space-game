#include "ThreadPool.h"

#include <vector>
#include <chrono>
#include <list>
#include <algorithm>

using namespace Game;
using namespace std;

FixedThreadPool::Task::~Task(){}

FixedThreadPool::FixedThreadPool(size_t max_thread_count) : threads_(max_thread_count), max_thread_count_(max_thread_count), state_(State::STOPPED), mutex_(), condition_(), tasks_(), scheduled_tasks_(){
}

FixedThreadPool::~FixedThreadPool(){
    stop();
    for(Task *task : scheduled_tasks_){
        delete task;
    }
}

bool FixedThreadPool::start() {
    lock_guard<mutex> guard{mutex_};
    if(state_ == State::STOPPED){
        
        copy(scheduled_tasks_.begin(), scheduled_tasks_.end(), back_inserter(tasks_));
        scheduled_tasks_.clear();
        
        for(size_t i = 0; i < max_thread_count_; ++i){
            threads_.emplace_back(&FixedThreadPool::perform_tasks, this);
        }
        state_ = State::RUNNING;
        return true;
    }else{
        return false;
    }
}

bool FixedThreadPool::running() const{
    lock_guard<mutex> guard{mutex_};
    return state_ == State::RUNNING;
}

FixedThreadPool::Task* FixedThreadPool::claim_task() {
    unique_lock<mutex> lock{mutex_};
    while(true){
        if(state_ == State::RUNNING){
            if(!tasks_.empty()){
                Task *task = tasks_.front();
                tasks_.pop_front();
                return task;
            }
            condition_.wait(lock);
        }else if(state_ == State::FINISHING){
            if(tasks_.empty()){
                return nullptr;
            }else{
                Task *task = tasks_.front();
                tasks_.pop_front();
                return task;
            }
        }else{
            return nullptr;
        }
    }
}


void FixedThreadPool::perform_tasks(){
    Task *task;
    while((task = claim_task())){
        try{
            task->execute();
            delete task;
        }catch(...){
            delete task;
            throw;
        }
    }
}


bool FixedThreadPool::stop(){
    return do_stop(State::STOPPING);
}

bool FixedThreadPool::finish_and_stop(){
    return do_stop(State::FINISHING);
}

bool FixedThreadPool::do_stop(State stopping_state) {
    unique_lock<mutex> lock{mutex_};
    if(state_ == State::RUNNING){        
        thread waiting_thread{[&](){
            for(auto i = threads_.begin(); i != threads_.end(); ++i){
                if(i->joinable()){
                    i->join();
                }
            }
        }};
        
        state_ = stopping_state;
        condition_.notify_all();
        lock.unlock();
        
        waiting_thread.join();

        threads_.clear();
        lock.lock();
        
        copy(tasks_.begin(), tasks_.end(), front_inserter(scheduled_tasks_));
        tasks_.clear();
        
        state_ = State::STOPPED;
        return true;
    }else{
        return false;
    }
}

void FixedThreadPool::do_submit(Task* task){
    lock_guard<mutex> guard{mutex_};
    if(state_ == State::RUNNING){
        tasks_.push_back(task);
        condition_.notify_one();
    }else{
        scheduled_tasks_.push_back(task);
    }
};

void FixedThreadPool::clear(){
    lock_guard<mutex> guard{mutex_};
    for(Task *task : tasks_){
        delete task;
    }
    tasks_.clear();
};
