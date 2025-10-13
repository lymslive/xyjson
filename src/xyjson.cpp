#include "xyjson.h"

#include <cstring>
#include <fstream>

namespace yyjson {

/**************************************************************/
// Path operations implementation for Value

Value Value::pathex(const char* path) const
{
    if (!m_val || !path || !*path) return Value(nullptr);
    
    yyjson_val* current = m_val;
    const char* ptr = path;
    
    while (*ptr)
    {
        if (*ptr == '/')
        {
            ptr++;
            continue;
        }
        
        if (yyjson_is_obj(current))
        {
            const char* key_start = ptr;
            while (*ptr && *ptr != '/') ptr++;
            
            current = yyjson_obj_getn(current, key_start, ptr - key_start);
            if (!current) return Value(nullptr);
        }
        else if (yyjson_is_arr(current))
        {
            if (*ptr >= '0' && *ptr <= '9')
            {
                size_t index = 0;
                while (*ptr >= '0' && *ptr <= '9')
                {
                    index = index * 10 + (*ptr - '0');
                    ptr++;
                }
                current = yyjson_arr_get(current, index);
                if (!current) return Value(nullptr);
            }
            else
            {
                return Value(nullptr);
            }
        }
        else
        {
            return Value(nullptr);
        }
    }
    
    return Value(current);
}

int Value::toNumberCast() const
{
    if (isString()) {
        const char* str = yyjson_get_str(m_val);
        if (!str) return 0;

        try {
            return std::stoi(str);
        } catch (...) {
            return 0;
        }
    }

    if (isInt())
    {
        return getor(0);
    }
    if (isBool())
    {
        return getor(false);
    }
    if (isSint())
    {
        return getor(int64_t(0));
    }
    if (isUint())
    {
        return getor(uint64_t(0));
    }
    if (isReal())
    {
        return getor(0.0);
    }

    return 0;
}

/**************************************************************/
// Path operator implementation for MutableValue

MutableValue MutableValue::pathex(const char* path) const
{
    if (!m_val || !path || !*path) return MutableValue(nullptr, m_doc);
    
    yyjson_mut_val* current = m_val;
    const char* ptr = path;
    
    while (*ptr)
    {
        if (*ptr == '/')
        {
            ptr++;
            continue;
        }
        
        if (yyjson_mut_is_obj(current))
        {
            const char* key_start = ptr;
            while (*ptr && *ptr != '/') ptr++;
            
            current = yyjson_mut_obj_getn(current, key_start, ptr - key_start);
            if (!current) return MutableValue(nullptr, m_doc);
        }
        else if (yyjson_mut_is_arr(current))
        {
            if (*ptr >= '0' && *ptr <= '9')
            {
                size_t index = 0;
                while (*ptr >= '0' && *ptr <= '9')
                {
                    index = index * 10 + (*ptr - '0');
                    ptr++;
                }
                current = yyjson_mut_arr_get(current, index);
                if (!current) return MutableValue(nullptr, m_doc);
            }
            else
            {
                return MutableValue(nullptr, m_doc);
            }
        }
        else
        {
            return MutableValue(nullptr, m_doc);
        }
    }
    
    return MutableValue(current, m_doc);
}

int MutableValue::toNumberCast() const
{
    if (isString()) {
        const char* str = yyjson_mut_get_str(m_val);
        if (!str) return 0;

        try {
            return std::stoi(str);
        } catch (...) {
            return 0;
        }
    }

    if (isInt())
    {
        return getor(0);
    }
    if (isBool())
    {
        return getor(false);
    }
    if (isSint())
    {
        return getor(int64_t(0));
    }
    if (isUint())
    {
        return getor(uint64_t(0));
    }
    if (isReal())
    {
        return getor(0.0);
    }

    return 0;
}


} // namespace yyjson
