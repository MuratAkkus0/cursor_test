#include <iostream>
#include <cassert>
#include <string>

// Include our headers
#include "../include/Utils.h"
#include "../include/FrequencyAnalyzer.h"

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
    SimpleTest::assert_eq("HELLO", Utils::removeNonAlpha("H3LL0!"), "removeNonAlpha");
    SimpleTest::assert_eq("HELLOWORLD", Utils::removeNonAlpha("Hello, World!"), "removeNonAlpha with punctuation");
    
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
    SimpleTest::assert_true(ic > 0.05 && ic < 0.08, "Index of Coincidence for English");
    
    // Test language detection
    std::string language = analyzer.detectLanguage(englishText);
    SimpleTest::assert_true(language == "english" || language == "unknown", "Language detection");
    
    // Test bigrams
    auto bigrams = analyzer.findCommonBigrams("THETHETHEHE", 3);
    SimpleTest::assert_true(bigrams.size() > 0, "Bigram detection");
    
    // Test trigrams
    auto trigrams = analyzer.findCommonTrigrams("THETHETHETHE", 3);
    SimpleTest::assert_true(trigrams.size() > 0, "Trigram detection");
}

/**
 * @brief Main test runner
 */
int main() {
    std::cout << "=== CryptoBreaker Unit Tests ===" << std::endl;
    std::cout << "Running Phase 1 tests..." << std::endl;
    
    try {
        testUtils();
        testFrequencyAnalyzer();
        
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