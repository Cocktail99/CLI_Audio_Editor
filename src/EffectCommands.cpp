#include <memory>
#include <ostream>

#include "Editor.h"
#include "Effects.h"

using namespace cmd;

namespace {

// Builds the effect for a command name, or returns nullptr if the
// name is not an effect.
std::unique_ptr<Effect> makeEffect(const std::string& name, const Args& args) {
    if (name == "REVERSE") {
        expectArgs(args, 0, 0, "REVERSE");
        return std::make_unique<Reverse>();
    }
    if (name == "VOLUME") {
        expectArgs(args, 1, 1, "VOLUME <factor>");
        return std::make_unique<Volume>(toNumber(args[0]));
    }
    if (name == "NORMALIZE") {
        expectArgs(args, 0, 1, "NORMALIZE [target peak 0-1]");
        return std::make_unique<Normalize>(args.empty() ? 0.99 : toNumber(args[0]));
    }
    if (name == "SPEED") {
        expectArgs(args, 1, 1, "SPEED <factor>");
        return std::make_unique<Speed>(toNumber(args[0]));
    }
    if (name == "FADEIN") {
        expectArgs(args, 1, 1, "FADEIN <seconds>");
        return std::make_unique<FadeIn>(toNumber(args[0]));
    }
    if (name == "FADEOUT") {
        expectArgs(args, 1, 1, "FADEOUT <seconds>");
        return std::make_unique<FadeOut>(toNumber(args[0]));
    }
    if (name == "ECHO") {
        expectArgs(args, 2, 2, "ECHO <delay seconds> <decay 0-1>");
        return std::make_unique<Echo>(toNumber(args[0]), toNumber(args[1]));
    }
    if (name == "LOWPASS") {
        expectArgs(args, 1, 1, "LOWPASS <cutoff Hz>");
        return std::make_unique<BiquadFilter>(BiquadFilter::Type::LowPass, toNumber(args[0]));
    }
    if (name == "HIGHPASS") {
        expectArgs(args, 1, 1, "HIGHPASS <cutoff Hz>");
        return std::make_unique<BiquadFilter>(BiquadFilter::Type::HighPass, toNumber(args[0]));
    }
    return nullptr;
}

}  // namespace

void Editor::handleEffect(const std::string& name, const Args& args, std::ostream& out) {
    std::unique_ptr<Effect> effect = makeEffect(name, args);
    if (!effect) {
        out << "Unknown command '" << name << "' (type HELP)\n";
        return;
    }
    modify([&](AudioClip& c) { effect->apply(c); });
    out << "Applied " << effect->name() << '\n';
}
