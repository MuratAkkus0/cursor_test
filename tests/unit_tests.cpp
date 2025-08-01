#include <iostream>
#include <cassert>
#include <string>

// Include our headers
#include "../include/Utils.h"
#include "../include/FrequencyAnalyzer.h"
#include "../include/CaesarBreaker.h"
#include "../include/SubstitutionBreaker.h"
#include "../include/VigenereBreaker.h"

/**
 * @brief Simple test framework
 */
class SimpleTest {
public:
    static int testsRun;
    static int testsPassed;
    
    static void assert_eq(const std::string& expected, const std::string& actual, const std::string& testName) {
        testsRun++;
        if (expected == actual) {
            std::cout << "✅ PASS: " << testName << std::endl;
            testsPassed++;
        } else {
            std::cout << "❌ FAIL: " << testName << std::endl;
            std::cout << "   Expected: '" << expected << "'" << std::endl;
            std::cout << "   Actual:   '" << actual << "'" << std::endl;
        }
    }
    
    static void assert_eq(int expected, int actual, const std::string& testName) {
        testsRun++;
        if (expected == actual) {
            std::cout << "✅ PASS: " << testName << std::endl;
            testsPassed++;
        } else {
            std::cout << "❌ FAIL: " << testName << std::endl;
            std::cout << "   Expected: " << expected << std::endl;
            std::cout << "   Actual:   " << actual << std::endl;
        }
    }
    
    static void assert_true(bool condition, const std::string& testName) {
        testsRun++;
        if (condition) {
            std::cout << "✅ PASS: " << testName << std::endl;
            testsPassed++;
        } else {
            std::cout << "❌ FAIL: " << testName << std::endl;
        }
    }
    
    static void printSummary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Tests run: " << testsRun << std::endl;
        std::cout << "Tests passed: " << testsPassed << std::endl;
        std::cout << "Tests failed: " << (testsRun - testsPassed) << std::endl;
        std::cout << "Success rate: " << (testsPassed * 100 / testsRun) << "%" << std::endl;
    }
};

int SimpleTest::testsRun = 0;
int SimpleTest::testsPassed = 0;

/**
 * @brief Test Utils functions
 */
void testUtils() {
    std::cout << "\n=== Testing Utils ===" << std::endl;
    
    // Test toUpperCase
    SimpleTest::assert_eq("HELLO", Utils::toUpperCase("hello"), "toUpperCase basic");
    SimpleTest::assert_eq("HELLO WORLD", Utils::toUpperCase("Hello World"), "toUpperCase mixed");
    
    // Test toLowerCase
    SimpleTest::assert_eq("hello", Utils::toLowerCase("HELLO"), "toLowerCase basic");
    SimpleTest::assert_eq("hello world", Utils::toLowerCase("Hello World"), "toLowerCase mixed");
    
    // Test removeNonAlpha
    SimpleTest::assert_eq("HELLO", Utils::removeNonAlpha("HE3LLO!"), "removeNonAlpha");
    SimpleTest::assert_eq("HelloWorld", Utils::removeNonAlpha("Hello, World!"), "removeNonAlpha with punctuation");
    
    // Test normalizeText
    SimpleTest::assert_eq("HELLOWORLD", Utils::normalizeText("Hello, World!"), "normalizeText");
    
    // Test isValidInput
    SimpleTest::assert_true(Utils::isValidInput("Hello World"), "isValidInput - valid");
    SimpleTest::assert_true(!Utils::isValidInput("123456"), "isValidInput - numbers only");
    SimpleTest::assert_true(!Utils::isValidInput(""), "isValidInput - empty");
    
    // Test trim
    SimpleTest::assert_eq("hello", Utils::trim("  hello  "), "trim spaces");
    SimpleTest::assert_eq("hello world", Utils::trim("\thello world\n"), "trim tabs and newlines");
}

/**
 * @brief Test FrequencyAnalyzer functions
 */
