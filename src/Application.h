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

#include "Type.h"

namespace Game {

    using ApplicationId = std::string;

    class ApplicationError : public std::runtime_error {
    public:
        ApplicationError(const ApplicationId &id, const std::string &message);
    };

    namespace ApplicationSystemPolicies {

        template<typename System > class HasShutDown {
            template<typename T, void (T::*)()> struct Test;

            template<typename T > Type::True test(Test<System, System::shutdown> *);

            template<typename T > Type::False test(...);
        public:
            static const bool result = sizeof (test(nullptr)) == sizeof (Type::True);
        };

        template<typename System> struct DestroyPolicy {

            static void execute(System *system) {
                delete system;
            };

        };

        template<typename System> struct ShutdownAndDestroyPolicy {

            static void execute(System *system) {
                try {
                    system->destroy();
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
                instance_ = new System{std::forward(args)...};
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

}

#endif	/* SINGLETON_H */

