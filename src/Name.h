/* 
 * File:   name.h
 * Author: hans
 *
 * Created on October 14, 2016, 7:28 PM
 */

#ifndef GAME_NAME_H
#define	GAME_NAME_H

#include "Script.h"
#include "Resource.h"

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace Game{
    
    ///
    /// \class Specifies an interface for name generation
    ///
    template<typename Entity> class NameGenerator{
    public:
        
        ///
        /// Destroys this name generator
        ///
        virtual ~NameGenerator();

        ///
        /// \param entity the entity to name
        /// \return the next name in utf-8 encoding generated from this generator
        ///
        virtual std::string next_name(const Entity &entity) = 0;
        
    protected:
        
        ///
        /// Constructs a new name generator
        ///
        NameGenerator();
        
    private:
        NameGenerator(const NameGenerator &) = delete;
        NameGenerator &operator=(const NameGenerator &) = delete;
    };
    
    ///
    /// \class an error type for all errors related to a StringPool
    ///
    class StringPoolError : public std::runtime_error{
    public:
        
        ///
        /// Creates a new StringPool
        /// \param message the message for this error
        ///
        StringPoolError(const std::string &message);
    };
    
    ///
    /// \class this type represents a pool of strings to be used in the construction of names
    ///
    class StringPool{
    public:
        ///
        /// Destroys this string pool
        ///
        virtual ~StringPool();
        
        ///
        //  When this method returns false, reset() will be called
        /// \return true if this pool can generate more strings, false otherwise
        ///
        virtual bool has_more() const = 0;
        
        ///
        /// Rests this string pool
        ///
        virtual void reset() = 0;
        
        ///
        /// Returns the next name in the pool
        /// Names should only recur after reset() was called
        /// The order of names is undefined and may change after subsequent calls to reset()
        /// \return the next name in the pool
        ///
        virtual std::string next() = 0;
        
        ///
        /// Returns the next name in the pool but does not discard it
        /// \return the next name in the pool
        ///
        virtual std::string peek() = 0;
        
    protected:
        
        ///
        /// Creates a new string pool
        ///
        StringPool();
    };
    
    ///
    /// \class this type represents a string pool containing a finite set of values in memory
    ///
    class BufferedStringPool : public StringPool{
    public:
                
        ///
        /// Creates a new buffered string pool
        /// \param randomized when set to true the strings in this pool are shuffled randomly whenever the pool is exhausted and before the next call to next() or peek()
        /// \param exhaustible when set to true has_more() will return true if the pool is exhausted, otherwise has_more() will always return true
        ///
        BufferedStringPool(bool randomized = true, bool exhaustible = false);
        
        ///
        /// Adds the specified value to the string pool
        /// \param value an utf-8 encoded string
        ///
        void add(const std::string &value);
        
        ///
        /// Destroys this string pool, deallocates the space used by the values
        ///
        virtual ~BufferedStringPool();
        
        ///
        /// Discards and returns the next string from the pool
        /// The string will not be returned again until reset() was called
        /// \return the next string from the pool
        ///
        std::string next();
        
        ///
        /// Returns the next string from the pool without discarding it
        /// \return the next string from the pool
        ///
        std::string peek();
        
        ///
        /// \return false if all names are used exactly once after the last call to reset(), true otherwise
        ///
        bool has_more() const;
        
        ///
        /// Shuffles the values
        ///
        void reset();
        
        ///
        /// Loads the values for this pool from the specified input stream
        /// The format of the file is as follows:
        /// Each line contains one name or a comment
        /// A comment is a line beginning with '#' and is ignored
        /// A name is any other line.
        /// Leading and trailing whitespace characters (as defined by std::is_space) is ignored
        ///
        /// \param input the input stream
        /// \return the number of names added
        ///
        std::size_t load(std::istream &input);
        
        ///
        /// Loads the values for this pool through the resource system
        /// \param id the id of the resource
        /// \return the number of names added
        ///
        void load_from_file(const ResourceId &id);
        
    private:
        std::vector<std::string> pool_;
        std::vector<std::string>::iterator next_;
        bool randomized_;
        bool exhaustible_;
    };
    
    namespace Script{
        
        class NameScript : public Runnable{
        protected:
            void before_initialize();
        };
    }
}

#endif	/* NAME_H */

