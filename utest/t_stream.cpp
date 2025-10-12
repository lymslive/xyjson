/**
 * @file t_stream.cpp
 * @author lymslive
 * @date 2025-10-12
 * @brief stream operations test for xyjson
 * */
#include "couttast/couttast.h"
#include "xyjson.h"

/* t_stream.cpp - 流操作测试
 * 包含：
 * - 文档流操作
 * - 文件流操作
 * - 标准输出流
 * - 文件操作
 */

DEF_TAST(stream_document_ops, "test Document stream operators << and >>")
{
    DESC("Test Document stream operators");
    {
        yyjson::Document doc;
        std::string jsonText = R"json({"name": "test", "value": 123})json";
        std::string output;
        
        // Read JSON using << operator
        doc << jsonText;
        COUT(doc.hasError(), false);
        
        // Extract values to verify
        std::string name = doc / "name" | "";
        int value = doc / "value" | 0;
        COUT(name, "test");
        COUT(value, 123);
        
        // Write JSON using >> operator
        doc >> output;
        COUT(output);
        COUT(output.empty(), false);
        COUT(output.find("\"name\":\"test\"") != std::string::npos, true);
        COUT(output.find("\"value\":123") != std::string::npos, true);
    }

    DESC("Test MutableDocument stream operators");
    {
        yyjson::MutableDocument doc;
        std::string jsonText = R"json({"name": "mutable", "value": 456})json";
        std::string output;

        // Read JSON using << operator
        doc << jsonText;
        COUT(doc.hasError(), false);

        // Extract values to verify
        std::string name = doc / "name" | "";
        int value = doc / "value" | 0;
        COUT(name, "mutable");
        COUT(value, 456);

        // Write JSON using >> operator
        doc >> output;
        COUT(output.empty(), false);
        COUT(output.find("\"name\":\"mutable\"") != std::string::npos, true);
        COUT(output.find("\"value\":456") != std::string::npos, true);
    }

    DESC("Test error handling with invalid JSON");
    {
        yyjson::Document doc;
        std::string invalidJson = "{invalid json}";
        
        doc << invalidJson;
        COUT(doc.hasError(), true);
        COUT(!doc, true);
        
        // Try to write invalid document
        std::string output;
        doc >> output;
        COUT(output.empty(), true);
    }
}

