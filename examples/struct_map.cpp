/**
 * @file struct_map.cpp
 * @brief C++ struct to JSON mapping demonstration
 *
 * This example demonstrates:
 * - Define a struct with various data types
 * - Deserialize from JSON using operator overloading
 * - Serialize to JSON using operator overloading
 * - Simple business logic processing
 *
 * Usage:
 *   ./struct_map              (use default JSON)
 *   ./struct_map <file.json>  (read from file)
 *   cat file.json | ./struct_map (read from stdin)
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <xyjson.h>

using namespace yyjson;

// Define a struct representing user information
struct User {
    int id;
    std::string name;
    std::string email;
    int age;
    bool is_active;
    double score;
    std::vector<std::string> tags;
    std::unique_ptr<std::string> note;

    // Default constructor
    User() : id(0), age(0), is_active(false), score(0.0), note(nullptr) {}

    // Deserialize from JSON using MutableDocument
    bool fromJson(const MutableDocument& doc) {
        try {
            id = doc / "id" | 0;
            name = doc / "name" | "";
            email = doc / "email" | "";
            age = doc / "age" | 0;
            is_active = doc / "is_active" | false;
            score = doc / "score" | 0.0;

            // Handle tags array
            auto tags_val = doc / "tags";
            if (tags_val.isArray()) {
                tags.clear();
                for (size_t i = 0; i < tags_val.size(); ++i) {
                    tags.push_back(tags_val[i] | "");
                }
            }

            // Handle optional note field
            auto note_val = doc / "note";
            if (note_val.isString()) {
                note = std::make_unique<std::string>(note_val | "");
            } else {
                note = nullptr;
            }

            return true;
        } catch (...) {
            return false;
        }
    }

    // Serialize to JSON using MutableValue operations
    bool toJson(MutableValue& json) const {
        json["id"] = id;
        json["name"] = name;
        json["email"] = email;
        json["age"] = age;
        json["is_active"] = is_active;
        json["score"] = score;

        // Serialize tags array
        auto tags_arr = json["tags"];
        tags_arr.setArray();
        for (const auto& tag : tags) {
            tags_arr << tag;
        }

        // Serialize optional note
        if (note) {
            json["note"] = *note;
        }

        return true;
    }

    // Simple business logic: validate user data
    bool isValid() const {
        return id > 0 && !name.empty() && age > 0 && age < 150;
    }

    // Simple business logic: calculate user level based on score
    std::string getLevel() const {
        if (score >= 90.0) return "Expert";
        if (score >= 70.0) return "Advanced";
        if (score >= 50.0) return "Intermediate";
        return "Beginner";
    }

    // Display user information
    void display() const {
        std::cout << "User ID: " << id << "\n";
        std::cout << "  Name: " << name << "\n";
        std::cout << "  Email: " << email << "\n";
        std::cout << "  Age: " << age << "\n";
        std::cout << "  Active: " << (is_active ? "Yes" : "No") << "\n";
        std::cout << "  Score: " << score << " (" << getLevel() << ")\n";
        std::cout << "  Tags: ";
        for (const auto& tag : tags) {
            std::cout << tag << " ";
        }
        std::cout << "\n";
        if (note) {
            std::cout << "  Note: " << *note << "\n";
        }
    }
};

void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << prog_name << " [file.json]\n";
    std::cerr << "If no file is provided, uses default JSON.\n";
    std::cerr << "If file is '-', reads from stdin.\n";
}

std::string get_default_json() {
    return R"({
    "id": 1001,
    "name": "Alice Smith",
    "email": "alice@example.com",
    "age": 28,
    "is_active": true,
    "score": 85.5,
    "tags": ["developer", "c++", "json"],
    "note": "Team lead for backend development"
})";
}

int main(int argc, char* argv[]) {
    std::string json_content;

    // Read JSON from file or stdin
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-") {
            // Read from stdin
            json_content.assign((std::istreambuf_iterator<char>(std::cin)),
                                std::istreambuf_iterator<char>());
        } else {
            // Read from file
            std::ifstream file(arg);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot open file '" << arg << "'\n";
                return 1;
            }
            json_content.assign((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
        }
    } else {
        // Use default JSON
        json_content = get_default_json();
    }

    // Parse JSON using MutableDocument
    MutableDocument doc(json_content);
    if (doc.hasError()) {
        std::cerr << "Error: Invalid JSON format\n";
        return 1;
    }

    // Deserialize JSON to struct
    User user;
    if (!user.fromJson(doc)) {
        std::cerr << "Error: Failed to deserialize JSON to User struct\n";
        return 1;
    }

    // Display user information
    std::cout << "=== Deserialized User ===\n";
    user.display();
    std::cout << "\n";

    // Perform business logic validation
    if (!user.isValid()) {
        std::cerr << "Warning: User data validation failed\n";
        return 1;
    }

    std::cout << "=== Business Logic ===\n";
    std::cout << "User Level: " << user.getLevel() << "\n";
    std::cout << "Validation: PASSED\n\n";

    // Serialize struct back to JSON
    MutableDocument mut_doc;
    auto mut_json = *mut_doc;
    user.toJson(mut_json);

    // Add additional field (demonstrating MutableValue operations)
    mut_json["processed_at"] = "2025-11-05";
    mut_json["processed"] = true;

    std::cout << "=== Reserialized JSON ===\n";
    std::string output;
    mut_doc.write(output);
    std::cout << output << "\n";

    return 0;
}
