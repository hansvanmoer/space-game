/* 
 * File:   Singleton.h
 * Author: hans
 *
 * Created on October 25, 2016, 8:59 PM
 */

#ifndef GAME_APPLICATION_H
#define	GAME_APPLICATION_H

#include <stdexcept>
#include <string>
#include <type_traits>
#include <iostream>

#include "Type.h"

namespace Game {

    using ApplicationId = std::string;

    class ApplicationError : public std::runtime_error {
    public:
        ApplicationError(const ApplicationId &id, const std::string &message);
    
        ApplicationId id() const;
        
    private:
        ApplicationId id_;
    };

    namespace ApplicationSystemPolicies {

        template<typename System > class HasShutDown {
            template<typename T, void (T::*)()> struct Test;

            template<typename T> static Type::True test(Test<T, &T::shutdown> *);

            template<typename T> static Type::False test(...);
        public:
            static const bool result = sizeof (test<System>(nullptr)) == sizeof (Type::True);
        };

        template<typename System> struct DestroyPolicy {

            static void execute(System *system) {
                delete system;
            };

        };

        template<typename System> struct ShutdownAndDestroyPolicy {

            static void execute(System *system) {
                try {
                    system->shutdown();
                    delete system;
                } catch (...) {
                    delete system;
                    throw;
                }
            };

        };


    };

    template<typename System> class ApplicationSystem {
    private:
        static System *instance_;

        using ShutDownPolicy = typename std::conditional<ApplicationSystemPolicies::HasShutDown<System>::result, ApplicationSystemPolicies::ShutdownAndDestroyPolicy<System>, ApplicationSystemPolicies::DestroyPolicy<System>>::type;

    public:

        template<typename... Args> static System &initialize(Args && ... args) {
            if (instance_) {
                throw ApplicationError{System::id, "application system already started"};
            } else {
                instance_ = new System{std::forward<Args...>(args)...};
                return *instance_;
            }
        };

        static void shutdown() {
            if (instance_) {
                ShutDownPolicy::execute(instance_);
            } else {
                throw ApplicationError{System::id, "application system not started"};
            }
        };

    };
    
    template<typename System> System *ApplicationSystem<System>::instance_;

    template<typename System> struct ApplicationSystemGuard{
        
        template<typename... Args> ApplicationSystemGuard(Args &&... args){
            ApplicationSystem<System>::initialize(std::forward<Args...>(args)...);
        };
        
        ~ApplicationSystemGuard(){
            using namespace std;
            try{
                ApplicationSystem<System>::shutdown();
            }catch(std::exception &e){
                cout << "unable to shutdown application system '" << System::id << "': " << e.what() << endl;
            }catch(...){
                cout << "unable to shutdown application system '" << System::id << "': unknown error" << endl;
            }
        };
        
    };
    
}

#endif	/* SINGLETON_H */

