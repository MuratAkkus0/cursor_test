#ifndef LANGUAGE_DETECTOR_H
#define LANGUAGE_DETECTOR_H

#include "FrequencyAnalyzer.h"
#include <string>
#include <vector>
#include <map>

/**
 * @brief Advanced language detection and multilingual support
 */
class LanguageDetector {
public:
    /**
     * @brief Constructor
     */
    LanguageDetector();
    
    /**
     * @brief Detects language using multiple methods
     * @param text Text to analyze
     * @return Most likely language with confidence score
     */
    std::pair<std::string, double> detectLanguage(const std::string& text);
    
    /**
     * @brief Gets supported languages
     * @return Vector of supported language codes
     */
    std::vector<std::string> getSupportedLanguages() const;
    
    /**
     * @brief Loads additional language frequency data
     * @param language Language code
     * @param frequencyFile Path to frequency file
     * @return Success status
     */
    bool loadLanguage(const std::string& language, const std::string& frequencyFile);

private:
    FrequencyAnalyzer analyzer;
    std::map<std::string, std::map<char, double>> languageProfiles;
    
    void initializeLanguageProfiles();
};

#endif // LANGUAGE_DETECTOR_H