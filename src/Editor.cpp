#include "Editor.h"

#include <ostream>
#include <stdexcept>

const AudioClip& Editor::clip() const {
    if (!clip_) throw std::logic_error("no clip loaded");
    return *clip_;
}

AudioClip& Editor::requireClip() {
    if (!clip_) throw std::runtime_error("no audio loaded (use LOAD <file> or TONE <hz> <seconds>)");
    return *clip_;
}

void Editor::replaceClip(AudioClip clip) {
    if (clip_) {
        undoStack_.push_back(std::move(*clip_));
        if (undoStack_.size() > kMaxUndo) undoStack_.erase(undoStack_.begin());
    }
    clip_ = std::move(clip);
}

void Editor::modify(const std::function<void(AudioClip&)>& edit) {
    AudioClip edited = requireClip();  // work on a copy
    edit(edited);                      // if this throws, the current clip is untouched
    replaceClip(std::move(edited));
}

const std::vector<std::pair<std::string, Editor::Handler>>& Editor::handlers() {
    static const std::vector<std::pair<std::string, Handler>> table = {
        {"LOAD", &Editor::handleLoad},
        {"SAVE", &Editor::handleSave},
        {"TONE", &Editor::handleTone},
        {"INFO", &Editor::handleInfo},
        {"UNDO", &Editor::handleUndo},
        {"CROP", &Editor::handleCrop},
        {"CUT", &Editor::handleCut},
        {"APPEND", &Editor::handleAppend},
        {"MIX", &Editor::handleMix},
        {"SILENCE", &Editor::handleSilence},
        {"MONO", &Editor::handleMono},
        {"SPECTROGRAM", &Editor::handleSpectrogram},
        {"PEAKFREQ", &Editor::handlePeakFreq},
    };
    return table;
}

bool Editor::execute(const std::string& line, std::ostream& out) {
    // Ignore comments, so scripts can be documented.
    const std::string withoutComment = line.substr(0, line.find('#'));

    try {
        cmd::Args tokens = cmd::tokenize(withoutComment);
        if (tokens.empty()) return true;

        const std::string name = cmd::toUpper(tokens[0]);
        const cmd::Args args(tokens.begin() + 1, tokens.end());

        if (name == "EXIT" || name == "QUIT") return false;
        if (name == "HELP") {
            printHelp(out);
            return true;
        }
        for (const auto& [command, handler] : handlers()) {
            if (command == name) {
                (this->*handler)(args, out);
                return true;
            }
        }
        handleEffect(name, args, out);  // reports unknown commands itself
    } catch (const std::exception& e) {
        out << "Error: " << e.what() << '\n';
    }
    return true;
}
