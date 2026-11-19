// Minimal self-contained test runner (no external dependencies).
#include <cmath>
#include <complex>
#include <cstdio>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "AudioClip.h"
#include "Editing.h"
#include "Editor.h"
#include "Effects.h"
#include "FFT.h"
#include "Spectrogram.h"

namespace {

constexpr double kPi = 3.14159265358979323846;
int failures = 0;

#define CHECK(cond)                                                                   \
    do {                                                                              \
        if (!(cond)) {                                                                \
            std::cerr << "  FAILED: " #cond " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            ++failures;                                                               \
        }                                                                             \
    } while (0)

#define CHECK_NEAR(a, b, eps) CHECK(std::fabs((a) - (b)) <= (eps))

AudioClip sine(double freq, double seconds, uint32_t rate = 44100, double amp = 0.5) {
    AudioClip clip(rate, 1, static_cast<std::size_t>(seconds * rate));
    for (std::size_t i = 0; i < clip.numFrames(); ++i)
        clip.channel(0)[i] = static_cast<float>(amp * std::sin(2 * kPi * freq * i / rate));
    return clip;
}

double rms(const std::vector<float>& v, std::size_t from = 0) {
    double sum = 0;
    for (std::size_t i = from; i < v.size(); ++i) sum += v[i] * v[i];
    return std::sqrt(sum / (v.size() - from));
}

// ------------------------------------------------------------------ tests

void testFftMatchesNaiveDft() {
    const std::size_t n = 64;
    std::vector<std::complex<double>> input(n);
    for (std::size_t i = 0; i < n; ++i) input[i] = {std::sin(i * 0.3) + 0.2 * i, std::cos(i * 0.7)};

    std::vector<std::complex<double>> fast = input;
    fft::transform(fast);

    for (std::size_t k = 0; k < n; ++k) {
        std::complex<double> sum = 0;
        for (std::size_t j = 0; j < n; ++j) sum += input[j] * std::polar(1.0, -2 * kPi * k * j / n);
        CHECK_NEAR(std::abs(fast[k] - sum), 0.0, 1e-9);
    }
}

void testFftInverse() {
    std::vector<std::complex<double>> a(128);
    for (std::size_t i = 0; i < a.size(); ++i) a[i] = std::sin(i * 0.1);
    auto b = a;
    fft::transform(b);
    fft::transform(b, true);
    for (std::size_t i = 0; i < a.size(); ++i) CHECK_NEAR(std::abs(a[i] - b[i]), 0.0, 1e-9);
}

void testFftRejectsNonPowerOfTwo() {
    std::vector<std::complex<double>> a(100);
    bool threw = false;
    try { fft::transform(a); } catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
}

void testWavRoundTrip() {
    AudioClip clip(22050, 2, 1000);
    for (std::size_t i = 0; i < 1000; ++i) {
        clip.channel(0)[i] = static_cast<float>(std::sin(i * 0.05) * 0.8);
        clip.channel(1)[i] = static_cast<float>(-0.3);
    }
    const std::string path = "test_roundtrip.wav";
    clip.saveWav(path);
    AudioClip loaded = AudioClip::loadWav(path);
    std::remove(path.c_str());

    CHECK(loaded.sampleRate() == 22050);
    CHECK(loaded.numChannels() == 2);
    CHECK(loaded.numFrames() == 1000);
    for (std::size_t i = 0; i < 1000; ++i) {
        CHECK_NEAR(loaded.channel(0)[i], clip.channel(0)[i], 1.0 / 16000);
        CHECK_NEAR(loaded.channel(1)[i], clip.channel(1)[i], 1.0 / 16000);
    }
}

void testLoadRejectsNonWav() {
    const std::string path = "not_a_wav.wav";
    { std::FILE* f = std::fopen(path.c_str(), "wb"); std::fputs("hello world, definitely not audio", f); std::fclose(f); }
    bool threw = false;
    try { AudioClip::loadWav(path); } catch (const std::runtime_error&) { threw = true; }
    std::remove(path.c_str());
    CHECK(threw);
}

void testCropAndCut() {
    AudioClip clip = sine(440, 2.0);
    editing::crop(clip, 0.5, 1.5);
    CHECK_NEAR(clip.duration(), 1.0, 1e-3);

    AudioClip clip2 = sine(440, 2.0);
    editing::cut(clip2, 0.5, 1.0);
    CHECK_NEAR(clip2.duration(), 1.5, 1e-3);
}

void testAppendMatchesChannels() {
    AudioClip stereo(44100, 2, 100);
    AudioClip mono = sine(440, 0.01);
    editing::append(stereo, mono);
    CHECK(stereo.numChannels() == 2);
    CHECK(stereo.numFrames() == 100 + mono.numFrames());
    CHECK(stereo.channel(1)[150] == mono.channel(0)[50]);
}

void testReverse() {
    AudioClip clip(44100, 1, 3);
    clip.channel(0) = {0.1f, 0.2f, 0.3f};
    Reverse().apply(clip);
    CHECK(clip.channel(0) == (std::vector<float>{0.3f, 0.2f, 0.1f}));
}

void testNormalize() {
    AudioClip clip = sine(440, 0.5, 44100, 0.1);
    Normalize(0.9).apply(clip);
    CHECK_NEAR(clip.peak(), 0.9, 1e-4);
}

void testSpeedChangesLengthAndPitch() {
    AudioClip clip = sine(440, 1.0);
    Speed(2.0).apply(clip);
    CHECK_NEAR(clip.duration(), 0.5, 1e-3);
    CHECK_NEAR(spectrogram::dominantFrequency(clip), 880.0, 10.0);
}

void testFades() {
    AudioClip clip = sine(440, 1.0);
    for (float& s : clip.channel(0)) s = 1.0f;
    FadeIn(0.1).apply(clip);
    FadeOut(0.1).apply(clip);
    CHECK(clip.channel(0).front() == 0.0f);
    CHECK(clip.channel(0).back() == 0.0f);
    CHECK(clip.channel(0)[clip.numFrames() / 2] == 1.0f);
}

void testEchoExtendsAndRepeats() {
    AudioClip clip(1000, 1, 10);
    clip.channel(0)[0] = 1.0f;  // single click
    Echo(0.1, 0.5).apply(clip);  // 100-sample delay
    CHECK(clip.numFrames() > 10);
    CHECK_NEAR(clip.channel(0)[100], 0.5, 1e-6);
    CHECK_NEAR(clip.channel(0)[200], 0.25, 1e-6);
}

void testLowPassRemovesHighFrequencies() {
    AudioClip low = sine(200, 1.0), high = sine(8000, 1.0);
    BiquadFilter lp(BiquadFilter::Type::LowPass, 1000);
    lp.apply(low);
    lp.apply(high);
    CHECK(rms(low.channel(0), 1000) > 0.3);    // ~0.35 for amplitude 0.5
    CHECK(rms(high.channel(0), 1000) < 0.02);  // strongly attenuated
}

void testHighPassRemovesLowFrequencies() {
    AudioClip low = sine(100, 1.0), high = sine(5000, 1.0);
    BiquadFilter hp(BiquadFilter::Type::HighPass, 2000);
    hp.apply(low);
    hp.apply(high);
    CHECK(rms(low.channel(0), 1000) < 0.01);
    CHECK(rms(high.channel(0), 1000) > 0.3);
}

void testDominantFrequency() {
    CHECK_NEAR(spectrogram::dominantFrequency(sine(1000, 1.0)), 1000.0, 10.0);
    CHECK_NEAR(spectrogram::dominantFrequency(sine(3150, 1.0)), 3150.0, 10.0);
}

void testSpectrogramFile() {
    const std::string path = "test_spec.bmp";
    spectrogram::renderBmp(sine(440, 1.0), path, 512, 200);
    std::FILE* f = std::fopen(path.c_str(), "rb");
    CHECK(f != nullptr);
    if (f) {
        char magic[2] = {};
        CHECK(std::fread(magic, 1, 2, f) == 2);
        CHECK(magic[0] == 'B' && magic[1] == 'M');
        std::fclose(f);
    }
    std::remove(path.c_str());
}

void testEditorCommandsAndUndo() {
    Editor editor;
    std::ostringstream out;
    editor.execute("TONE 440 2", out);
    CHECK(editor.hasClip());
    editor.execute("crop 0 1", out);  // commands are case-insensitive
    CHECK_NEAR(editor.clip().duration(), 1.0, 1e-3);
    editor.execute("UNDO", out);
    CHECK_NEAR(editor.clip().duration(), 2.0, 1e-3);

    std::ostringstream err;
    editor.execute("VOLUME banana", err);
    CHECK(err.str().find("Error") != std::string::npos);
    editor.execute("FOO", err);
    CHECK(err.str().find("Unknown command") != std::string::npos);
    CHECK(!editor.execute("EXIT", out));
}

void testEditorFailedEffectLeavesClipUnchanged() {
    Editor editor;
    std::ostringstream out;
    editor.execute("TONE 440 1 8000", out);
    const auto before = editor.clip().channel(0);
    editor.execute("LOWPASS 6000", out);  // above Nyquist (4000 Hz) -> error
    CHECK(out.str().find("Error") != std::string::npos);
    CHECK(editor.clip().channel(0) == before);
}

}  // namespace

