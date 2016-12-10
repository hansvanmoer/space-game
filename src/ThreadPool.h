///
/// \file contains several helper classed related to concurrency
///

#ifndef GAME_THREAD_POOL_H
#define	GAME_THREAD_POOL_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <vector>
#include <utility>

namespace Game{
 
    ///
    /// \class A simple thread pool with a fixed (non static) amount of worker threads.
    /// All member functions of this class are thread safe.
    ///
    class FixedThreadPool{
    public:
        
        ///
        /// Creates a new thread pool with the specified amount of working threads
        /// Threads are created when start is called and destroyed before stop() or finish_and_stop() returns
        /// \param max_thread_count the amount of worker threads to use
        ///
        FixedThreadPool(std::size_t max_thread_count = 1);
        
        ///
        /// Stops and destroys the thread pool
        /// All scheduled and unfinished tasks are destroyed
        ///
        ~FixedThreadPool();
        
        ///
        /// Starts the thread pool.
        /// This action can only succeed when the pool is stopped
        /// \return true if the pool has been started, false if it could not be started (e.g. it was already running)
        ///
        bool start();
        
        ///
        /// Stops the thread pool
        /// This call will block until the pool has been stopped
        /// All unfinished tasks are scheduled to be run first when the pool is restarted
        /// \return true if the pool has been stopped, false if it could not be stopped (e.g. it was not running)
        ///
        bool stop();
        
        ///
        /// Finishes all current tasks and then stops the pool
        /// All subsequent calls to submit will schedule the tasks to be run the next time the pool starts
        /// This call will block until the pool has been stopped
        ///
        bool finish_and_stop();
        
        ///
        /// Checks if the pool is running
        /// This call may block
        /// \return true if the pool is currently running and accepting tasks, false otherwise
        ///
        bool running() const;
        
        ///
        /// Submit a new task
        /// If the pool is not running, the task will be scheduled to run after the pool starts
        /// \param task should be a callable object with no parameters or return type
        ///
        template<typename T> void submit(T &&task){
            do_submit(new TaskImpl<T>{std::forward<T>(task)});
        };
        
        ///
        /// Submit a new task
        /// If the pool is not running, the task will be scheduled to run after the pool starts
        /// \param task should be a callable object with no parameters or return type
        ///
        template<typename T> void submit(const T &task){
            do_submit(new TaskImpl<T>{task});
        };
        
        ///
        /// Clears all scheduled and active tasks
        /// This function may block
        ///
        void clear();
        
    private:
        
        enum class State{
            RUNNING, FINISHING, STOPPING, STOPPED
        };
        
        class Task{
        public:
            virtual ~Task();
            
            virtual void execute() = 0;
        };
        
        template<typename T> class TaskImpl : public Task{
        public:
            
            explicit TaskImpl(const T &task) : task_(task){};
            
            explicit TaskImpl(T &&task) : task_(std::move(task)){};
            
            ~TaskImpl(){};
            
            void execute() override{
                task_();
            };
            
        private:
            T task_;
        };
        
        std::vector<std::thread> threads_;
        const std::size_t max_thread_count_; 
        State state_;
        mutable std::mutex mutex_;
        std::condition_variable condition_;
        
        std::list<Task *> tasks_;
        std::list<Task *> scheduled_tasks_;
        
        void perform_tasks();
        
        Task *claim_task();
        
        void do_submit(Task *task);
        
        bool do_stop(State stopping_state);
        
        FixedThreadPool(const FixedThreadPool &) = delete;
        FixedThreadPool &operator=(const FixedThreadPool &) = delete;
    };
    
}

#endif	/* CONCURRENCY_H */