DEF_TAST(stream_file_ops, "test FILE* and std::fstream stream operations")
{
    DESC("Test FILE* stream operations");
    {
        // Create a temporary file
        const char* temp_filename = "/tmp/yyjson_test.json";
        
        // Write test data to file
        FILE* write_fp = fopen(temp_filename, "w");
        COUT(write_fp != nullptr, true);
        
        if (write_fp)
        {
            yyjson::Document doc;
            doc << R"json({"file_test": "FILE*", "number": 789})json";
            doc.write(write_fp);
            fclose(write_fp);
        }
        
        // Read from file using FILE*
        FILE* read_fp = fopen(temp_filename, "r");
        COUT(read_fp != nullptr, true);
        
        if (read_fp)
        {
            yyjson::Document doc;
            doc.read(read_fp);
            fclose(read_fp);
            
            COUT(doc.hasError(), false);
            COUT(doc / "file_test" | std::string(), "FILE*");
            COUT(doc / "number" | 0, 789);
        }
        
        // Clean up
        remove(temp_filename);
    }
    
    DESC("Test std::ifstream and std::ofstream operations");
    {
        const char* temp_filename = "/tmp/yyjson_stream_test.json";
        
        // Write to file using std::ofstream
        {
            std::ofstream ofs(temp_filename);
            COUT(ofs.is_open(), true);
            
            if (ofs.is_open())
            {
                yyjson::Document doc;
                doc << R"json({"stream_test": "std::fstream", "value": 999})json";
                doc.write(ofs);
                ofs.close();
            }
        }
        
        // Read from file using std::ifstream
        {
            std::ifstream ifs(temp_filename);
            COUT(ifs.is_open(), true);
            
            if (ifs.is_open())
            {
                yyjson::Document doc;
                doc.read(ifs);
                ifs.close();
                
                COUT(doc.hasError(), false);
                COUT(doc / "stream_test" | std::string(), "std::fstream");
                COUT(doc / "value" | 0, 999);
            }
        }
        
        // Test MutableDocument with file streams
        {
            std::ofstream ofs(temp_filename);
            COUT(ofs.is_open(), true);
            
            if (ofs.is_open())
            {
                yyjson::MutableDocument doc;
                doc << R"json({"mutable_stream": true, "count": 111})json";
                doc.write(ofs);
                ofs.close();
            }
        }
        
        {
            std::ifstream ifs(temp_filename);
            COUT(ifs.is_open(), true);
            
            if (ifs.is_open())
            {
                yyjson::MutableDocument doc;
                doc.read(ifs);
                ifs.close();
                
                COUT(doc.hasError(), false);
                COUT(doc / "mutable_stream" | false, true);
                COUT(doc / "count" | 0, 111);
            }
        }
        
        // Clean up
        remove(temp_filename);
    }
    
    DESC("Test stream operators with FILE* and std::fstream");
    {
        const char* temp_filename = "/tmp/yyjson_operator_test.json";
        
        // Test << operator with std::ifstream
        {
            // First create a test file
            std::ofstream ofs(temp_filename);
            ofs << R"json({"operator_test": "success", "id": 12345})json";
            ofs.close();
            
            std::ifstream ifs(temp_filename);
            yyjson::Document doc;
            doc << ifs; // Use << operator with std::ifstream
            
            COUT(doc.hasError(), false);
            COUT(doc / "operator_test" | std::string(), "success");
            COUT(doc / "id" | 0, 12345);
        }
        
        // Test >> operator with std::ofstream
        {
            yyjson::Document doc;
            doc << R"json({"output_test": "works", "number": 54321})json";
            
            std::ofstream ofs(temp_filename);
            doc >> ofs; // Use >> operator with std::ofstream
            ofs.close();
            
            // Verify file content
            std::ifstream ifs(temp_filename);
            std::string content((std::istreambuf_iterator<char>(ifs)), 
                               std::istreambuf_iterator<char>());
            
            COUT(content.find("\"output_test\":\"works\"") != std::string::npos, true);
            COUT(content.find("\"number\":54321") != std::string::npos, true);
        }
        
        // Clean up
        remove(temp_filename);
    }
}

DEF_TAST(stream_std_output, "test standard output stream operators <<")
{
    DESC("Test standard output stream operators <<");
    {
        // Test Document standard output stream - try COUT macro directly
        {
            yyjson::Document doc1("{\"message\": \"Hello World\", \"count\": 42}");
            yyjson::Document doc2("{\"message\": \"Hello World\", \"count\": 42}");
            COUT(doc1, doc2);

            std::string output;
            doc1 >> output;
            std::ostringstream oss;
            oss << doc2;
            COUT(oss.str(), output);
        }
        
        // Test Value standard output stream
        {
            yyjson::Document doc("{\"name\": \"Alice\", \"age\": 25}");
            yyjson::Value val1 = doc.root();
            yyjson::Value val2 = doc.root();
            COUT(val1, val2);
        }
        
        // Test MutableDocument standard output stream
        {
            yyjson::MutableDocument doc1("{\"status\": \"active\", \"level\": 3}");
            yyjson::MutableDocument doc2("{\"status\": \"active\", \"level\": 3}");
            
            COUT(doc1, doc2);
        }
        
        // Test MutableValue standard output stream
        {
            yyjson::MutableDocument doc("\"simple string\"");
            yyjson::MutableValue mval1 = doc.root();
            yyjson::MutableValue mval2 = doc.root();
            
            COUT(mval1, mval2);
        }
        
        // Test array values with standard output stream
        {
            yyjson::Document arrayDoc1("[1, 2, \"three\", true]");
            yyjson::Document arrayDoc2("[1, 2, \"three\", true]");
            
            COUT(arrayDoc1, arrayDoc2);
        }
        
        // Test with nested structures
        {
            yyjson::Document nestedDoc1("{\"data\": {\"items\": [1, 2, 3], \"info\": \"test\"}}");
            yyjson::Document nestedDoc2("{\"data\": {\"items\": [1, 2, 3], \"info\": \"test\"}}");
            
            COUT(nestedDoc1, nestedDoc2);
        }
        
        // Test primitive values
        {
            yyjson::Document numDoc1("123");
            yyjson::Document numDoc2("123");
            
            COUT(numDoc1, numDoc2);
            
            yyjson::Document boolDoc1("true");
            yyjson::Document boolDoc2("true");
            
            COUT(boolDoc1, boolDoc2);
        }
        
        // Test different values should not be equal
        {
            yyjson::Document doc1("{\"a\": 1}");
            yyjson::Document doc2("{\"a\": 2}");
            
            // This should fail if COUT macro works correctly with inequality
            COUT(doc1 != doc2, true);
        }
    }
}

