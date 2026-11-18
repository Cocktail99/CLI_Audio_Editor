#include <ostream>

#include "Editing.h"
#include "Editor.h"

using namespace cmd;

void Editor::handleCrop(const Args& args, std::ostream& out) {
    expectArgs(args, 2, 2, "CROP <start s> <end s>");
    const double start = toNumber(args[0]), end = toNumber(args[1]);
    modify([&](AudioClip& c) { editing::crop(c, start, end); });
    out << "Cropped to " << formatTime(clip_->duration()) << '\n';
}

void Editor::handleCut(const Args& args, std::ostream& out) {
    expectArgs(args, 2, 2, "CUT <start s> <end s>");
    const double start = toNumber(args[0]), end = toNumber(args[1]);
    modify([&](AudioClip& c) { editing::cut(c, start, end); });
    out << "Cut, new length " << formatTime(clip_->duration()) << '\n';
}

void Editor::handleAppend(const Args& args, std::ostream& out) {
    expectArgs(args, 1, 1, "APPEND <file.wav>");
    const AudioClip other = AudioClip::loadWav(args[0]);
    modify([&](AudioClip& c) { editing::append(c, other); });
    out << "Appended " << args[0] << ", new length " << formatTime(clip_->duration()) << '\n';
}

void Editor::handleMix(const Args& args, std::ostream& out) {
    expectArgs(args, 1, 2, "MIX <file.wav> [at seconds]");
    const AudioClip other = AudioClip::loadWav(args[0]);
    const double at = args.size() > 1 ? toNumber(args[1]) : 0.0;
    modify([&](AudioClip& c) { editing::mix(c, other, at); });
    out << "Mixed in " << args[0] << '\n';
}

void Editor::handleSilence(const Args& args, std::ostream& out) {
    expectArgs(args, 2, 2, "SILENCE <at s> <length s>");
    const double at = toNumber(args[0]), length = toNumber(args[1]);
    modify([&](AudioClip& c) { editing::insertSilence(c, at, length); });
    out << "Inserted silence\n";
}

void Editor::handleMono(const Args& args, std::ostream& out) {
    expectArgs(args, 0, 0, "MONO");
    modify([](AudioClip& c) { editing::toMono(c); });
    out << "Converted to mono\n";
}
