#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>

/**
 * @brief Utility functions for string manipulation, file I/O, and validation
 */
namespace Utils {
    
    /**
     * @brief Converts string to uppercase
     * @param text Input string
     * @return Uppercase string
     */
    std::string toUpperCase(const std::string& text);
    
    /**
     * @brief Converts string to lowercase
     * @param text Input string
     * @return Lowercase string
     */
    std::string toLowerCase(const std::string& text);
    
    /**
     * @brief Removes all non-alphabetic characters from string
     * @param text Input string
     * @return String containing only alphabetic characters
     */
    std::string removeNonAlpha(const std::string& text);
    
    /**
     * @brief Reads entire file content as string
     * @param filename Path to file
     * @return File contents as string, empty if file cannot be read
     */
    std::string readFile(const std::string& filename);
    
    /**
     * @brief Writes string content to file
     * @param filename Path to file
     * @param content Content to write
     * @return true if successful, false otherwise
     */
    bool writeFile(const std::string& filename, const std::string& content);
    
    /**
     * @brief Splits string by delimiter
     * @param text Input string
     * @param delimiter Character to split on
     * @return Vector of split strings
     */
    std::vector<std::string> split(const std::string& text, char delimiter);
    
    /**
     * @brief Validates input text for cipher processing
     * @param text Input text
     * @return true if valid, false otherwise
     */
    bool isValidInput(const std::string& text);
    
    /**
     * @brief Removes whitespace from beginning and end of string
     * @param text Input string
     * @return Trimmed string
     */
    std::string trim(const std::string& text);
    
    /**
     * @brief Normalizes text for cipher analysis (uppercase, no spaces/punctuation)
     * @param text Input text
     * @return Normalized text
     */
    std::string normalizeText(const std::string& text);
    
    /**
     * @brief Checks if file exists and is readable
     * @param filename Path to file
     * @return true if file exists and is readable
     */
    bool fileExists(const std::string& filename);
    
    /**
     * @brief Calculates percentage and formats as string
     * @param value Current value
     * @param total Total value
     * @param precision Decimal places
     * @return Formatted percentage string
     */
    std::string formatPercentage(double value, double total, int precision = 1);
    
    /**
     * @brief Generates progress bar string
     * @param current Current progress
     * @param total Total progress
     * @param width Width of progress bar
     * @return Progress bar string
     */
    std::string getProgressBar(int current, int total, int width = 50);
}

#endif // UTILS_H