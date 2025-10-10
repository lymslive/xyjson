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

/**************************************************************/
// Document read/write methods implementation

bool Document::read(const char* str, size_t len)
{
    // Free existing document first
    free();
    
    if (len == 0 && str) len = strlen(str);
    m_doc = yyjson_read(str, len, 0);
    syncRoot();
    return isValid();
}

bool Document::read(FILE* fp)
{
    // Free existing document first
    free();
    
    if (!fp) return false;
    m_doc = yyjson_read_fp(fp, 0, nullptr, nullptr);
    syncRoot();
    return isValid();
}

bool Document::read(std::ifstream& ifs)
{
    // Read entire file content into string and use existing read method
    ifs.seekg(0, std::ios::end);
    size_t size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    
    if (size == 0) return false;
    
    std::string content(size, '\0');
    ifs.read(&content[0], size);
    
    return read(content);
}

bool Document::readFile(const char* path)
{
    if (!path) return false;
    FILE* fp = fopen(path, "rb");
    if (!fp) return false;

    bool result = read(fp);
    fclose(fp);
    return result;
}

bool Document::write(std::string& output) const
{
    if (!m_doc) return false;
    char* json = yyjson_write(m_doc, 0, nullptr);
    if (!json) return false;
    output = json;
    std::free(json);
    return true;
}

bool Document::write(FILE* fp) const
{
    if (!m_doc || !fp) return false;
    return yyjson_write_fp(fp, m_doc, 0, nullptr, nullptr);
}

bool Document::write(std::ofstream& ofs) const
{
    // Write to string first, then to file stream
    std::string content;
    if (!write(content)) return false;
    
    ofs.seekp(0, std::ios::beg);
    ofs.write(content.c_str(), content.size());
    ofs.flush();
    
    return !ofs.fail();
}

bool Document::writeFile(const char* path) const
{
    if (!path) return false;
    FILE* fp = fopen(path, "wb");
    if (!fp) return false;

    bool result = write(fp);
    fclose(fp);
    return result;
}

/**************************************************************/
// MutableDocument read/write methods implementation

bool MutableDocument::read(const char* str, size_t len)
{
    // Free existing document first
    free();
    
    if (len == 0 && str) len = strlen(str);
    yyjson_doc* doc = yyjson_read(str, len, 0);
    if (doc != nullptr)
    {
        m_doc = yyjson_doc_mut_copy(doc, nullptr);
        yyjson_doc_free(doc);
    }
    
    syncRoot();
    return isValid();
}

bool MutableDocument::read(FILE* fp)
{
    // Free existing document first
    free();
    
    if (!fp) return false;
    yyjson_doc* doc = yyjson_read_fp(fp, 0, nullptr, nullptr);
    if (doc != nullptr)
    {
        m_doc = yyjson_doc_mut_copy(doc, nullptr);
        yyjson_doc_free(doc);
    }
    
    syncRoot();
    return isValid();
}

bool MutableDocument::read(std::ifstream& ifs)
{
    // Read entire file content into string and use existing read method
    ifs.seekg(0, std::ios::end);
    size_t size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    
    if (size == 0) return false;
    
    std::string content(size, '\0');
    ifs.read(&content[0], size);
    
    return read(content);
}

bool MutableDocument::readFile(const char* path)
{
    if (!path) return false;
    FILE* fp = fopen(path, "rb");
    if (!fp) return false;

    bool result = read(fp);
    fclose(fp);
    return result;
}

bool MutableDocument::write(std::string& output) const
{
    if (!m_doc) return false;
    char* json = yyjson_mut_write(m_doc, 0, nullptr);
    if (!json) return false;
    output = json;
    std::free(json);
    return true;
}

bool MutableDocument::write(FILE* fp) const
{
    if (!m_doc || !fp) return false;
    return yyjson_mut_write_fp(fp, m_doc, 0, nullptr, nullptr);
}

bool MutableDocument::write(std::ofstream& ofs) const
{
    // Write to string first, then to file stream
    std::string content;
    if (!write(content)) return false;
    
    ofs.seekp(0, std::ios::beg);
    ofs.write(content.c_str(), content.size());
    ofs.flush();
    
    return !ofs.fail();
}

bool MutableDocument::writeFile(const char* path) const
{
    if (!path) return false;
    FILE* fp = fopen(path, "wb");
    if (!fp) return false;
    
    bool result = write(fp);
    fclose(fp);
    return result;
}

} // namespace yyjson
