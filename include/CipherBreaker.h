#ifndef CIPHER_BREAKER_H
#define CIPHER_BREAKER_H

#include <string>
#include <vector>
#include <memory>

/**
 * @brief Abstract base class for all cipher breakers
 * 
 * This class defines the common interface that all cipher breaking algorithms
 * must implement. It provides a consistent API for different cipher types.
 */
class CipherBreaker {
public:
    /**
     * @brief Virtual destructor for proper polymorphic destruction
     */
    virtual ~CipherBreaker() = default;
    
    /**
     * @brief Main cipher breaking method
     * @param ciphertext Encrypted text to break
     * @return Most likely plaintext solution
     */
    virtual std::string breakCipher(const std::string& ciphertext) = 0;
    
    /**
     * @brief Gets multiple possible solutions ranked by probability
     * @param ciphertext Encrypted text to analyze
     * @return Vector of possible plaintext solutions in order of likelihood
     */
    virtual std::vector<std::string> getPossibleSolutions(const std::string& ciphertext) = 0;
    
    /**
     * @brief Scores how likely a text is to be correct plaintext
     * @param plaintext Text to score
     * @return Score (higher means more likely to be correct)
     */
    virtual double scorePlaintext(const std::string& plaintext) = 0;
    
    /**
     * @brief Gets the name/type of this cipher breaker
     * @return String identifier for the cipher type
     */
    virtual std::string getCipherType() const = 0;
    
    /**
     * @brief Gets confidence level of the best solution
     * @return Confidence percentage (0-100)
     */
    virtual double getConfidence() const { return confidence; }
    
    /**
     * @brief Gets time taken for last analysis
     * @return Time in milliseconds
     */
    virtual double getAnalysisTime() const { return analysisTimeMs; }
    
    /**
     * @brief Sets whether to show verbose output during analysis
     * @param verbose True to enable verbose output
     */
    virtual void setVerbose(bool verbose) { this->verbose = verbose; }
    
    /**
     * @brief Sets the target language for analysis
     * @param language Language code (e.g., "english", "turkish")
     */
    virtual void setTargetLanguage(const std::string& language) { 
        this->targetLanguage = language; 
    }

protected:
    /// Confidence level of the last analysis (0-100)
    double confidence = 0.0;
    
    /// Time taken for last analysis in milliseconds
    double analysisTimeMs = 0.0;
    
    /// Whether to show verbose output
    bool verbose = false;
    
    /// Target language for analysis
    std::string targetLanguage = "english";
    
    /**
     * @brief Common method to validate input before processing
     * @param ciphertext Input text to validate
     * @return true if valid, false otherwise
     */
    bool validateInput(const std::string& ciphertext) const;
    
    /**
     * @brief Updates confidence based on analysis results
     * @param score Analysis score
     * @param maxScore Maximum possible score
     */
    void updateConfidence(double score, double maxScore);
    
    /**
     * @brief Prints verbose information if verbose mode is enabled
     * @param message Message to print
     */
    void printVerbose(const std::string& message) const;
};

/**
 * @brief Result structure for cipher analysis
 */
struct CipherResult {
    std::string plaintext;      ///< Decrypted text
    std::string key;           ///< Key used for decryption
    double score;              ///< Confidence score
    double timeMs;             ///< Time taken for analysis
    std::string cipherType;    ///< Type of cipher detected/used
    
    /**
     * @brief Constructor
     */
    CipherResult(const std::string& plaintext = "", 
                const std::string& key = "", 
                double score = 0.0,
                double timeMs = 0.0,
                const std::string& cipherType = "") 
        : plaintext(plaintext), key(key), score(score), timeMs(timeMs), cipherType(cipherType) {}
};

/**
 * @brief Factory function for creating cipher breakers
 * @param cipherType Type of cipher ("caesar", "substitution", "vigenere", "auto")
 * @return Unique pointer to cipher breaker instance
 */
std::unique_ptr<CipherBreaker> createCipherBreaker(const std::string& cipherType);

#endif // CIPHER_BREAKER_H