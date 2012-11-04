
#include "SharedObject.h"

#ifndef _WIN32
#include <dlfcn.h>
#include <stdlib.h>
#include <pthread.h>
#endif


SharedObject::SharedObject(const char* fname) : _so_handle(0)
{ }


bool SharedObject::open(const char *fname)
{
#ifdef _WIN32
    _so_handle = LoadLibrary(fname);

    if (_so_handle == NULL)
    {
        _error = "SharedObject::open() LoadLibrary failed";
        return false;
    }

#else
    if (fname != 0)
    {
        _so_handle = dlopen(fname, RTLD_NOW | RTLD_GLOBAL);
        
        setupError();
     
        return _error.empty();
    }
    else
    {
        _so_handle = 0;
        _error.clear();
    }
#endif    
    return true;
}


void SharedObject::close()
{
#ifdef _WIN32
    if (_so_handle)
    {
        FreeLibrary(_so_handle);
        _so_handle = 0;
    }
#else
    if (_so_handle)
    {
        dlclose(_so_handle);
        _so_handle = 0;
    }
#endif
}


bool SharedObject::getSymbol(void** value, const char* keyname)
{
    // Get key-symbol/value pair from library

    if (value == 0) return false;
    
#ifdef _WIN32
    *value = (void *)GetProcAddress(_so_handle, keyname);
    if (*value != 0) return true;
#else
    if (_so_handle != 0)
    {
        *value = dlsym(_so_handle, keyname);
        setupError();
        if (_error.empty())
        {
            return true;
        }
    }
#endif
    return false;
}

void SharedObject::setupError()
{
    char *error = dlerror();
    
    if (error == 0) 
        _error.clear();
    else
        _error = error;
}


const std::string& SharedObject::lastError()
{
    return _error;
}