DEF_TAST(stream_file_operations, "test file read/write operations for Document and MutableDocument")
{
    // Create a test JSON content
    std::string testJson = R"json({
        "name": "test",
        "value": 42,
        "active": true,
        "items": [1, 2, 3],
        "settings": {
            "enabled": false,
            "threshold": 5.5
        }
    })json";
    
    const char* testFilePath = "/tmp/test_json_file.json";
    const char* testFileMutPath = "/tmp/test_json_file_mut.json";
    
    DESC("Test Document file operations");
    {
        // Create original document from string
        yyjson::Document originalDoc(testJson);
        COUT(originalDoc.hasError(), false);
        
        // Write to file
        COUT(originalDoc.writeFile(testFilePath), true);
        
        // Read from file
        yyjson::Document readDoc;
        COUT(readDoc.readFile(testFilePath), true);
        COUT(readDoc.hasError(), false);
        
        
        // Verify content matches
        COUT(readDoc / "name" | std::string(), "test");
        COUT(readDoc / "value" | 0, 42);
        COUT(readDoc / "active" | false, true);
        COUT(readDoc / "items" / 0 | 0, 1);
        COUT(readDoc / "settings" / "threshold" | 0.0, 5.5);
        
        // Test write to string for comparison
        std::string originalStr, readStr;
        originalDoc.write(originalStr);
        readDoc.write(readStr);
        COUT(originalStr == readStr, true);
    }
    
    DESC("Test MutableDocument file operations");
    {
        // Create original mutable document from string
        yyjson::MutableDocument originalMutDoc(testJson);
        COUT(originalMutDoc.hasError(), false);
        
        // Modify some values
        originalMutDoc / "value" = 100;
        originalMutDoc / "active" = false;
        originalMutDoc / "settings" / "enabled" = true;
        originalMutDoc / "items" << 4;
        
        // Write to file
        COUT(originalMutDoc.writeFile(testFileMutPath), true);
        
        // Read from file
        yyjson::MutableDocument readMutDoc;
        COUT(readMutDoc.readFile(testFileMutPath), true);
        COUT(readMutDoc.hasError(), false);
        
        // Verify modified content
        COUT(readMutDoc / "value" | 0, 100);
        COUT(readMutDoc / "active" | true, false);
        COUT(readMutDoc / "settings" / "enabled" | false, true);
        COUT(readMutDoc / "items" / 3 | 0, 4);
        COUT((readMutDoc / "items").size(), 4);
        
        // Test write to string for comparison
        std::string originalStr, readStr;
        originalMutDoc.write(originalStr);
        readMutDoc.write(readStr);
        COUT(originalStr == readStr, true);
    }
    
    DESC("Test error handling for file operations");
    {
        yyjson::Document doc;
        yyjson::MutableDocument mutDoc;
        
        // Test reading non-existent file
        COUT(doc.readFile("/non/existent/path.json"), false);
        COUT(mutDoc.readFile("/non/existent/path.json"), false);
        
        // Test writing to invalid path
        COUT(doc.writeFile("/invalid/path/"), false);
        COUT(mutDoc.writeFile("/invalid/path/"), false);
        
        // Test with nullptr paths
        COUT(doc.readFile(nullptr), false);
        COUT(mutDoc.readFile(nullptr), false);
        COUT(doc.writeFile(nullptr), false);
        COUT(mutDoc.writeFile(nullptr), false);
    }
    
    // Clean up test files
    remove(testFilePath);
    remove(testFileMutPath);
}
