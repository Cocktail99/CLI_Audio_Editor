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

// Linear fade in over the first `seconds`.
class FadeIn : public Effect {
public:
    explicit FadeIn(double seconds);
    void apply(AudioClip& clip) const override;
    std::string name() const override { return "fade in"; }
private:
    double seconds_;
};

// Linear fade out over the last `seconds`.
class FadeOut : public Effect {
public:
    explicit FadeOut(double seconds);
    void apply(AudioClip& clip) const override;
    std::string name() const override { return "fade out"; }
private:
    double seconds_;
};

// Feedback echo: y[n] = x[n] + decay * y[n - delay].
// The clip is extended so the echo tail is not cut off.
class Echo : public Effect {
public:
    Echo(double delaySeconds, double decay);
    void apply(AudioClip& clip) const override;
    std::string name() const override { return "echo"; }
private:
    double delay_;
    double decay_;
};

// Changes playback speed (and therefore pitch) by resampling with
// linear interpolation. factor 2.0 = twice as fast, 0.5 = half speed.
class Speed : public Effect {
public:
    explicit Speed(double factor);
    void apply(AudioClip& clip) const override;
    std::string name() const override { return "speed"; }
private:
    double factor_;
};

// Second-order IIR (biquad) filter, using the coefficients from
// Robert Bristow-Johnson's "Audio EQ Cookbook".
class BiquadFilter : public Effect {
public:
    enum class Type { LowPass, HighPass };
    BiquadFilter(Type type, double cutoffHz, double q = 0.7071);
    void apply(AudioClip& clip) const override;
    std::string name() const override;
private:
    Type type_;
    double cutoff_;
    double q_;
};
