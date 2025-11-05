/**
 * @file json_transformer.cpp
 * @brief JSON data transformation and processing tool
 *
 * This example demonstrates:
 * - Single JSON file processing
 * - Chained operator usage for complex queries
 * - Dynamic JSON modification with MutableValue
 * - Type-safe value extraction
 *
 * Features:
 * - Transform JSON structure
 * - Filter and process data
 * - Generate summary reports
 *
 * Usage:
 *   ./json_transformer --transform <file.json>  Transform JSON structure
 *   ./json_transformer --filter <file.json?path>  Filter by path
 *   ./json_transformer --summary <file.json>  Generate summary report
 *
 * Examples:
 *   ./json_transformer --transform data.json
 *   ./json_transformer --filter data.json?users[0]/name
 *   ./json_transformer --summary data.json
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <xyjson.h>

using namespace yyjson;

// Structure to hold transformation results
struct TransformStats {
    int files_processed = 0;
    int objects_created = 0;
    int fields_added = 0;

    void display() const {
        std::cout << "\n=== Transformation Statistics ===\n";
        std::cout << "Files processed: " << files_processed << "\n";
        std::cout << "Objects created: " << objects_created << "\n";
        std::cout << "Fields added: " << fields_added << "\n";
    }
};

// Example transformation: enrich user data
bool enrich_user_data(MutableValue& user, TransformStats& stats) {
    if (!user.isObject()) return false;

    // Add computed fields using operator overloading
    auto age = user / "age" | 0;

    // Add age group
    std::string age_group;
    if (age < 18) age_group = "minor";
    else if (age < 65) age_group = "adult";
    else age_group = "senior";

    user / "age_group" = age_group;
    stats.fields_added++;

    // Add timestamp
    user / "processed_at" = "2025-11-05T00:00:00Z";
    stats.fields_added++;

    stats.objects_created++;
    return true;
}

// Example transformation: clean and normalize data
bool normalize_data(MutableValue& data, TransformStats& stats) {
    if (!data.isObject()) return false;

    // Iterate over object properties
    auto iter = data.iterator("");
    while (iter.isValid()) {
        auto key_name = iter.key() | "";
        auto value = iter.value();

        // If string value is empty, mark for removal (simplified, no removal in this demo)
        if (value.isString()) {
            auto str = std::string(value | "");
            if (!str.empty()) {
                // Normalize string: uppercase first letter
                str[0] = std::toupper(str[0]);
                data / key_name = str;
            }
        }

        ++iter;
    }

    return true;
}

// Generate summary report
void generate_summary(const MutableDocument& doc) {
    auto root = *doc;

    std::cout << "\n=== JSON Summary Report ===\n";

    // Top-level structure
    std::cout << "Top-level type: " << (root.isObject() ? "Object" :
                                        root.isArray() ? "Array" :
                                        "Other") << "\n";

    if (root.isObject()) {
        std::cout << "Object contains " << root.size() << " keys\n";

        // Check for users array
        auto users = root / "users";
        if (users.isArray()) {
            std::cout << "Users count: " << users.size() << "\n";

            // Calculate average age if present
            int total_age = 0;
            int age_count = 0;
            for (size_t i = 0; i < std::min(users.size(), (size_t)10); ++i) {
                auto user = users / i;
                auto age = user / "age" | 0;
                if (age > 0) {
                    total_age += age;
                    age_count++;
                }
            }

            if (age_count > 0) {
                std::cout << "Average age (sample): "
                          << (total_age / (double)age_count) << "\n";
            }
        }
    } else if (root.isArray()) {
        std::cout << "Array size: " << root.size() << "\n";
    }
}

// Filter and display data by path
void filter_by_path(const MutableDocument& doc, const std::string& path) {
    auto root = *doc;
    auto result = root / path;

    if (!result.isValid()) {
        std::cerr << "Warning: Path not found: " << path << "\n";
        return;
    }

    std::cout << "\n=== Filtered Result (path: " << path << ") ===\n";
    // Display the value type
    std::cout << "Type: " << result.typeName() << "\n";

    // Print value based on type
    if (result.isString()) {
        std::cout << "Value: " << (result | "") << "\n";
    } else if (result.isNumber()) {
        std::cout << "Value: " << (result | 0) << "\n";
    } else if (result.isBool()) {
        std::cout << "Value: " << ((result | false) ? "true" : "false") << "\n";
    } else {
        std::cout << "Value: (complex type, use --summary for details)\n";
    }
}

void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << prog_name << " <command> [options]\n\n";
    std::cerr << "Commands:\n";
    std::cerr << "  --transform <file.json>                Transform JSON structure\n";
    std::cerr << "  --filter <file.json?path>              Filter by JSON path\n";
    std::cerr << "  --summary <file.json>                  Generate summary report\n\n";
    std::cerr << "Examples:\n";
    std::cerr << "  " << prog_name << " --transform data.json\n";
    std::cerr << "  " << prog_name << " --filter data.json?users[0]/name\n";
    std::cerr << "  " << prog_name << " --summary data.json\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string command = argv[1];

    try {
        if (command == "--transform") {
            // Transform single file
            if (argc < 3) {
                std::cerr << "Error: --transform requires a file\n";
                return 1;
            }

            // Read file
            std::string json_content;
            std::ifstream file(argv[2]);
            if (!file.is_open()) {
                std::cerr << "Error: Failed to read file: " << argv[2] << "\n";
                return 1;
            }
            json_content.assign((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());

            // Parse and transform using MutableDocument
            MutableDocument doc(json_content);
            if (doc.hasError()) {
                std::cerr << "Error: Invalid JSON format\n";
                return 1;
            }

            auto root = *doc;
            TransformStats stats;

            if (root.isObject()) {
                normalize_data(root, stats);
            }

            stats.display();
            std::cout << "\n=== Transformed JSON ===\n";
            std::string output;
            doc.write(output);
            std::cout << output << "\n";

        } else if (command == "--filter") {
            // Filter by path
            if (argc < 3) {
                std::cerr << "Error: --filter requires a file?path argument\n";
                return 1;
            }

            std::string arg = argv[2];
            size_t pos = arg.find('?');
            if (pos == std::string::npos) {
                std::cerr << "Error: --filter requires file?path format\n";
                return 1;
            }

            std::string filepath = arg.substr(0, pos);
            std::string path = arg.substr(pos + 1);

            // Read file
            std::string json_content;
            std::ifstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "Error: Failed to read file: " << filepath << "\n";
                return 1;
            }
            json_content.assign((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());

            MutableDocument doc(json_content);
            if (doc.hasError()) {
                std::cerr << "Error: Invalid JSON format\n";
                return 1;
            }

            filter_by_path(doc, path);

        } else if (command == "--summary") {
            // Generate summary
            if (argc < 3) {
                std::cerr << "Error: --summary requires a file\n";
                return 1;
            }

            // Read file
            std::string json_content;
            std::ifstream file(argv[2]);
            if (!file.is_open()) {
                std::cerr << "Error: Failed to read file: " << argv[2] << "\n";
                return 1;
            }
            json_content.assign((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());

            MutableDocument doc(json_content);
            if (doc.hasError()) {
                std::cerr << "Error: Invalid JSON format\n";
                return 1;
            }

            generate_summary(doc);

        } else {
            std::cerr << "Error: Unknown command: " << command << "\n";
            print_usage(argv[0]);
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
