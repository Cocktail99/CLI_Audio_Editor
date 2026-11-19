# CLI-Audio-Editor

A command-line audio editor written in C++17, with no external libraries. It loads WAV files, edits them (crop, cut, append, mix), applies effects (echo, fades, speed change, low-pass and high-pass filters, normalization) and analyzes them with an FFT implemented from scratch, which it also uses to render spectrogram images. The result can be saved back to disk.

![Spectrogram of the demo script: an A4 + E5 chord with echo and fade-out](docs/spectrogram.png)

*Spectrogram produced by `examples/demo.txt`: two tones (440 Hz and 660 Hz), echo, then a fade-out. Time runs left to right, frequency (log scale) bottom to top.*

## Features

- **WAV reader and writer built from scratch.** It parses RIFF chunks and reads 8/16/24/32-bit PCM and 32/64-bit float, mono or multichannel. It saves as 16-bit PCM.
- **Editing:** crop, cut, append, mix/overlay, insert silence, convert to mono
- **Effects:** volume, normalize, reverse, fade in/out, feedback echo, speed change (linear-interpolation resampling), and low-pass and high-pass biquad filters
- **Analysis:** an iterative radix-2 Cooley–Tukey FFT, spectrogram rendering to BMP (Hann window, dB scale, log-frequency axis), and dominant-frequency detection
- **Undo** for the last 20 operations
- **Interactive mode or scripts:** type commands live, or run a whole script file

## Build

Requires CMake 3.14+ and a C++17 compiler (GCC, Clang or MSVC).

```bash
cmake -S . -B build
cmake --build build
./build/tests            # run the unit tests
```

## Usage

```bash
./build/audio_editor                       # interactive mode
./build/audio_editor examples/demo.txt     # run a script
```

```
> LOAD song.wav
Loaded song.wav (3:12.40, 44100 Hz, 2 ch)
> CROP 30 60
Cropped to 0:30.00
> LOWPASS 800
Applied low-pass filter
> FADEOUT 3
Applied fade out
> SPECTROGRAM song.bmp
Spectrogram written to song.bmp
> SAVE clip.wav
Saved clip.wav
```

### Commands

| Command | Description |
|---|---|
| `LOAD <file.wav>` | Open a WAV file |
| `SAVE [file.wav]` | Save (to the current file if no name is given) |
| `TONE <hz> <seconds> [rate]` | Generate a sine wave |
| `INFO` | Duration, sample rate, channels, peak level |
| `CROP <start> <end>` | Keep only this part (seconds) |
| `CUT <start> <end>` | Remove this part |
| `APPEND <file.wav>` | Add another file at the end |
| `MIX <file.wav> [at]` | Overlay another file, optionally starting at `at` seconds |
| `SILENCE <at> <length>` | Insert silence |
| `MONO` | Mix all channels down to one |
| `VOLUME <factor>` | Scale the volume (0.5 = half) |
| `NORMALIZE [peak]` | Scale so the loudest sample hits `peak` (default 0.99) |
| `REVERSE` | Play backwards |
| `SPEED <factor>` | Change speed and pitch (2 = twice as fast) |
| `FADEIN <s>` / `FADEOUT <s>` | Linear fades |
| `ECHO <delay> <decay>` | Feedback echo, e.g. `ECHO 0.3 0.5` |
| `LOWPASS <hz>` / `HIGHPASS <hz>` | Second-order biquad filters |
| `SPECTROGRAM <file.bmp> [fft]` | Render a spectrogram image |
| `PEAKFREQ` | Print the dominant frequency |
| `UNDO`, `HELP`, `EXIT` | |

Commands are case-insensitive, `#` starts a comment, and file names with spaces can be quoted: `LOAD "my song.wav"`.

## How it works

- **`AudioClip`** stores samples as `float`s in [-1, 1], one vector per channel. That makes every effect independent of the file's original bit depth.
- **Effects** subclass a small `Effect` interface (`apply(AudioClip&)`), so adding a new effect means one class plus one line in the command parser.
- **FFT:** the input is first permuted into bit-reversed order. The transform is then built up from size 2 to size N with butterfly operations, which is O(N log N) instead of the naive O(N²) DFT. The tests check it against a naive DFT.
- **Spectrogram:** a short-time Fourier transform. The signal is split into overlapping windows, each multiplied by a Hann window to reduce spectral leakage, then FFT'd. Magnitudes are converted to dB and mapped to a color scale.
- **Filters:** biquad IIR filters using the coefficients from Robert Bristow-Johnson's *Audio EQ Cookbook*, run in Direct Form I.
- **Echo:** `y[n] = x[n] + decay · y[n − delay]`, with the clip extended until the echo tail drops below −60 dB.
- **Undo:** each editing command saves a snapshot first. If the command fails, the snapshot is restored, so a bad command never corrupts the clip.

## Project structure

```
include/                 headers
src/
  AudioClip.cpp          clip container, peak, mono mixdown
  WavReader.cpp          WAV parsing (RIFF chunks, all sample formats)
  WavWriter.cpp          16-bit PCM WAV output
  Crop.cpp Cut.cpp Append.cpp Mix.cpp Silence.cpp Mono.cpp
                         editing operations
  Volume.cpp Normalize.cpp Reverse.cpp Fade.cpp Echo.cpp Speed.cpp BiquadFilter.cpp
                         effects
  FFT.cpp                radix-2 Cooley-Tukey FFT
  Spectrogram.cpp        STFT, spectrogram rendering, dominant frequency
  ColorMap.cpp BmpWriter.cpp
                         image output
  CommandUtils.cpp       tokenizing and argument parsing
  Editor.cpp             command dispatch and undo
  FileCommands.cpp EditCommands.cpp EffectCommands.cpp AnalysisCommands.cpp Help.cpp
                         command handlers
  main.cpp               interactive / script loop
tests/                   unit tests (no framework needed)
examples/                demo script
```
