#pragma once

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
    int* distribution = nullptr;
    int distribution_count = 0;
    int distribution_sum = 0;
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

    int codepoint_count = -1;
    int* words = LoadCodepoints(data, &codepoint_count);

    if (words == nullptr) {
        TraceLog(LOG_FATAL, "LoadCodepoints failed from file %s", filename);
        UnloadFileText(data);
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

    UnloadFileText(data);

    TraceLog(LOG_INFO, "Loaded %i words from file %s", result.word_count, filename);
    
    return result;
}

void dictionary_load_distribution(Dictionary* dict, const char* filename) {
    free(dict->distribution);
    dict->distribution = nullptr;
    dict->distribution_sum = 0;

    char* text = LoadFileText(filename);
    if (text == nullptr) {
        TraceLog(LOG_FATAL, "Could not load distribution file %s", filename);
        return;
    }

    int split_count = 0;
    const char** splits = TextSplit(text, ',', &split_count);

    if (split_count % 2 != 0) {
        TraceLog(LOG_ERROR, "Issue with distribution, it has an odd number of elements %i, %s",split_count, filename);
        split_count -= 1;
    }

    int* distribution = (int*)calloc(split_count, sizeof(int));
    if (distribution == nullptr) {
        TraceLog(LOG_FATAL, "Could not allocate distribution memory");
        return;
    }

    int i = 0;
    while (i < split_count) {
        int codepoint_count = 0;
        distribution[i] = GetCodepointNext(splits[i], &codepoint_count);
        if (codepoint_count == 0) {
            TraceLog(LOG_ERROR, "Did not find valid codepoint, skiping entry %i");
            continue;
        }
        distribution[i + 1] = TextToInteger(splits[i + 1]);
        if (distribution[i + 1] < 1) {
            TraceLog(LOG_ERROR, "Distribution entry needs to be  >0, skiping entry %i");
            continue;
        }
        dict->distribution_sum += distribution[i + 1];
        i += 2;
    }

    UnloadFileText(text);
    TraceLog(LOG_INFO, "Loaded distribution from %s with %i letters", filename, split_count / 2);
    dict->distribution = distribution;
    dict->distribution_count = split_count;
}

int dictionary_get_random_letter(Dictionary* dict) 
{
    int draw = GetRandomValue(0, dict->distribution_sum - 1);
    int index = 0;
    while (draw > 0 && index < dict->distribution_count) {
        draw -= dict->distribution[index + 1];
        index += 2;
    }

    return dict->distribution[index];
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
    free(dictionary->distribution);
    *dictionary = { 0 };
}