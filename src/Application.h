///
/// \file contains definitions related to the application's structure and subsystems
///

#ifndef GAME_APPLICATION_H
#define	GAME_APPLICATION_H

#include <stdexcept>
#include <string>
#include <type_traits>
#include <iostream>

#include "Type.h"

namespace Game {

    ///
    /// \typedef a type representing the id of a subsystem
    ///
    using ApplicationId = std::string;

    ///
    /// \class a basic error to be thrown when a problem occurs initializing or destroying an application
    ///
    class ApplicationError : public std::runtime_error {
    public:
        
        ///
        /// Creates a new application error
        /// \param id the id of the subsystem in whose lifecycle the error occurred
        /// \param message the message for this error
        ///
        ApplicationError(const ApplicationId &id, const std::string &message);
    
        ///
        /// Returns the id of the subsystem that raised the error
        /// \return the id of the subsystem that raised the error
        ///
        ApplicationId id() const;
        
    private:
        ApplicationId id_;
    };

    namespace ApplicationSystemPolicies {

        ///
        /// \class A traits type that checks whether a type has a non constant, non returning shutdown member function
        ///
        template<typename System > class HasShutDown {
            
            // SFINAE helper type
            template<typename T, void (T::*)()> struct Test;

            // SFINAE helper method
            template<typename T> static Type::True test(Test<T, &T::shutdown> *);

            // SFINAE helper method
            template<typename T> static Type::False test(...);
        public:
            
            ///
            /// True if the type has a non constant, non returning shutdown member function, false otherwise
            ///
            static const bool result = sizeof (test<System>(nullptr)) == sizeof (Type::True);
        };

        ///
        /// \class This policy destroys a subsystem without calling any method. Use this policy when a subsystem's shutdown can't throw errors
        ///
        template<typename System> struct DestroyPolicy {

            ///
            /// Destroys the subsystem
            /// \param system the subsystem
            ///
            static void execute(System *system) {
                delete system;
            };

        };

        ///
        /// \class This policy destroys a subsystem after calling the shutdown method. Use this policy when a subsystem's shutdown can throw errors
        ///
        template<typename System> struct ShutdownAndDestroyPolicy {

            ///
            /// Shuts the subsystem down and destroys it
            /// If an error is thrown in the shutdown method it is rethrown after the subsystem is destroyed
            /// \param system the subsystem
            ///
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

    ///
    /// \class A singleton that provides access to a subsystem. Use this class to avoid having to declare global variables.
    ///
    template<typename System> class ApplicationSystem {
    private:
        static System *instance_;

        using ShutDownPolicy = typename std::conditional<ApplicationSystemPolicies::HasShutDown<System>::result, ApplicationSystemPolicies::ShutdownAndDestroyPolicy<System>, ApplicationSystemPolicies::DestroyPolicy<System>>::type;

    public:

        ///
        /// Initializes the subsystem by calling the constructor with the specified arguments
        /// Note: this call is not thread safe
        /// \param args the subsystem constructor arguments
        /// \return a reference to the subsystem
        /// \throw ApplicationError if the subsystem was already initialized
        ///
        template<typename... Args> static System &initialize(Args && ... args) {
            if (instance_) {
                throw ApplicationError{System::id, "application system already started"};
            } else {
                instance_ = new System{std::forward<Args...>(args)...};
                return *instance_;
            }
        };

        ///
        /// Destroys this subsystem.
        /// If the subsystem has a non constant, non returning shutdown member function it will be called before the destructor is called.
        /// Note: this call is no thread safe
        ///
        static void shutdown() {
            if (instance_) {
                ShutDownPolicy::execute(instance_);
            } else {
                throw ApplicationError{System::id, "application system not started"};
            }
        };

    };
    
    template<typename System> System *ApplicationSystem<System>::instance_;

    ///
    /// \class A guard for a subsystem's initialization / shutdown
    ///
    template<typename System> struct ApplicationSystemGuard{
        
        ///
        /// Initializes the subsystem
        /// \param args the parameters that need to be forwarded to the subsystem's constructor
        ///
        template<typename... Args> ApplicationSystemGuard(Args &&... args){
            ApplicationSystem<System>::initialize(std::forward<Args...>(args)...);
        };
        
        ///
        /// Destroys the subsystem and prints out possible errors
        ///
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

