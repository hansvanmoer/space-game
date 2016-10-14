/* 
 * File:   name.h
 * Author: hans
 *
 * Created on October 14, 2016, 7:28 PM
 */

#ifndef GAME_NAME_H
#define	GAME_NAME_H

#include <string>
#include <iostream>
#include <vector>

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
        virtual bool has_more() = 0;
        
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
        ///
        BufferedStringPool();
        
        ///
        /// Adds the specified value to the string pool
        /// \param value a pointer to a utf-8 encoded char array
        /// \param the numbers of chars in the char array
        ///
        void add(const char *value, std::size_t length);
        
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
        /// \return the next string from the pool
        ///
        std::string next();
        
        ///
        /// \return false if all names are used exactly once after the last call to reset(), true otherwise
        ///
        bool has_more();
        
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

    private:
        std::vector<std::string> pool_;
        std::vector<std::string>::iterator next_;
    };
    
}

#endif	/* NAME_H */

