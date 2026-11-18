#include <cmath>
#include <iomanip>
#include <ostream>
#include <stdexcept>

#include "Editor.h"

using namespace cmd;

namespace {
constexpr double kPi = 3.14159265358979323846;
}

void Editor::handleLoad(const Args& args, std::ostream& out) {
    expectArgs(args, 1, 1, "LOAD <file.wav>");
    replaceClip(AudioClip::loadWav(args[0]));
    currentPath_ = args[0];
    out << "Loaded " << args[0] << " (" << formatTime(clip_->duration()) << ", " << clip_->sampleRate()
        << " Hz, " << clip_->numChannels() << " ch)\n";
}

void Editor::handleSave(const Args& args, std::ostream& out) {
    expectArgs(args, 0, 1, "SAVE [file.wav]");
    const AudioClip& clip = requireClip();
    const std::string path = args.empty() ? currentPath_ : args[0];
    if (path.empty()) throw std::invalid_argument("usage: SAVE <file.wav>");
    clip.saveWav(path);
    currentPath_ = path;
    out << "Saved " << path << '\n';
}

// Generates a sine wave; handy for testing without any audio files.
void Editor::handleTone(const Args& args, std::ostream& out) {
    expectArgs(args, 2, 3, "TONE <frequency Hz> <seconds> [sample rate]");
    const double freq = toNumber(args[0]);
    const double seconds = toNumber(args[1]);
    const double rate = args.size() > 2 ? toNumber(args[2]) : 44100;
    if (freq <= 0 || seconds <= 0 || rate < 1000) throw std::invalid_argument("invalid tone parameters");

    AudioClip tone(static_cast<uint32_t>(rate), 1, static_cast<std::size_t>(seconds * rate));
    auto& ch = tone.channel(0);
    for (std::size_t i = 0; i < ch.size(); ++i)
        ch[i] = static_cast<float>(0.5 * std::sin(2 * kPi * freq * i / rate));

    replaceClip(std::move(tone));
    currentPath_.clear();
    out << "Generated a " << freq << " Hz tone (" << formatTime(seconds) << ")\n";
}

void Editor::handleInfo(const Args& args, std::ostream& out) {
    expectArgs(args, 0, 0, "INFO");
    const AudioClip& clip = requireClip();
    const float peak = clip.peak();
    out << "Duration:    " << formatTime(clip.duration()) << " (" << clip.numFrames() << " frames)\n"
        << "Sample rate: " << clip.sampleRate() << " Hz\n"
        << "Channels:    " << clip.numChannels() << '\n'
        << "Peak:        " << std::fixed << std::setprecision(1)
        << (peak > 0 ? 20 * std::log10(peak) : -INFINITY) << " dBFS\n"
        << std::defaultfloat;
}

void Editor::handleUndo(const Args& args, std::ostream& out) {
    expectArgs(args, 0, 0, "UNDO");
    if (undoStack_.empty()) throw std::runtime_error("nothing to undo");
    clip_ = std::move(undoStack_.back());
    undoStack_.pop_back();
    out << "Undone\n";
}
