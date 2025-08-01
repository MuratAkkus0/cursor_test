#ifndef SUBSTITUTION_BREAKER_H
#define SUBSTITUTION_BREAKER_H

#include "CipherBreaker.h"
#include "FrequencyAnalyzer.h"
#include <map>
#include <set>
#include <random>

/**
 * @brief Substitution cipher breaker using frequency analysis and optimization
 * 
 * This class implements sophisticated techniques to break monoalphabetic substitution ciphers.
 * It uses frequency analysis, bigram/trigram analysis, and hill-climbing optimization
 * to find the most likely plaintext mapping.
 */
class SubstitutionBreaker : public CipherBreaker {
public:
    /**
     * @brief Constructor
     */
    SubstitutionBreaker();
    
    /**
     * @brief Destructor
     */
    ~SubstitutionBreaker() = default;
    
    // CipherBreaker interface implementation
    std::string breakCipher(const std::string& ciphertext) override;
    std::vector<std::string> getPossibleSolutions(const std::string& ciphertext) override;
    double scorePlaintext(const std::string& plaintext) override;
    std::string getCipherType() const override { return "substitution"; }
    
    /**
     * @brief Breaks cipher using frequency analysis as starting point
     * @param ciphertext Encrypted text
     * @return Most likely plaintext
     */
    std::string breakByFrequency(const std::string& ciphertext);
    
    /**
     * @brief Generates initial mapping based on frequency analysis
     * @param ciphertext Encrypted text
     * @return Initial character mapping (cipher -> plain)
     */
    std::map<char, char> generateMapping(const std::string& ciphertext);
    
    /**
     * @brief Improves mapping using bigram analysis
     * @param initialMapping Starting mapping
     * @param ciphertext Original ciphertext
     * @return Improved mapping
     */
    std::map<char, char> improveMappingWithBigrams(const std::map<char, char>& initialMapping,
                                                   const std::string& ciphertext);
    
    /**
     * @brief Scores a mapping by applying it and analyzing result
     * @param mapping Character mapping to test
     * @param ciphertext Text to decrypt with mapping
     * @return Score (higher is better)
     */
    double scoreMapping(const std::map<char, char>& mapping, const std::string& ciphertext);
    
    /**
     * @brief Hill climbing optimization to improve mapping
     * @param initialMapping Starting point
     * @param ciphertext Text to optimize against
     * @param maxIterations Maximum optimization steps
     * @return Optimized mapping
     */
    std::map<char, char> hillClimbOptimization(const std::map<char, char>& initialMapping,
                                               const std::string& ciphertext,
                                               int maxIterations = 1000);
    
    /**
     * @brief Simulated annealing optimization for better global optima
     * @param initialMapping Starting point
     * @param ciphertext Text to optimize against
     * @param maxIterations Maximum iterations
     * @param initialTemperature Starting temperature
     * @return Optimized mapping
     */
    std::map<char, char> simulatedAnnealing(const std::map<char, char>& initialMapping,
                                            const std::string& ciphertext,
                                            int maxIterations = 2000,
                                            double initialTemperature = 100.0);
    
    /**
     * @brief Sets optimization method
     * @param method "frequency", "hill_climbing", "simulated_annealing", "hybrid"
     */
    void setOptimizationMethod(const std::string& method) { this->optimizationMethod = method; }
    
    /**
     * @brief Sets maximum iterations for optimization algorithms
     * @param maxIter Maximum iterations
     */
    void setMaxIterations(int maxIter) { this->maxIterations = maxIter; }
    
    /**
     * @brief Gets detailed optimization history for analysis
     * @return Vector of (iteration, score) pairs
     */
    std::vector<std::pair<int, double>> getOptimizationHistory() const { return optimizationHistory; }
    
    /**
     * @brief Applies character mapping to text (public for testing)
     * @param text Text to transform
     * @param mapping Character mapping
     * @return Transformed text
     */
    std::string applyMapping(const std::string& text, const std::map<char, char>& mapping);

private:
    /// Frequency analyzer instance
    FrequencyAnalyzer analyzer;
    
    /// English bigram frequencies
    std::map<std::string, double> bigramFreq;
    
    /// English trigram frequencies
    std::map<std::string, double> trigramFreq;
    
    /// Common English words for validation
    std::set<std::string> commonWords;
    
    /// Optimization method to use
    std::string optimizationMethod;
    
    /// Maximum iterations for optimization
    int maxIterations;
    
    /// Random number generator
    mutable std::mt19937 rng;
    
    /// Optimization history for debugging
    std::vector<std::pair<int, double>> optimizationHistory;
    
    /**
     * @brief Initializes bigram frequency tables
     */
    void initializeBigramFrequencies();
    
    /**
     * @brief Initializes trigram frequency tables
     */
    void initializeTrigramFrequencies();
    
    /**
     * @brief Loads common English words
     */
    void loadCommonWords();
    

    
    /**
     * @brief Calculates bigram score for text
     * @param text Text to analyze
     * @return Bigram score (higher is better)
     */
    double calculateBigramScore(const std::string& text);
    
    /**
     * @brief Calculates trigram score for text
     * @param text Text to analyze
     * @return Trigram score (higher is better)
     */
    double calculateTrigramScore(const std::string& text);
    
    /**
     * @brief Calculates common word score
     * @param text Text to analyze
     * @return Word score (higher is better)
     */
    double calculateWordScore(const std::string& text);
    
    /**
     * @brief Generates a neighbor mapping by swapping two characters
     * @param mapping Current mapping
     * @return Neighboring mapping
     */
    std::map<char, char> generateNeighborMapping(const std::map<char, char>& mapping);
    
    /**
     * @brief Ensures mapping is complete and valid
     * @param mapping Mapping to validate
     * @return Complete valid mapping
     */
    std::map<char, char> completeMapping(const std::map<char, char>& mapping);
    
    /**
     * @brief Swaps two characters in a mapping
     * @param mapping Mapping to modify
     * @param char1 First character
     * @param char2 Second character
     * @return Modified mapping
     */
    std::map<char, char> swapCharacters(const std::map<char, char>& mapping, char char1, char char2);
    
    /**
     * @brief Updates optimization progress for verbose output
     * @param iteration Current iteration
     * @param score Current score
     * @param temperature Current temperature (for SA)
     */
    void updateOptimizationProgress(int iteration, double score, double temperature = 0.0);
    
    /**
     * @brief Calculates combined score from multiple metrics
     * @param freqScore Frequency score
     * @param bigramScore Bigram score
     * @param trigramScore Trigram score
     * @param wordScore Word score
     * @return Combined score
     */
    double calculateCombinedScore(double freqScore, double bigramScore, 
                                 double trigramScore, double wordScore);
};

#endif // SUBSTITUTION_BREAKER_H