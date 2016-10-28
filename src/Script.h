/* 
 * File:   Script.h
 * Author: hans
 *
 * Created on October 14, 2016, 7:46 PM
 */

#ifndef GAME_SCRIPT_H
#define	GAME_SCRIPT_H

#include <unordered_map>
#include <stdexcept>

#include "Application.h"

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
            BasicHandle() : object_(){};
            
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

    }

}

#endif	/* SCRIPT_H */

