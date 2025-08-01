#ifndef CAESAR_BREAKER_H
#define CAESAR_BREAKER_H

#include "CipherBreaker.h"
#include "FrequencyAnalyzer.h"
#include <utility>
#include <vector>
#include <map>

/**
 * @brief Caesar cipher breaker using frequency analysis
 * 
 * This class implements frequency analysis techniques to break Caesar ciphers.
 * It tries all possible shifts (0-25) and uses statistical analysis to determine
 * the most likely plaintext.
 */
class CaesarBreaker : public CipherBreaker {
public:
    /**
     * @brief Constructor
     */
    CaesarBreaker();
    
    /**
     * @brief Destructor
     */
    ~CaesarBreaker() = default;
    
    // CipherBreaker interface implementation
    std::string breakCipher(const std::string& ciphertext) override;
    std::vector<std::string> getPossibleSolutions(const std::string& ciphertext) override;
    double scorePlaintext(const std::string& plaintext) override;
    std::string getCipherType() const override { return "caesar"; }
    
    /**
     * @brief Finds all possible keys with their scores
     * @param ciphertext Encrypted text
     * @return Vector of pairs (key, score) sorted by score (best first)
     */
    std::vector<std::pair<int, double>> findPossibleKeys(const std::string& ciphertext);
    
    /**
     * @brief Decrypts text with specific Caesar key
     * @param ciphertext Encrypted text
     * @param key Caesar shift value (0-25)
     * @return Decrypted text
     */
    std::string decrypt(const std::string& ciphertext, int key);
    
    /**
     * @brief Finds the most likely Caesar key
     * @param ciphertext Encrypted text
     * @return Most likely key value
     */
    int findBestKey(const std::string& ciphertext);
    
    /**
     * @brief Encrypts text with Caesar cipher (for testing)
     * @param plaintext Text to encrypt
     * @param key Caesar shift value
     * @return Encrypted text
     */
    std::string encrypt(const std::string& plaintext, int key);
    
    /**
     * @brief Sets minimum text length for reliable analysis
     * @param minLength Minimum length
     */
    void setMinimumTextLength(int minLength) { this->minTextLength = minLength; }
    
    /**
     * @brief Gets detailed analysis results for all keys
     * @param ciphertext Encrypted text
     * @return Map of key to analysis details
     */
    std::map<int, double> getDetailedAnalysis(const std::string& ciphertext);

private:
    /// Frequency analyzer instance
    FrequencyAnalyzer analyzer;
    
    /// Minimum text length for reliable analysis
    int minTextLength;
    
    /// Last analysis results (for debugging)
    std::map<int, double> lastAnalysis;
    
    /**
     * @brief Performs Caesar shift on a single character
     * @param c Character to shift
     * @param shift Shift amount
     * @return Shifted character
     */
    char caesarShiftChar(char c, int shift);
    
    /**
     * @brief Applies Caesar shift to entire string
     * @param text Text to shift
     * @param shift Shift amount
     * @return Shifted text
     */
    std::string caesarShift(const std::string& text, int shift);
    
    /**
     * @brief Scores a potential plaintext using multiple methods
     * @param text Text to score
     * @return Combined score (higher is better)
     */
    double scoreText(const std::string& text);
    
    /**
     * @brief Calculates chi-squared score for frequency analysis
     * @param text Text to analyze
     * @return Chi-squared score (lower is better)
     */
    double calculateChiSquared(const std::string& text);
    
    /**
     * @brief Calculates Index of Coincidence score
     * @param text Text to analyze
     * @return IC score (higher is better for English)
     */
    double calculateICScore(const std::string& text);
    
    /**
     * @brief Checks for common English patterns
     * @param text Text to check
     * @return Pattern score (0-1, higher is better)
     */
    double checkCommonPatterns(const std::string& text);
    
    /**
     * @brief Normalizes a score to 0-1 range
     * @param score Raw score
     * @param minVal Minimum possible value
     * @param maxVal Maximum possible value
     * @return Normalized score
     */
    double normalizeScore(double score, double minVal, double maxVal);
    
    /**
     * @brief Updates verbose output during analysis
     * @param key Current key being tested
     * @param score Score for this key
     * @param preview Preview of decrypted text
     */
    void updateProgress(int key, double score, const std::string& preview);
};

#endif // CAESAR_BREAKER_H