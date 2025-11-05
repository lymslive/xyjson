/**
 * @file jp_jq.cpp
 * @brief Minimal jq-like JSON path query tool
 *
 * Usage:
 *   ./jp /path/to/file.json?/path/to/node
 *   ./jp -?/path/to/node    (read from stdin)
 *   ./jp /path/to/file.json (print entire file, formatted)
 *   ./jp -?                 (print stdin input, formatted)
 *
 * Examples:
 *   ./jp data.json?users[0]/name
 *   echo '{"a": {"b": 1}}' | ./jp -?/a/b
 */

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <xyjson.h>

using namespace yyjson;

void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << prog_name << " <file.json?path> | -?path\n";
    std::cerr << "\nOptions:\n";
    std::cerr << "  file.json?path   Query path in JSON file\n";
    std::cerr << "  -?path           Read JSON from stdin, then query path\n";
    std::cerr << "  file.json        Print entire JSON file (formatted)\n";
    std::cerr << "  -?               Print stdin input (formatted)\n";
    std::cerr << "\nExamples:\n";
    std::cerr << "  " << prog_name << " data.json?users[0]/name\n";
    std::cerr << "  echo '{\"a\": {\"b\": 1}}' | " << prog_name << " -?/a/b\n";
}

bool read_json_from_file(const char* filepath, std::string& json_content) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file '" << filepath << "'\n";
        return false;
    }

    json_content.assign((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return true;
}

bool read_json_from_stdin(std::string& json_content) {
    json_content.assign((std::istreambuf_iterator<char>(std::cin)),
                        std::istreambuf_iterator<char>());
    return true;
}

int main(int argc, char* argv[]) {
    // Parse arguments
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string arg = argv[1];
    std::string json_source;
    std::string query_path;

    bool from_stdin = false;

    // Check if argument starts with '-?' or '?'
    if (arg == "-?" || arg == "?") {
        from_stdin = true;
        json_source = "";
        query_path = "";
    } else if (arg.rfind("-?", 0) == 0 || arg.rfind("?", 0) == 0) {
        from_stdin = true;
        query_path = arg.substr(arg.find('?') + 1);
    } else if (arg.find('?') != std::string::npos) {
        // Split by '?'
        size_t pos = arg.find('?');
        json_source = arg.substr(0, pos);
        query_path = arg.substr(pos + 1);
    } else {
        // No '?' found, treat entire argument as file path
        json_source = arg;
        query_path = "";
    }

    // Read JSON content
    std::string json_content;
    if (from_stdin) {
        if (!read_json_from_stdin(json_content)) {
            std::cerr << "Error: Failed to read from stdin\n";
            return 1;
        }
    } else {
        if (!read_json_from_file(json_source.c_str(), json_content)) {
            return 1;
        }
    }

    // Parse JSON using MutableDocument for easier output
    MutableDocument doc(json_content);
    if (doc.hasError()) {
        std::cerr << "Error: Invalid JSON format\n";
        return 1;
    }

    // If no query path specified, print entire JSON formatted
    if (query_path.empty()) {
        std::string output;
        doc.write(output);
        std::cout << output << "\n";
        return 0;
    }

    // Query the path
    auto result = doc / query_path;

    if (!result.isValid()) {
        std::cerr << "Warning: Path not found: " << query_path << "\n";
        return 1;
    }

    std::cout << result.toString() << "\n";
    return 0;
}
