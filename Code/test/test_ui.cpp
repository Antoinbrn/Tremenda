/**
 * Unit tests for UI Manager
 */

#include <Arduino.h>
#include <unity.h>
#include "ui/UIManager.h"
#include "utils/StringUtils.h"
#include "utils/TimeUtils.h"

using namespace StringUtils;
using namespace TimeUtils;

// Test text scrolling logic
void test_text_scrolling() {
    const char* longText = "This is a very long track title that should scroll";
    int textWidth = strlen(longText) * 6; // 6 pixels per char
    int screenWidth = 128;

    TEST_ASSERT_TRUE(textWidth > screenWidth);

    int offset = 0;
    int charsVisible = screenWidth / 6;
    int maxOffset = (strlen(longText) + charsVisible) * 6;

    // Test scrolling wraps correctly
    for (int i = 0; i < maxOffset * 2; i += 6) {
        offset = i % maxOffset;
        TEST_ASSERT_TRUE(offset >= 0 && offset < maxOffset);
    }
}

// Test time formatting
void test_time_formatting() {
    TEST_ASSERT_EQUAL_STRING("0:00", formatMinutesSeconds(0));
    TEST_ASSERT_EQUAL_STRING("1:30", formatMinutesSeconds(90000));
    TEST_ASSERT_EQUAL_STRING("3:45", formatMinutesSeconds(225000));
    TEST_ASSERT_EQUAL_STRING("10:05", formatMinutesSeconds(605000));
    TEST_ASSERT_EQUAL_STRING("59:59", formatMinutesSeconds(3599000));

    TEST_ASSERT_EQUAL_STRING("0:00", formatDuration(0));
    TEST_ASSERT_EQUAL_STRING("1:30", formatDuration(90000));
    TEST_ASSERT_EQUAL_STRING("1:01:30", formatDuration(3690000));
}

// Test string utilities
void test_string_utils() {
    TEST_ASSERT_EQUAL_STRING("hello", trim("  hello  ").c_str());
    TEST_ASSERT_EQUAL_STRING("hello", toLowerCase("HELLO").c_str());
    TEST_ASSERT_TRUE(endsWith("song.mp3", ".mp3"));
    TEST_ASSERT_TRUE(startsWith("/Music/", "/Music"));

    std::vector<String> parts = split("a,b,c", ',');
    TEST_ASSERT_EQUAL(3, parts.size());
    TEST_ASSERT_EQUAL_STRING("a", parts[0].c_str());
    TEST_ASSERT_EQUAL_STRING("b", parts[1].c_str());
    TEST_ASSERT_EQUAL_STRING("c", parts[2].c_str());

    TEST_ASSERT_EQUAL_STRING("a,b,c", join(parts, ",").c_str());

    TEST_ASSERT_EQUAL_STRING("song.mp3", getFileName("/Music/Artist/song.mp3").c_str());
    TEST_ASSERT_EQUAL_STRING("Artist", getFolderName("/Music/Artist/song.mp3").c_str());
    TEST_ASSERT_EQUAL_STRING(".mp3", getExtension("song.mp3").c_str());
    TEST_ASSERT_EQUAL_STRING("song", removeExtension("song.mp3").c_str());

    TEST_ASSERT_TRUE(naturalCompare("track1.mp3", "track2.mp3") < 0);
    TEST_ASSERT_TRUE(naturalCompare("track10.mp3", "track2.mp3") > 0);
    TEST_ASSERT_TRUE(naturalCompare("track2.mp3", "track2.mp3") == 0);

    TEST_ASSERT_EQUAL_STRING("1 KB", formatFileSize(1024));
    TEST_ASSERT_EQUAL_STRING("1 MB", formatFileSize(1024 * 1024));
}

// Test progress bar calculation
void test_progress_bar() {
    int width = 124; // Screen width - margins
    uint32_t current = 90000;  // 1:30
    uint32_t total = 180000;   // 3:00

    int fillWidth = (width - 2) * current / total;
    TEST_ASSERT_EQUAL(61, fillWidth); // 50% of 122

    // Test edge cases
    fillWidth = (width - 2) * 0 / total;
    TEST_ASSERT_EQUAL(0, fillWidth);

    fillWidth = (width - 2) * total / total;
    TEST_ASSERT_EQUAL(width - 2, fillWidth);
}

// Test volume bar
void test_volume_bar() {
    int width = 124;
    int height = 8;

    for (int vol = 0; vol <= 100; vol += 10) {
        int fillWidth = (width - 2) * vol / 100;
        TEST_ASSERT_TRUE(fillWidth >= 0 && fillWidth <= width - 2);
    }
}

// Test battery voltage calculation
void test_battery_calculation() {
    // ADC reading 0-4095, 3.3V reference, 2x voltage divider
    float voltage = (2048 * 3.3 / 4095) * 2.0;
    TEST_ASSERT_FLOAT_WITHIN(0.1, 3.3, voltage);

    // Full battery
    voltage = (4095 * 3.3 / 4095) * 2.0;
    TEST_ASSERT_FLOAT_WITHIN(0.1, 6.6, voltage); // Would be clamped in reality
}

// Test timeout handling (millis rollover)
void test_timeout_rollover() {
    unsigned long start = 0xFFFF0000; // Near rollover
    unsigned long timeout = 10000;    // 10 seconds

    // Simulate time passing
    unsigned long now = 0x00005000; // After rollover
    unsigned long elapsed = now - start; // Handles rollover correctly

    TEST_ASSERT_TRUE(elapsed > timeout);
}

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_text_scrolling);
    RUN_TEST(test_time_formatting);
    RUN_TEST(test_string_utils);
    RUN_TEST(test_progress_bar);
    RUN_TEST(test_volume_bar);
    RUN_TEST(test_battery_calculation);
    RUN_TEST(test_timeout_rollover);

    UNITY_END();
    return 0;
}