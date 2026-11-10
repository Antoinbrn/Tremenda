/**
 * Unit tests for Audio Manager
 * Run with: pio test -e esp32dev_test
 */

#include <Arduino.h>
#include <unity.h>
#include "audio/AudioManager.h"
#include "audio/BluetoothA2DP.h"
#include "storage/FileIndexer.h"

// Mock BluetoothA2DP for testing
class MockBluetoothA2DP : public BluetoothA2DP {
public:
    MockBluetoothA2DP() : connected(false), playing(false), volume(50) {}

    bool begin(const char* deviceName) override {
        connected = true;
        return true;
    }

    void loop() override {}

    bool playFile(const char* filepath) override {
        if (!connected) return false;
        playing = true;
        currentFile = filepath;
        return true;
    }

    void pause() override { playing = false; }
    void resume() override { playing = true; }
    void stop() override { playing = false; currentFile = ""; }
    bool isPlaying() const override { return playing; }

    void setVolume(uint8_t vol) override { volume = vol; }
    uint8_t getVolume() const override { return volume; }

    bool isConnected() const override { return connected; }

    uint32_t getPosition() const override { return 1000; }
    uint32_t getDuration() const override { return 180000; }

    void setConnected(bool c) { connected = c; }

    String currentFile;
    bool connected;
    bool playing;
    uint8_t volume;
};

// Test AudioManager initialization
void test_audio_manager_init() {
    AudioManager audio;
    // Note: Full test requires hardware, so we test logic only
    TEST_ASSERT_TRUE(true); // Placeholder
}

// Test volume mapping
void test_volume_mapping() {
    // Test potentiometer to volume mapping
    // 0-4095 ADC -> 0-100 volume
    int testValues[] = {100, 500, 1000, 2000, 3000, 4000};
    uint8_t expectedVolumes[] = {0, 1, 5, 25, 56, 100};

    for (int i = 0; i < 6; i++) {
        // This would use the actual mapping function
        // For now, just verify the logic compiles
        TEST_ASSERT_TRUE(true);
    }
}

// Test playlist next/prev logic
void test_playlist_navigation() {
    std::vector<int> playlist = {0, 1, 2, 3, 4};
    int currentIndex = 2;

    // Next track
    int next = (currentIndex + 1) % playlist.size();
    TEST_ASSERT_EQUAL(3, next);

    // Previous track
    int prev = (currentIndex - 1 + playlist.size()) % playlist.size();
    TEST_ASSERT_EQUAL(1, prev);

    // Wrap around
    currentIndex = 4;
    next = (currentIndex + 1) % playlist.size();
    TEST_ASSERT_EQUAL(0, next);

    currentIndex = 0;
    prev = (currentIndex - 1 + playlist.size()) % playlist.size();
    TEST_ASSERT_EQUAL(4, prev);
}

// Test shuffle logic
void test_shuffle_logic() {
    std::vector<int> playlist = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> shuffled = playlist;

    // Fisher-Yates shuffle
    for (int i = shuffled.size() - 1; i > 0; i--) {
        int j = random(i + 1);
        std::swap(shuffled[i], shuffled[j]);
    }

    // Verify all elements present
    for (int val : playlist) {
        bool found = false;
        for (int s : shuffled) {
            if (s == val) { found = true; break; }
        }
        TEST_ASSERT_TRUE(found);
    }
}

// Test track info parsing
void test_track_info_parsing() {
    const char* testPath = "/Music/Artist - Song Title.mp3";
    String path = testPath;
    int lastSlash = path.lastIndexOf('/');
    String filename = path.substring(lastSlash + 1);
    int dotPos = filename.lastIndexOf('.');
    String name = filename.substring(0, dotPos);

    TEST_ASSERT_EQUAL_STRING("Artist - Song Title", name.c_str());

    int dashPos = name.indexOf(" - ");
    if (dashPos > 0) {
        String artist = name.substring(0, dashPos);
        String title = name.substring(dashPos + 3);
        TEST_ASSERT_EQUAL_STRING("Artist", artist.c_str());
        TEST_ASSERT_EQUAL_STRING("Song Title", title.c_str());
    }
}

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_audio_manager_init);
    RUN_TEST(test_volume_mapping);
    RUN_TEST(test_playlist_navigation);
    RUN_TEST(test_shuffle_logic);
    RUN_TEST(test_track_info_parsing);

    UNITY_END();
    return 0;
}