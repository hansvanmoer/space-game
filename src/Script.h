/* 
 * File:   Script.h
 * Author: hans
 *
 * Created on October 14, 2016, 7:46 PM
 */

#ifndef GAME_SCRIPT_H
#define	GAME_SCRIPT_H

#include "Application.h"
#include "Log.h"

#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <mutex>
#include <memory>

#include <boost/python.hpp>

namespace Game {

    namespace Script {

        class ScriptError : public std::runtime_error{
        public:
            ScriptError(const std::string &message);
        };
        
        class ObjectError : public ScriptError{
        public:
            ObjectError(const std::string &message);
        };
        
        class NoSuchObjectError : public ObjectError{
        public:
            NoSuchObjectError(const std::string &id);
        };
        
        class DuplicateObjectError : public ObjectError{
        public:
            DuplicateObjectError(const std::string &id);
        };
        
        using Id = std::string;
        
        class NullHandleError : public ObjectError{
        public:
            NullHandleError();
        };
        
        template<typename Object> class BasicHandle{
        public:    
            BasicHandle();;
            
            BasicHandle(Object *object) : object_(object){};
            
            bool attached() const{
                return object_;
            };
            
            Object *object(){
                if(!object_){
                    throw NullHandleError{};
                }
                return object_;
            };
            
            const Object *object() const{
                if(!object_){
                    throw NullHandleError{};
                }
                return object;
            };
            
            operator bool() const{
                return attached();
            };
            
            bool operator!() const{
                return !attached();
            };
            
            Object *operator*(){
                return object();
            };
            
            const Object *operator*() const{
                return object();
            };
            
        private:
            Object *object_;
        };
        
        template<typename Object, typename Handle> struct BasicHandlePolicy{
        public:
            
            static Handle wrap(Object *object){
                return Handle{object};
            };
            
        };
        
        template<typename Object> struct BasicHandlePolicy<Object,Object>{
        public:
            
            static Object *wrap(Object *object){
                return object;
            };
            
        };
        
        template<typename Object_, typename Handle_ = Object_ *, typename HandlePolicy = BasicHandlePolicy<Object_,Handle_> > class ObjectManager {
        public:
            using Id = Script::Id;
            using Object = Object_;
            using Handle = Handle_;

            ObjectManager() : objects_() {
            };

            virtual ~ObjectManager() {
                for (auto entry : objects_) {
                    delete entry.second;
                }
            };

            bool exists(const Id &id) const{
                return objects_.find(id) != objects_.end();
            };
            
            Handle get(const Id &id) const{
                auto found = objects_.find(id);
                if(found == objects_.end()){
                    throw NoSuchObjectError{id};
                }else{
                    return HandlePolicy::wrap(found->second);
                }
            };
            
        protected:
            
            Handle add(const Id &id, Object *object){
                if(objects_.find(id) == objects_.end()){
                    objects_.emplace(id, object);
                    return HandlePolicy::wrap(object);
                }else{
                    throw DuplicateObjectError{id};
                }
            };
            
        private:
            std::unordered_map<Id, Object *> objects_;
        };
        
        class Writer{
        public:
            
            virtual ~Writer();
            
            virtual void write(const char *output) = 0;
            
            virtual void flush() = 0;
        protected:
            Writer();
        private:
            Writer(const Writer &) = delete;
            Writer &operator=(const Writer &) = delete;
        };
        
        class LoggerWriter : public Writer{
        public:
            
            LoggerWriter(const Log::Logger &logger, Log::Level level = Log::Level::debug);
            
            ~LoggerWriter();
            
            void write(const char *output);
            
            void flush();
            
        private:
            Log::LogLinesAdapter<std::string> logger_;
            std::ostringstream buffer_;
            bool dirty_;
        };
        
        class IgnoreWriter : public Writer{
        public:
            IgnoreWriter();
            
            ~IgnoreWriter();
            
            void write(const char *output);
            
            void flush();
        };
        
        class StandardWriter : public Writer{
        public:
            StandardWriter();

            StandardWriter(const StandardWriter &);
            
            StandardWriter &operator=(const StandardWriter &);
            
            ~StandardWriter();
            
            void write(const char *output);
            
            void flush();
            
        };
        
        class ErrorWriter : public Writer{
        public:
            ErrorWriter();
            
            ErrorWriter(const ErrorWriter &);
            
            ErrorWriter &operator=(const ErrorWriter &);
            
            ~ErrorWriter();
            
            void write(const char *output);
            
            void flush();
        };
        
        class ScriptSystem;
        
        class Runnable{
        public:
            
            Runnable();
            
            virtual ~Runnable();
            
            void run();
            
            void load(std::istream &input);
            
            void output(const std::shared_ptr<Writer> &writer);
            
            void standard_output(const std::shared_ptr<Writer> &writer);
            
            Writer &standard_output();
            
            void error_output(const std::shared_ptr<Writer> &writer);
            
            Writer &error_output();
            
        protected:
            
            virtual void before_initialize() = 0;
            
        private:
                        
            boost::python::str code_;
            std::shared_ptr<Writer> standard_output_;
            std::shared_ptr<Writer> error_output_;

            Runnable(const Runnable &) = delete;
            Runnable &operator=(const Runnable &) = delete;
            
            friend class ScriptSystem;
        };
        
        class ScriptSystem{
        public:
            static const ApplicationId id;
            
            ScriptSystem();
            
             ~ScriptSystem();
            
            void run(Runnable *script);
            
            bool try_run(Runnable *script);
            
            Runnable &current_script();
            
        private:
            Runnable *current_script_;
            std::mutex run_mutex_;
            
            void do_run(Runnable *script);
            
            ScriptSystem(const ScriptSystem &) = delete;
            ScriptSystem &operator=(const ScriptSystem &) = delete;
        };

    }
    
    using ScriptSystem = Script::ScriptSystem;

}

#endif	/* SCRIPT_H */

