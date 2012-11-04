
#ifndef _SHARED_OBJECT_H_
#define _SHARED_OBJECT_H_

#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

typedef void*(*factory_func_t)(const char *options);

class SharedObject
{
public:
    SharedObject(const char *fname = 0);

    bool open(const char *fname);
    void close();

    bool getSymbol(void** value, const char* keyname);

    template<typename T>
    T* createObject(const char *symbol_name = 0, const char *init_options = 0)
    {

        if (_so_handle == 0) return 0;

        factory_func_t init_func;

        if (symbol_name == 0)
        {
            symbol_name = "initialize";
        }

        void *fp;

        if (!getSymbol(&fp, symbol_name)) return (T *)0;

        init_func = (factory_func_t)fp;

        return static_cast<T *>(init_func(init_options));
    }

    const std::string& lastError();

    static const char* sharedObjectExtension()
    {
#if defined(__APPLE__)
        return "dylib";
#elif defined(_WIN32)
        return "dll";
#else
        return "so";
#endif
    }
    
protected:
    void setupError();

#ifdef _WIN32
    HMODULE     _dll_handle;
#else
    void        *_so_handle;
#endif
    std::string  _error;
};


#endif // _SHARED_OBJECT_H_