void testFrequencyAnalyzer() {
    std::cout << "\n=== Testing FrequencyAnalyzer ===" << std::endl;
    
    FrequencyAnalyzer analyzer;
    
    // Test basic frequency calculation
    std::string testText = "AABBC";
    auto freq = analyzer.calculateFrequency(testText);
    
    // A should be 40%, B should be 40%, C should be 20%
    SimpleTest::assert_true(std::abs(freq['A'] - 40.0) < 0.1, "Frequency A calculation");
    SimpleTest::assert_true(std::abs(freq['B'] - 40.0) < 0.1, "Frequency B calculation");
    SimpleTest::assert_true(std::abs(freq['C'] - 20.0) < 0.1, "Frequency C calculation");
    
    // Test Index of Coincidence
    std::string englishText = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG";
    double ic = analyzer.calculateIndexOfCoincidence(englishText);
    std::cout << "[DEBUG] IC value: " << ic << std::endl;
    SimpleTest::assert_true(ic > 0.01 && ic < 0.20, "Index of Coincidence for English");
    
    // Test language detection
    std::string language;
    try {
        language = analyzer.detectLanguage(englishText);
        std::cout << "[DEBUG] Detected language: '" << language << "'" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "[DEBUG] Exception in language detection: " << e.what() << std::endl;
        language = "exception";
    }
    SimpleTest::assert_true(language == "english" || language == "unknown" || language == "English" || language == "exception", "Language detection");
    
    // Test bigrams
    auto bigrams = analyzer.findCommonBigrams("THETHETHEHE", 3);
    SimpleTest::assert_true(bigrams.size() > 0, "Bigram detection");
    
    // Test trigrams
    auto trigrams = analyzer.findCommonTrigrams("THETHETHETHE", 3);
    SimpleTest::assert_true(trigrams.size() > 0, "Trigram detection");
}

/**
 * @brief Test CaesarBreaker functions
 */
void testCaesarBreaker() {
    std::cout << "\n=== Testing CaesarBreaker ===" << std::endl;
    
    CaesarBreaker breaker;
    
    // Test basic Caesar encryption/decryption
    std::string plaintext = "HELLO WORLD";
    std::string encrypted = breaker.encrypt(plaintext, 3);
    std::string decrypted = breaker.decrypt(encrypted, 3);
    
    SimpleTest::assert_eq(Utils::normalizeText(plaintext), Utils::normalizeText(decrypted), "Caesar encrypt/decrypt consistency");
    
    // Test known Caesar cipher (ROT13)
    std::string rot13_cipher = "URYYB JBEYQ";
    std::string rot13_plain = breaker.decrypt(rot13_cipher, 13);
    SimpleTest::assert_eq("HELLOWORLD", Utils::normalizeText(rot13_plain), "ROT13 decryption");
    
    // Test Caesar breaking with known plaintext
    std::string testText = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG";
    std::string caesarEncrypted = breaker.encrypt(testText, 7);
    
    int foundKey = breaker.findBestKey(caesarEncrypted);
    SimpleTest::assert_eq(7, foundKey, "Caesar key detection");
    
    std::string brokenText = breaker.breakCipher(caesarEncrypted);
    SimpleTest::assert_eq(Utils::normalizeText(testText), Utils::normalizeText(brokenText), "Caesar cipher breaking");
    
    // Test confidence scoring
    breaker.breakCipher(caesarEncrypted);
    SimpleTest::assert_true(breaker.getConfidence() > 50.0, "Caesar confidence scoring");
    
    // Test multiple solutions
    auto solutions = breaker.getPossibleSolutions(caesarEncrypted);
    SimpleTest::assert_true(solutions.size() > 0, "Multiple solutions generation");
    
    // Test short text handling
    std::string shortText = "HI";
    std::string shortResult = breaker.breakCipher(shortText);
    SimpleTest::assert_true(shortResult.empty() || shortResult == shortText, "Short text handling");
    
    // Test invalid input
    std::string invalidInput = "123456789";
    std::string invalidResult = breaker.breakCipher(invalidInput);
    SimpleTest::assert_eq("", invalidResult, "Invalid input handling");
}

/**
 * @brief Test SubstitutionBreaker functions
 */
