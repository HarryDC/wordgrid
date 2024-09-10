#include "raylib.h"

#include <stdlib.h>

enum class Mode {
    CR,
    CRLF,
    LF
};

struct Dictionary {
    int *words = nullptr; // 0 separated "strings" of codepoints for the words
    int words_size = 0;
    int word_count = 0;
    Mode mode = Mode::CRLF;
};

static const char CR = 13;
static const char LF = 10;

Dictionary dictionary_load(const char* filename)
{
    char* data = LoadFileText(filename);
    if (data == nullptr) {
        TraceLog(LOG_FATAL, "Could not find dictionary file %s", filename);
        return Dictionary{0};
    }

    Dictionary result;

    int i = 0;
    while (data[i] != CR && data[i] != LF) {
        ++i;
    }

    int skip_crlf = 1;
    result.mode = (data[i] == CR) ? Mode::CR : Mode::LF;
    if (data[i + 1] == LF) {
        skip_crlf = 2;
        result.mode = Mode::CRLF;
    }

    int target_size = 1024;
    int growth = 1.4;

    int codepoint_count = -1;
    int* words = LoadCodepoints(data, &codepoint_count);

    if (words == nullptr) {
        TraceLog(LOG_FATAL, "LoadCodepoints failed from file %s", filename);
        return Dictionary{ 0 };
    }

    // Allows to add a '0' at the end
    int* temp = (int*)RL_REALLOC(words, (codepoint_count + 1) * sizeof(int));
    words[codepoint_count] = 0;

    // Clear linebreaks 
    for (int i = 0; i < codepoint_count + 1; ++i) {
        if (words[i] == CR || words[i] == LF || words[i] == 0) {
            result.word_count++;
            words[i++] = 0;
        }
    }

    result.word_count = result.word_count / skip_crlf;
    result.words = words;
    result.words_size = codepoint_count + 1;

    TraceLog(LOG_INFO, "Loaded %i words from file %s", result.word_count, filename);

    return result;
}

// Assumes codepoints is null terminated
bool dictionary_exists(Dictionary* dictionary, int* codepoints, int codepoint_count) {
    // linear search, assumes unsorted array
    if (codepoints[codepoint_count - 1] != 0) {
        TraceLog(LOG_ERROR, "codepoints was not null terminated");
        return false;
    }

    int index = 0;
    while (index < dictionary->words_size) {
        int search_index = 0;
        while (dictionary->words[index] == codepoints[search_index] && dictionary->words[index] != 0) {
            search_index++;
            index++;
        }
        if (dictionary->words[index] == 0 && codepoints[search_index] == 0) {
            return true;
        }
        // Forward to next 0
        while (dictionary->words[index++] != 0) {}
    }
    return false;
}

void dictionary_unload(Dictionary *dictionary)
{
    free(dictionary->words);
    *dictionary = { 0 };
}