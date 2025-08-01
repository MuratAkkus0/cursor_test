#include "Utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <regex>

namespace Utils {

std::string toUpperCase(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string toLowerCase(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string removeNonAlpha(const std::string& text) {
    std::string result;
    result.reserve(text.length());
    
    for (char c : text) {
        if (std::isalpha(c)) {
            result += c;
        }
    }
    
    return result;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create file " << filename << std::endl;
        return false;
    }
    
    file << content;
    return file.good();
}

std::vector<std::string> split(const std::string& text, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(text);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

bool isValidInput(const std::string& text) {
    if (text.empty()) {
        return false;
    }
    
    // Check if text contains at least some alphabetic characters
    int alphaCount = 0;
    for (char c : text) {
        if (std::isalpha(c)) {
            alphaCount++;
        }
    }
    
    // Require at least 50% alphabetic characters for meaningful analysis
    return alphaCount >= text.length() * 0.5;
}

std::string trim(const std::string& text) {
    size_t start = text.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = text.find_last_not_of(" \t\n\r\f\v");
    return text.substr(start, end - start + 1);
}

std::string normalizeText(const std::string& text) {
    std::string result = removeNonAlpha(text);
    return toUpperCase(result);
}

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

std::string formatPercentage(double value, double total, int precision) {
    if (total == 0) {
        return "0.0%";
    }
    
    double percentage = (value / total) * 100.0;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << percentage << "%";
    return ss.str();
}

std::string getProgressBar(int current, int total, int width) {
    if (total == 0) {
        return "[" + std::string(width, ' ') + "] 0%";
    }
    
    double progress = static_cast<double>(current) / total;
    int filled = static_cast<int>(progress * width);
    
    std::string bar = "[";
    bar += std::string(filled, '=');
    if (filled < width) {
        bar += ">";
        bar += std::string(width - filled - 1, ' ');
    }
    bar += "] ";
    bar += formatPercentage(current, total, 0);
    
    return bar;
}

} // namespace Utils