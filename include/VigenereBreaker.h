#ifndef VIGENERE_BREAKER_H
#define VIGENERE_BREAKER_H

#include "CipherBreaker.h"
#include "FrequencyAnalyzer.h"
#include "CaesarBreaker.h"
#include <vector>
#include <map>
#include <utility>

/**
 * @brief Vigenère cipher breaker using Kasiski examination and frequency analysis
 * 
 * This class implements sophisticated techniques to break polyalphabetic Vigenère ciphers.
 * It uses Kasiski examination to find key length, Index of Coincidence analysis,
 * and Caesar cipher breaking for each key position.
 */
class VigenereBreaker : public CipherBreaker {
public:
    /**
     * @brief Constructor
     */
    VigenereBreaker();
    
    /**
     * @brief Destructor
     */
    ~VigenereBreaker() = default;
    
    // CipherBreaker interface implementation
    std::string breakCipher(const std::string& ciphertext) override;
    std::vector<std::string> getPossibleSolutions(const std::string& ciphertext) override;
    double scorePlaintext(const std::string& plaintext) override;
    std::string getCipherType() const override { return "vigenere"; }
    
    /**
     * @brief Finds possible key lengths using multiple methods
     * @param ciphertext Encrypted text
     * @return Vector of likely key lengths (ranked by confidence)
     */
    std::vector<int> findKeyLength(const std::string& ciphertext);
    
    /**
     * @brief Finds the key for a given key length
     * @param ciphertext Encrypted text
     * @param keyLength Length of the key
     * @return Most likely key string
     */
    std::string findKey(const std::string& ciphertext, int keyLength);
    
    /**
     * @brief Kasiski examination for finding key length
     * @param ciphertext Encrypted text
     * @return Vector of candidate key lengths with confidence scores
     */
    std::vector<std::pair<int, double>> kasiskiExamination(const std::string& ciphertext);
    
    /**
     * @brief Index of Coincidence method for key length detection
     * @param ciphertext Encrypted text
     * @return Vector of candidate key lengths with IC scores
     */
    std::vector<std::pair<int, double>> indexOfCoincidenceMethod(const std::string& ciphertext);
    
    /**
     * @brief Encrypts text with Vigenère cipher (for testing)
     * @param plaintext Text to encrypt
     * @param key Encryption key
     * @return Encrypted text
     */
    std::string encrypt(const std::string& plaintext, const std::string& key);
    
    /**
     * @brief Decrypts text with known Vigenère key
     * @param ciphertext Encrypted text
     * @param key Decryption key
     * @return Decrypted text
     */
    std::string decrypt(const std::string& ciphertext, const std::string& key);
    
    /**
     * @brief Sets minimum and maximum key lengths to search
     * @param minLen Minimum key length (default: 2)
     * @param maxLen Maximum key length (default: 20)
     */
    void setKeyLengthRange(int minLen, int maxLen) { 
        this->minKeyLength = minLen; 
        this->maxKeyLength = maxLen; 
    }
    
    /**
     * @brief Sets minimum substring length for Kasiski examination
     * @param minLen Minimum substring length (default: 3)
     */
    void setMinSubstringLength(int minLen) { this->minSubstringLength = minLen; }
    
    /**
     * @brief Gets detailed analysis results for debugging
     * @return Map of analysis method to results
     */
    std::map<std::string, std::vector<std::pair<int, double>>> getAnalysisResults() const {
        return analysisResults;
    }
    
    // Public utility functions for testing
    std::vector<std::string> splitByKeyPosition(const std::string& text, int keyLength);
    int charToShift(char c);
    char shiftToChar(int shift);

private:
    /// Frequency analyzer instance
    FrequencyAnalyzer analyzer;
    
    /// Caesar breaker for individual key positions
    CaesarBreaker caesarBreaker;
    
    /// Minimum key length to search
    int minKeyLength;
    
    /// Maximum key length to search
    int maxKeyLength;
    
    /// Minimum substring length for Kasiski examination
    int minSubstringLength;
    
    /// Analysis results for debugging
    std::map<std::string, std::vector<std::pair<int, double>>> analysisResults;
    
    /**
     * @brief Calculates Index of Coincidence for specific key length
     * @param text Input text
     * @param keyLength Key length to test
     * @return IC value for this key length
     */
    double calculateICForKeyLength(const std::string& text, int keyLength);
    

    
    /**
     * @brief Finds the character for a specific key position
     * @param subtextCipher Cipher text for this position
     * @return Most likely key character
     */
    char findSubkey(const std::string& subtextCipher);
    
    /**
     * @brief Finds repeating substrings and their distances
     * @param text Input text
     * @param minLength Minimum substring length
     * @return Map of substring to list of positions
     */
    std::map<std::string, std::vector<int>> findRepeatingSubstrings(const std::string& text, int minLength);
    
    /**
     * @brief Calculates GCD of a list of numbers
     * @param numbers List of numbers
     * @return Greatest Common Divisor
     */
    int calculateGCD(const std::vector<int>& numbers);
    
    /**
     * @brief Helper function to calculate GCD of two numbers
     * @param a First number
     * @param b Second number
     * @return GCD of a and b
     */
    int gcd(int a, int b);
    
    /**
     * @brief Analyzes distance patterns in Kasiski examination
     * @param distances Vector of distances between repeated patterns
     * @return Map of possible key lengths to confidence scores
     */
    std::map<int, double> analyzeDistancePatterns(const std::vector<int>& distances);
    
    /**
     * @brief Combines results from different key length detection methods
     * @param kasiskiResults Results from Kasiski examination
     * @param icResults Results from IC analysis
     * @return Combined ranked list of key lengths
     */
    std::vector<int> combineKeyLengthResults(const std::vector<std::pair<int, double>>& kasiskiResults,
                                           const std::vector<std::pair<int, double>>& icResults);
    
    /**
     * @brief Validates if a found key makes sense
     * @param key Proposed key
     * @param ciphertext Original ciphertext
     * @return Validation score (0-1)
     */
    double validateKey(const std::string& key, const std::string& ciphertext);
    
    /**
     * @brief Updates progress during key length search
     * @param currentLength Current key length being tested
     * @param maxLength Maximum key length
     * @param method Method being used
     * @param score Current score
     */
    void updateKeySearchProgress(int currentLength, int maxLength, 
                               const std::string& method, double score);
    
    /**
     * @brief Performs Vigenère character shift
     * @param c Character to shift
     * @param shift Shift amount
     * @return Shifted character
     */
    char vigenereShift(char c, int shift);
    

};

#endif // VIGENERE_BREAKER_H