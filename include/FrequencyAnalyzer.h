#ifndef FREQUENCY_ANALYZER_H
#define FREQUENCY_ANALYZER_H

#include <map>
#include <string>
#include <vector>

/**
 * @brief Frequency analysis engine for cryptanalysis
 * 
 * This class provides frequency analysis capabilities for breaking classical ciphers.
 * It can calculate character frequencies, perform chi-squared tests, and detect languages.
 */
class FrequencyAnalyzer {
public:
    /**
     * @brief Constructor initializes default language frequencies
     */
    FrequencyAnalyzer();
    
    /**
     * @brief Destructor
     */
    ~FrequencyAnalyzer() = default;
    
    /**
     * @brief Calculates character frequency distribution in text
     * @param text Input text to analyze
     * @return Map of character to frequency percentage
     */
    std::map<char, double> calculateFrequency(const std::string& text);
    
    /**
     * @brief Performs chi-squared test between observed and expected frequencies
     * @param observed Observed frequency distribution
     * @param expected Expected frequency distribution
     * @return Chi-squared value (lower is better match)
     */
    double chiSquaredTest(const std::map<char, double>& observed,
                         const std::map<char, double>& expected);
    
    /**
     * @brief Detects language of text based on frequency analysis
     * @param text Input text
     * @return Language name (e.g., "english", "turkish") or "unknown"
     */
    std::string detectLanguage(const std::string& text);
    
    /**
     * @brief Loads language frequency data from file
     * @param language Language name
     * @param filename Path to frequency file
     * @return true if successful, false otherwise
     */
    bool loadLanguageFrequencies(const std::string& language,
                                const std::string& filename);
    
    /**
     * @brief Gets expected frequencies for a language
     * @param language Language name
     * @return Map of character frequencies, empty if language not found
     */
    std::map<char, double> getLanguageFrequencies(const std::string& language) const;
    
    /**
     * @brief Gets list of supported languages
     * @return Vector of supported language names
     */
    std::vector<std::string> getSupportedLanguages() const;
    
    /**
     * @brief Calculates Index of Coincidence for text
     * @param text Input text
     * @return Index of Coincidence value
     */
    double calculateIndexOfCoincidence(const std::string& text);
    
    /**
     * @brief Finds most common bigrams in text
     * @param text Input text
     * @param count Number of top bigrams to return
     * @return Vector of pairs (bigram, frequency)
     */
    std::vector<std::pair<std::string, double>> findCommonBigrams(const std::string& text, int count = 10);
    
    /**
     * @brief Finds most common trigrams in text
     * @param text Input text
     * @param count Number of top trigrams to return
     * @return Vector of pairs (trigram, frequency)
     */
    std::vector<std::pair<std::string, double>> findCommonTrigrams(const std::string& text, int count = 10);
    
    /**
     * @brief Scores how "English-like" a text is
     * @param text Input text
     * @return Score (higher is more English-like)
     */
    double scoreEnglishness(const std::string& text);

private:
    /// Language frequency tables
    std::map<std::string, std::map<char, double>> languageFrequencies;
    
    /// Common English bigrams with frequencies
    std::map<std::string, double> englishBigrams;
    
    /// Common English trigrams with frequencies
    std::map<std::string, double> englishTrigrams;
    
    /**
     * @brief Initializes default frequency tables
     */
    void initializeDefaultFrequencies();
    
    /**
     * @brief Initializes English bigram frequencies
     */
    void initializeBigramFrequencies();
    
    /**
     * @brief Initializes English trigram frequencies
     */
    void initializeTrigramFrequencies();
    
    /**
     * @brief Normalizes frequency map to percentages
     * @param frequencies Raw frequency counts
     * @param totalChars Total character count
     * @return Normalized frequency percentages
     */
    std::map<char, double> normalizeFrequencies(const std::map<char, int>& frequencies, 
                                               int totalChars) const;
};

#endif // FREQUENCY_ANALYZER_H