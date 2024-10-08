/*******************************************************************************************
*
*   WordGrid
*   Simple Word Puzzle Game
*   (C) Harald Scheirich 2024
*   WordGrid is is licensed under an unmodified zlib/libpng license see LICENSE
*
********************************************************************************************/

#pragma once

#include "raylib.h"

#include <stdlib.h>

enum class LinebreakMode {
    CR,
    CRLF,
    LF
};

struct Dictionary {
    int *words = nullptr; // 0 separated "strings" of codepoints for the words
    int words_size = 0;
    int word_count = 0;
    LinebreakMode mode = LinebreakMode::CRLF;
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
    result.mode = (data[i] == CR) ? LinebreakMode::CR : LinebreakMode::LF;
    if (data[i + 1] == LF) {
        skip_crlf = 2;
        result.mode = LinebreakMode::CRLF;
    }

    TraceLog(LOG_INFO, "Linefeed Mode %d", static_cast<int>(result.mode));

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
            words[i] = 0;
        }
    }

    result.word_count = result.word_count / skip_crlf;
    result.words = words;
    result.words_size = codepoint_count + 1;

    UnloadFileText(data);

    TraceLog(LOG_INFO, "Loaded %i words from file %s", result.word_count, filename);

    int start = 0;
    int end = 0;
    for (int i = 0; i < 10; ++i) {
        while (words[end] != 0) end++;
        
        TraceLog(LOG_DEBUG, "Word %d: [%c,%c,%c,%c,%c]", i, words[start], words[start+1], words[start+2], words[start+3], words[start+4]);
        TraceLog(LOG_DEBUG, "Data %d: [%d,%d,%d,%d,%d]", i, words[start], words[start + 1], words[start + 2], words[start + 3], words[start + 4]);
        start = end+1;
        end = start;
    }
    
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
    int index = -2;
    do {
        index += 2;
        draw -= dict->distribution[index + 1];
    } while (draw > 0 && index < dict->distribution_count);

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
            TraceLog(LOG_DEBUG, "FOUND: [%c,%c,%c,%c,%c]", codepoints[0], codepoints[1], codepoints[2], codepoints[3], codepoints[4]);
            return true;
        }
        // Forward to next 0
        while (dictionary->words[index++] != 0) {}
    }
    TraceLog(LOG_DEBUG, "Not Found: [%c,%c,%c,%c,%c]", codepoints[0], codepoints[1], codepoints[2], codepoints[3], codepoints[4]);
    return false;
}

void dictionary_unload(Dictionary *dictionary)
{
    free(dictionary->words);
    free(dictionary->distribution);
    *dictionary = { 0 };
}