int main() {
    const std::vector<std::pair<const char*, std::function<void()>>> tests = {
        {"FFT matches naive DFT", testFftMatchesNaiveDft},
        {"FFT inverse", testFftInverse},
        {"FFT rejects non power of two", testFftRejectsNonPowerOfTwo},
        {"WAV round trip", testWavRoundTrip},
        {"load rejects non-WAV", testLoadRejectsNonWav},
        {"crop and cut", testCropAndCut},
        {"append matches channels", testAppendMatchesChannels},
        {"reverse", testReverse},
        {"normalize", testNormalize},
        {"speed changes length and pitch", testSpeedChangesLengthAndPitch},
        {"fades", testFades},
        {"echo", testEchoExtendsAndRepeats},
        {"low-pass filter", testLowPassRemovesHighFrequencies},
        {"high-pass filter", testHighPassRemovesLowFrequencies},
        {"dominant frequency", testDominantFrequency},
        {"spectrogram file", testSpectrogramFile},
        {"editor commands and undo", testEditorCommandsAndUndo},
        {"failed effect leaves clip unchanged", testEditorFailedEffectLeavesClipUnchanged},
    };

    for (const auto& [name, test] : tests) {
        const int before = failures;
        test();
        std::cout << (failures == before ? "[PASS] " : "[FAIL] ") << name << '\n';
    }
    std::cout << '\n' << (failures == 0 ? "All tests passed" : std::to_string(failures) + " check(s) failed") << '\n';
    return failures == 0 ? 0 : 1;
}