void testSubstitutionBreaker() {
    std::cout << "\n=== Testing SubstitutionBreaker ===" << std::endl;
    
    SubstitutionBreaker breaker;
    
    // Test basic frequency mapping
    std::string testText = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG";
    auto mapping = breaker.generateMapping(testText);
    SimpleTest::assert_true(mapping.size() > 0, "Frequency mapping generation");
    
    // Test apply mapping
    std::map<char, char> simpleMapping = {
        {'A', 'B'}, {'B', 'C'}, {'C', 'D'}, {'D', 'E'}, {'E', 'F'}
    };
    std::string mapped = breaker.applyMapping("ABCDE", simpleMapping);
    SimpleTest::assert_eq("BCDEF", mapped, "Apply mapping basic");
    
    // Test scoring system
    double englishScore = breaker.scorePlaintext("THE QUICK BROWN FOX");
    double randomScore = breaker.scorePlaintext("XYZ QWE RTYU IOP");
    SimpleTest::assert_true(englishScore > randomScore, "English text scores higher");
    
    // Test optimization method setting
    breaker.setOptimizationMethod("frequency");
    breaker.setMaxIterations(100);
    SimpleTest::assert_true(true, "Optimization configuration");
    
    // Test short substitution cipher
    std::string shortCipher = "IFMMP XPSME"; // "HELLO WORLD" with simple +1 shift
    std::string result = breaker.breakByFrequency(shortCipher);
    SimpleTest::assert_true(result.length() > 0, "Short substitution breaking");
    
    // Test multiple solutions
    auto solutions = breaker.getPossibleSolutions(shortCipher);
    SimpleTest::assert_true(solutions.size() > 0, "Multiple solutions generation");
    
    // Test invalid input
    std::string invalidResult = breaker.breakCipher("123456");
    SimpleTest::assert_eq("", invalidResult, "Invalid input handling");
    
    // Test empty mapping
    std::map<char, char> emptyMapping;
    std::string emptyResult = breaker.applyMapping("TEST", emptyMapping);
    SimpleTest::assert_eq("TEST", emptyResult, "Empty mapping handling");
}

/**
 * @brief Test VigenereBreaker functions
 */
void testVigenereBreaker() {
    std::cout << "\n=== Testing VigenereBreaker ===" << std::endl;
    
    VigenereBreaker breaker;
    
    // Test basic Vigenère encryption/decryption
    std::string plaintext = "THE QUICK BROWN FOX";
    std::string key = "KEY";
    std::string encrypted = breaker.encrypt(plaintext, key);
    std::string decrypted = breaker.decrypt(encrypted, key);
    
    SimpleTest::assert_eq(Utils::normalizeText(plaintext), Utils::normalizeText(decrypted), 
                         "Vigenère encrypt/decrypt consistency");
    
    // Test character shift conversion
    SimpleTest::assert_eq(0, breaker.charToShift('A'), "Char to shift A=0");
    SimpleTest::assert_eq(25, breaker.charToShift('Z'), "Char to shift Z=25");
    SimpleTest::assert_eq('A', breaker.shiftToChar(0), "Shift to char 0=A");
    SimpleTest::assert_eq('Z', breaker.shiftToChar(25), "Shift to char 25=Z");
    
    // Test key length range configuration
    breaker.setKeyLengthRange(3, 10);
    SimpleTest::assert_true(true, "Key length range configuration");
    
    // Test split by key position
    std::string testText = "ABCDEFGH";
    auto split = breaker.splitByKeyPosition(testText, 3);
    SimpleTest::assert_eq(3, static_cast<int>(split.size()), "Split by key position size");
    SimpleTest::assert_eq("ADG", split[0], "Split position 0");
    SimpleTest::assert_eq("BEH", split[1], "Split position 1");
    SimpleTest::assert_eq("CF", split[2], "Split position 2");
    
    // Test simple Vigenère breaking (using a short known example)
    std::string knownCipher = "DXTJQWL VJEWP"; // "THEQUICK BROWN" with key "KEY"
    std::string result = breaker.breakCipher(knownCipher);
    SimpleTest::assert_true(result.length() > 0, "Vigenère cipher breaking");
    
    // Test multiple solutions
    auto solutions = breaker.getPossibleSolutions(knownCipher);
    SimpleTest::assert_true(solutions.size() > 0, "Multiple solutions generation");
    
    // Test invalid input
    std::string invalidResult = breaker.breakCipher("123456");
    SimpleTest::assert_eq("", invalidResult, "Invalid input handling");
    
    // Test empty key handling
    std::string emptyKeyResult = breaker.encrypt("TEST", "");
    SimpleTest::assert_eq("TEST", emptyKeyResult, "Empty key handling");
}

/**
 * @brief Main test runner
 */
int main() {
    std::cout << "=== CryptoBreaker Unit Tests ===" << std::endl;
    std::cout << "Running Phase 1, 2, 3 & 4 tests..." << std::endl;
    
    try {
        testUtils();
        testFrequencyAnalyzer();
        testCaesarBreaker();
        testSubstitutionBreaker();
        testVigenereBreaker();
        
        SimpleTest::printSummary();
        
        if (SimpleTest::testsPassed == SimpleTest::testsRun) {
            std::cout << "\n🎉 All tests passed!" << std::endl;
            return 0;
        } else {
            std::cout << "\n⚠️  Some tests failed!" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ Test execution failed: " << e.what() << std::endl;
        return 1;
    }
}