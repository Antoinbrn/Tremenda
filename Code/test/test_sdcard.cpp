/**
 * Unit tests for SD Card Manager and File Indexer
 */

#include <Arduino.h>
#include <unity.h>
#include "audio/SDCardManager.h"
#include "storage/FileIndexer.h"
#include "utils/StringUtils.h"

using namespace StringUtils;

// Mock file structure for testing
struct MockFileEntry {
    String path;
    String name;
    String folder;
    uint32_t size;
    String extension;
};

std::vector<MockFileEntry> createMockIndex() {
    std::vector<MockFileEntry> files;
    files.push_back({"/Music/Rock/song1.mp3", "song1.mp3", "Rock", 5000000, ".mp3"});
    files.push_back({"/Music/Rock/song2.mp3", "song2.mp3", "Rock", 4500000, ".mp3"});
    files.push_back({"/Music/Pop/song3.mp3", "song3.mp3", "Pop", 4000000, ".mp3"});
    files.push_back({"/Music/Jazz/song4.flac", "song4.flac", "Jazz", 25000000, ".flac"});
    files.push_back({"/Podcasts/ep1.mp3", "ep1.mp3", "Podcasts", 50000000, ".mp3"});
    return files;
}

void test_audio_file_detection() {
    SDCardManager sd;

    // Test supported extensions
    const char* supported[] = {"song.mp3", "track.WAV", "audio.FLAC", "music.m4a", "podcast.aac", "song.ogg"};
    for (const char* f : supported) {
        TEST_ASSERT_TRUE(sd.isAudioFile(f));
    }

    // Test unsupported extensions
    const char* unsupported[] = {"image.jpg", "doc.pdf", "video.mp4", "script.py", "data.txt"};
    for (const char* f : unsupported) {
        TEST_ASSERT_FALSE(sd.isAudioFile(f));
    }
}

void test_file_entry_creation() {
    std::vector<MockFileEntry> files = createMockIndex();

    for (const auto& f : files) {
        TEST_ASSERT_TRUE(f.path.length() > 0);
        TEST_ASSERT_TRUE(f.name.length() > 0);
        TEST_ASSERT_TRUE(f.folder.length() > 0);
        TEST_ASSERT_TRUE(f.size > 0);
        TEST_ASSERT_TRUE(f.extension.length() > 0);
    }
}

void test_folder_grouping() {
    std::vector<MockFileEntry> files = createMockIndex();

    // Group by folder
    std::map<String, std::vector<int>> folderMap;
    for (size_t i = 0; i < files.size(); i++) {
        folderMap[files[i].folder].push_back(i);
    }

    TEST_ASSERT_EQUAL(4, folderMap.size()); // Rock, Pop, Jazz, Podcasts
    TEST_ASSERT_EQUAL(2, folderMap["Rock"].size());
    TEST_ASSERT_EQUAL(1, folderMap["Pop"].size());
    TEST_ASSERT_EQUAL(1, folderMap["Jazz"].size());
    TEST_ASSERT_EQUAL(1, folderMap["Podcasts"].size());
}

void test_search_filtering() {
    std::vector<MockFileEntry> files = createMockIndex();
    std::vector<int> results;

    // Search for "song"
    String query = "song";
    for (size_t i = 0; i < files.size(); i++) {
        String name = files[i].name;
        name.toLowerCase();
        if (name.indexOf(query) >= 0) {
            results.push_back(i);
        }
    }
    TEST_ASSERT_EQUAL(4, results.size()); // song1, song2, song3, song4

    // Search for "rock"
    query = "rock";
    results.clear();
    for (size_t i = 0; i < files.size(); i++) {
        String folder = files[i].folder;
        folder.toLowerCase();
        if (folder.indexOf(query) >= 0) {
            results.push_back(i);
        }
    }
    TEST_ASSERT_EQUAL(2, results.size());

    // Search for "ep"
    query = "ep";
    results.clear();
    for (size_t i = 0; i < files.size(); i++) {
        String name = files[i].name;
        name.toLowerCase();
        if (name.indexOf(query) >= 0) {
            results.push_back(i);
        }
    }
    TEST_ASSERT_EQUAL(1, results.size());
}

void test_index_persistence() {
    // Test JSON serialization/deserialization
    DynamicJsonDocument doc(2048);
    JsonArray array = doc.to<JsonArray>();

    std::vector<MockFileEntry> files = createMockIndex();
    for (const auto& f : files) {
        JsonObject obj = array.createNestedObject();
        obj["path"] = f.path;
        obj["name"] = f.name;
        obj["folder"] = f.folder;
        obj["size"] = f.size;
        obj["ext"] = f.extension;
    }

    String json;
    serializeJson(doc, json);
    TEST_ASSERT_TRUE(json.length() > 0);

    // Deserialize
    DynamicJsonDocument doc2(2048);
    deserializeJson(doc2, json);
    JsonArray array2 = doc2.as<JsonArray>();
    TEST_ASSERT_EQUAL(files.size(), array2.size());
}

void test_natural_sort() {
    std::vector<String> files = {
        "track1.mp3", "track10.mp3", "track2.mp3", "track20.mp3", "track3.mp3"
    };

    std::sort(files.begin(), files.end(), [](const String& a, const String& b) {
        return naturalCompare(a, b) < 0;
    });

    TEST_ASSERT_EQUAL_STRING("track1.mp3", files[0].c_str());
    TEST_ASSERT_EQUAL_STRING("track2.mp3", files[1].c_str());
    TEST_ASSERT_EQUAL_STRING("track3.mp3", files[2].c_str());
    TEST_ASSERT_EQUAL_STRING("track10.mp3", files[3].c_str());
    TEST_ASSERT_EQUAL_STRING("track20.mp3", files[4].c_str());
}

void test_playlist_building() {
    std::vector<MockFileEntry> files = createMockIndex();

    // All Songs playlist
    std::vector<int> allSongs;
    for (size_t i = 0; i < files.size(); i++) {
        allSongs.push_back(i);
    }
    TEST_ASSERT_EQUAL(5, allSongs.size());

    // Folder playlists
    std::map<String, std::vector<int>> folderPlaylists;
    for (size_t i = 0; i < files.size(); i++) {
        if (files[i].folder != "Root") {
            folderPlaylists[files[i].folder].push_back(i);
        }
    }

    TEST_ASSERT_EQUAL(4, folderPlaylists.size());
}

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_audio_file_detection);
    RUN_TEST(test_file_entry_creation);
    RUN_TEST(test_folder_grouping);
    RUN_TEST(test_search_filtering);
    RUN_TEST(test_index_persistence);
    RUN_TEST(test_natural_sort);
    RUN_TEST(test_playlist_building);

    UNITY_END();
    return 0;
}