#pragma once

#include <string>

#include "AudioClip.h"

// Every effect transforms a clip in place. New effects only need to
// subclass Effect and be registered in the command parser.
class Effect {
public:
    virtual ~Effect() = default;
    virtual void apply(AudioClip& clip) const = 0;
    virtual std::string name() const = 0;
};

// Multiplies every sample by a constant factor.
class Volume : public Effect {
public:
    explicit Volume(double factor);
    void apply(AudioClip& clip) const override;
    std::string name() const override { return "volume"; }
private:
    double factor_;
};

// Scales the clip so its loudest sample reaches the target peak.
class Normalize : public Effect {
public:
    explicit Normalize(double targetPeak = 0.99);
    void apply(AudioClip& clip) const override;
    std::string name() const override { return "normalize"; }
private:
    double target_;
};

// Plays the clip backwards.
class Reverse : public Effect {
public:
    void apply(AudioClip& clip) const override;
    std::string name() const override { return "reverse"; }
};
