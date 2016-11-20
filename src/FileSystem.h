/* 
 * File:   FileSystem.h
 * Author: hans
 *
 * Created on October 29, 2016, 3:41 PM
 */

#ifndef GAME_FILESYSTEM_H
#define	GAME_FILESYSTEM_H

#include "CLI.h"
#include "Application.h"

#include <string>

#include <boost/filesystem.hpp>

namespace Game{
    
    class FileSystem{
    public:
        
        static const ApplicationId id;
        
        FileSystem(const CLI::Call &call);
        
        boost::filesystem::path data_path() const;
        
        boost::filesystem::path modules_path() const;
        
        boost::filesystem::path settings_path() const;
        
    private:
        
        boost::filesystem::path data_path_;
        
        boost::filesystem::path modules_path_;
        
        boost::filesystem::path settings_path_;
    };
    
}

#endif	/* FILESYSTEM_H */

