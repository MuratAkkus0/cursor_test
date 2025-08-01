#ifndef AUTO_CIPHER_DETECTOR_H
#define AUTO_CIPHER_DETECTOR_H

#include "FrequencyAnalyzer.h"
#include "LanguageDetector.h"
#include <string>
#include <vector>
#include <map>

/**
 * @brief Automatic cipher type detection
 */
class AutoCipherDetector {
public:
    /**
     * @brief Cipher detection result
     */
    struct CipherDetectionResult {
        std::string cipherType;     ///< Detected cipher type
        double confidence;          ///< Confidence score (0-1)
        std::string reasoning;      ///< Explanation of detection
        std::map<std::string, double> scores; ///< All cipher type scores
        
        CipherDetectionResult(const std::string& type = "unknown", 
                            double conf = 0.0, 
                            const std::string& reason = "")
            : cipherType(type), confidence(conf), reasoning(reason) {}
    };
    
    /**
     * @brief Constructor
     */
    AutoCipherDetector();
    
    /**
     * @brief Detects the most likely cipher type
     * @param ciphertext Text to analyze
     * @return Detection result with confidence
     */
    CipherDetectionResult detectCipherType(const std::string& ciphertext);
    
    /**
     * @brief Gets all possible cipher types with scores
     * @param ciphertext Text to analyze
     * @return Map of cipher types to confidence scores
     */
    std::map<std::string, double> getAllCipherScores(const std::string& ciphertext);
    
    /**
     * @brief Sets minimum text length for reliable detection
     * @param minLength Minimum length
     */
    void setMinimumTextLength(int minLength) { this->minTextLength = minLength; }

private:
    FrequencyAnalyzer frequencyAnalyzer;
    LanguageDetector languageDetector;
    int minTextLength;
    
    /**
     * @brief Calculates score for Caesar cipher likelihood
     * @param text Text to analyze
     * @return Score (0-1)
     */
    double calculateCaesarScore(const std::string& text);
    
    /**
     * @brief Calculates score for Substitution cipher likelihood
     * @param text Text to analyze
     * @return Score (0-1)
     */
    double calculateSubstitutionScore(const std::string& text);
    
    /**
     * @brief Calculates score for Vigenère cipher likelihood
     * @param text Text to analyze
     * @return Score (0-1)
     */
    double calculateVigenereScore(const std::string& text);
    
    /**
     * @brief Calculates score for plaintext likelihood
     * @param text Text to analyze
     * @return Score (0-1)
     */
    double calculatePlaintextScore(const std::string& text);
    
    /**
     * @brief Analyzes character frequency distribution patterns
     * @param text Text to analyze
     * @return Pattern analysis results
     */
    std::map<std::string, double> analyzeFrequencyPatterns(const std::string& text);
    
    /**
     * @brief Creates reasoning text based on analysis
     * @param cipherType Detected cipher type
     * @param analysis Analysis results
     * @return Human-readable reasoning
     */
    std::string generateReasoning(const std::string& cipherType, 
                                 const std::map<std::string, double>& analysis);
};

#endif // AUTO_CIPHER_DETECTOR_H