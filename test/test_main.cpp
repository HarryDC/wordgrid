#include "unity.h"
#include "raylib.h"
#include "dictionary.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_dict_should_load(void) {
    Dictionary dict = dictionary_load("resources/dict_test_plain.txt");
    TEST_ASSERT_TRUE(dict.words != nullptr);
    TEST_ASSERT_EQUAL(4, dict.word_count);
    dictionary_unload(&dict);
}

void test_dict_should_load_german(void) {
    Dictionary dict = dictionary_load("resources/dict_test_german.txt");
    TEST_ASSERT_TRUE(dict.words != nullptr);
    TEST_ASSERT_EQUAL(4, dict.word_count);
    dictionary_unload(&dict);
}

void test_dict_find_english(void) {
    Dictionary dict = dictionary_load("resources/dict_test_plain.txt");
    int exists[6] = { 'W', 'O', 'R', 'D', 0, 0};
    TEST_ASSERT_TRUE(dictionary_exists(&dict, exists, 6));
    int notexist[6] = { 'X', 'M', 'R', 'D', 0, 0};
    TEST_ASSERT_FALSE(dictionary_exists(&dict, notexist, 6));
    dictionary_unload(&dict);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dict_should_load);
    RUN_TEST(test_dict_should_load_german);
    RUN_TEST(test_dict_find_english);
    return UNITY_END();
}