#pragma once

#include <functional>
#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

#include "AudioClip.h"
#include "CommandUtils.h"

// Holds the editor state (current clip, undo history) and executes
// one text command at a time, e.g. "LOAD song.wav" or "ECHO 0.3 0.5".
class Editor {
public:
    // Executes a single command line and writes feedback to `out`.
    // Returns false when the user asked to exit.
    bool execute(const std::string& line, std::ostream& out);

    bool hasClip() const { return clip_.has_value(); }
    const AudioClip& clip() const;

    static void printHelp(std::ostream& out);

private:
    using Handler = void (Editor::*)(const cmd::Args&, std::ostream&);
    static const std::vector<std::pair<std::string, Handler>>& handlers();

    // File commands (FileCommands.cpp)
    void handleLoad(const cmd::Args& args, std::ostream& out);
    void handleSave(const cmd::Args& args, std::ostream& out);
    void handleTone(const cmd::Args& args, std::ostream& out);
    void handleInfo(const cmd::Args& args, std::ostream& out);
    void handleUndo(const cmd::Args& args, std::ostream& out);

    // Editing commands (EditCommands.cpp)
    void handleCrop(const cmd::Args& args, std::ostream& out);
    void handleCut(const cmd::Args& args, std::ostream& out);
    void handleAppend(const cmd::Args& args, std::ostream& out);
    void handleMix(const cmd::Args& args, std::ostream& out);
    void handleSilence(const cmd::Args& args, std::ostream& out);
    void handleMono(const cmd::Args& args, std::ostream& out);

    // Effect commands (EffectCommands.cpp)
    void handleEffect(const std::string& name, const cmd::Args& args, std::ostream& out);

    // Analysis commands (AnalysisCommands.cpp)
    void handleSpectrogram(const cmd::Args& args, std::ostream& out);
    void handlePeakFreq(const cmd::Args& args, std::ostream& out);

    AudioClip& requireClip();
    void replaceClip(AudioClip clip);  // keeps the old clip on the undo stack
    // Runs an edit on the clip; if it throws, the clip is restored.
    void modify(const std::function<void(AudioClip&)>& edit);

    std::optional<AudioClip> clip_;
    std::string currentPath_;
    std::vector<AudioClip> undoStack_;
    static constexpr std::size_t kMaxUndo = 20;
};
