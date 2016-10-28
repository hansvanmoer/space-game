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

namespace Game {

    using ApplicationId = std::string;

    class ApplicationError : public std::runtime_error {
    public:
        ApplicationError(const ApplicationId &id, const std::string &message);
    };

    namespace ApplicationSystemPolicies {

        template<typename System> struct ConstructorPolicy {

            template<typename... Args> static System *execute(Args && ... args) {
                return new System{std::forward(args)...};
            };

        };

        template<typename System> struct DestructorPolicy {

            static void execute(System &system) {
            };
        };

        template<typename System> struct ShutdownPolicy {

            static void execute(System &system) {
                system.shutdown();
            };
        };

    }

    template<typename System, typename InitializePolicy = ApplicationSystemPolicies::ConstructorPolicy<System>, typename ShutdownPolicy = ApplicationSystemPolicies::DestructorPolicy<System> > class ApplicationSystem {
    private:
        static System *instance_;
    public:

        template<typename... Args> static System &initialize(Args && ... args) {
            if (instance_) {
                throw ApplicationError{System::id, "application system already started"};
            } else {
                instance_ = InitializePolicy::execute(std::forward(args)...);
                return *instance_;
            }
        };

        static void shutdown() {
            if (instance_) {
                ShutdownPolicy::execute(instance_);
                delete instance_;
            } else {
                throw ApplicationError{System::id, "application system not started"};
            }
        };

    };



}

#endif	/* SINGLETON_H */

