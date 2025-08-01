#include "LanguageDetector.h"
#include "Utils.h"
#include <algorithm>
#include <cmath>
#include <limits>

LanguageDetector::LanguageDetector() {
    initializeLanguageProfiles();
}

std::pair<std::string, double> LanguageDetector::detectLanguage(const std::string& text) {
    auto textFreq = analyzer.calculateFrequency(text);
    
    double bestScore = std::numeric_limits<double>::max();
    std::string bestLanguage = "unknown";
    
    for (const auto& profile : languageProfiles) {
        double score = analyzer.chiSquaredTest(textFreq, profile.second);
        if (score < bestScore) {
            bestScore = score;
            bestLanguage = profile.first;
        }
    }
    
    // Convert chi-squared to confidence (0-1)
    double confidence = 1.0 / (1.0 + bestScore / 100.0);
    
    return std::make_pair(bestLanguage, confidence);
}

std::vector<std::string> LanguageDetector::getSupportedLanguages() const {
    std::vector<std::string> languages;
    for (const auto& profile : languageProfiles) {
        languages.push_back(profile.first);
    }
    return languages;
}

bool LanguageDetector::loadLanguage(const std::string& language, const std::string& frequencyFile) {
    return analyzer.loadLanguageFrequencies(language, frequencyFile);
}

void LanguageDetector::initializeLanguageProfiles() {
    // Initialize with basic English and Turkish profiles
    languageProfiles["english"] = analyzer.getLanguageFrequencies("english");
    languageProfiles["turkish"] = analyzer.getLanguageFrequencies("turkish");
}