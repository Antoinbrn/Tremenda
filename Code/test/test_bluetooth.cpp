/**
 * Unit tests for Bluetooth A2DP
 */

#include <Arduino.h>
#include <unity.h>
#include "audio/BluetoothA2DP.h"

// Mock A2DP state machine
enum A2DPState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    DISCONNECTING
};

class MockA2DP {
public:
    A2DPState state = DISCONNECTED;
    String deviceName;
    uint8_t volume = 50;
    bool autoReconnect = true;
    String lastPlayedFile;

    bool begin(const char* name) {
        deviceName = name;
        state = CONNECTING;
        return true;
    }

    void loop() {
        if (state == CONNECTING) {
            // Simulate connection
            state = CONNECTED;
        }
    }

    bool playFile(const char* filepath) {
        if (state != CONNECTED) return false;
        lastPlayedFile = filepath;
        return true;
    }

    void pause() {}
    void resume() {}
    void stop() { lastPlayedFile = ""; }
    bool isPlaying() const { return lastPlayedFile.length() > 0; }

    void setVolume(uint8_t v) { volume = v; }
    uint8_t getVolume() const { return volume; }

    bool isConnected() const { return state == CONNECTED; }

    void disconnect() { state = DISCONNECTED; }
    void reconnect() { state = CONNECTING; }
};

void test_a2dp_initialization() {
    MockA2DP a2dp;
    bool result = a2dp.begin("TestDevice");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_STRING("TestDevice", a2dp.deviceName.c_str());
}

void test_volume_control() {
    MockA2DP a2dp;
    a2dp.begin("Test");

    a2dp.setVolume(0);
    TEST_ASSERT_EQUAL(0, a2dp.getVolume());

    a2dp.setVolume(100);
    TEST_ASSERT_EQUAL(100, a2dp.getVolume());

    a2dp.setVolume(150); // Should clamp
    TEST_ASSERT_EQUAL(100, a2dp.getVolume());
}

void test_connection_state_machine() {
    MockA2DP a2dp;
    a2dp.begin("Test");

    TEST_ASSERT_EQUAL(DISCONNECTED, a2dp.state); // Before begin
    a2dp.begin("Test");
    TEST_ASSERT_EQUAL(CONNECTING, a2dp.state);

    a2dp.loop(); // Simulate connection
    TEST_ASSERT_EQUAL(CONNECTED, a2dp.state);
    TEST_ASSERT_TRUE(a2dp.isConnected());

    a2dp.disconnect();
    TEST_ASSERT_EQUAL(DISCONNECTED, a2dp.state);
    TEST_ASSERT_FALSE(a2dp.isConnected());
}

void test_playback_control() {
    MockA2DP a2dp;
    a2dp.begin("Test");
    a2dp.loop();

    // Play
    bool result = a2dp.playFile("/Music/song.mp3");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(a2dp.isPlaying());
    TEST_ASSERT_EQUAL_STRING("/Music/song.mp3", a2dp.lastPlayedFile.c_str());

    // Pause
    a2dp.pause();
    // isPlaying would still return true in mock, real impl would track state

    // Stop
    a2dp.stop();
    TEST_ASSERT_FALSE(a2dp.isPlaying());
    TEST_ASSERT_EQUAL_STRING("", a2dp.lastPlayedFile.c_str());
}

void test_play_without_connection() {
    MockA2DP a2dp;
    a2dp.begin("Test");
    // Don't call loop(), so still CONNECTING

    bool result = a2dp.playFile("/Music/song.mp3");
    TEST_ASSERT_FALSE(result); // Should fail when not connected
}

void test_auto_reconnect() {
    MockA2DP a2dp;
    a2dp.begin("Test");
    a2dp.loop();
    TEST_ASSERT_TRUE(a2dp.isConnected());

    a2dp.disconnect();
    TEST_ASSERT_FALSE(a2dp.isConnected());

    // Simulate reconnect logic
    if (!a2dp.isConnected() && a2dp.autoReconnect) {
        a2dp.reconnect();
    }
    TEST_ASSERT_EQUAL(CONNECTING, a2dp.state);

    a2dp.loop();
    TEST_ASSERT_EQUAL(CONNECTED, a2dp.state);
}

void test_metadata_callback() {
    // Test metadata parsing
    const char* testMetadata[][2] = {
        {"TIT2", "Song Title"},
        {"TPE1", "Artist Name"},
        {"TALB", "Album Name"},
        {"Title", "Alt Title"},
        {"Artist", "Alt Artist"},
        {"Album", "Alt Album"}
    };

    String title, artist, album;
    for (auto m : testMetadata) {
        if (strcmp(m[0], "TIT2") == 0 || strcmp(m[0], "Title") == 0) {
            title = m[1];
        } else if (strcmp(m[0], "TPE1") == 0 || strcmp(m[0], "Artist") == 0) {
            artist = m[1];
        } else if (strcmp(m[0], "TALB") == 0 || strcmp(m[0], "Album") == 0) {
            album = m[1];
        }
    }

    TEST_ASSERT_EQUAL_STRING("Song Title", title.c_str());
    TEST_ASSERT_EQUAL_STRING("Artist Name", artist.c_str());
    TEST_ASSERT_EQUAL_STRING("Album Name", album.c_str());
}

void test_multiple_devices() {
    // Test device name uniqueness
    MockA2DP a2dp1, a2dp2;
    a2dp1.begin("Device1");
    a2dp2.begin("Device2");

    TEST_ASSERT_EQUAL_STRING("Device1", a2dp1.deviceName.c_str());
    TEST_ASSERT_EQUAL_STRING("Device2", a2dp2.deviceName.c_str());
    TEST_ASSERT_NOT_EQUAL(a2dp1.deviceName, a2dp2.deviceName);
}

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_a2dp_initialization);
    RUN_TEST(test_volume_control);
    RUN_TEST(test_connection_state_machine);
    RUN_TEST(test_playback_control);
    RUN_TEST(test_play_without_connection);
    RUN_TEST(test_auto_reconnect);
    RUN_TEST(test_metadata_callback);
    RUN_TEST(test_multiple_devices);

    UNITY_END();
    return 0;
}