#include <ostream>

#include "Editor.h"

void Editor::printHelp(std::ostream& out) {
    out << "Files\n"
           "  LOAD <file.wav>                 open a WAV file\n"
           "  SAVE [file.wav]                 save (16-bit PCM)\n"
           "  TONE <hz> <seconds> [rate]      generate a sine wave\n"
           "  INFO                            show duration, rate, channels, peak\n"
           "Editing\n"
           "  CROP <start> <end>              keep only this part (seconds)\n"
           "  CUT <start> <end>               remove this part\n"
           "  APPEND <file.wav>               add another file at the end\n"
           "  MIX <file.wav> [at]             overlay another file\n"
           "  SILENCE <at> <length>           insert silence\n"
           "  MONO                            mix all channels to one\n"
           "Effects\n"
           "  VOLUME <factor>                 e.g. 0.5 = half as loud\n"
           "  NORMALIZE [peak]                scale to peak (default 0.99)\n"
           "  REVERSE                         play backwards\n"
           "  SPEED <factor>                  2 = twice as fast (higher pitch)\n"
           "  FADEIN <seconds> / FADEOUT <seconds>\n"
           "  ECHO <delay> <decay>            e.g. ECHO 0.3 0.5\n"
           "  LOWPASS <hz> / HIGHPASS <hz>    biquad filters\n"
           "Analysis\n"
           "  SPECTROGRAM <image.bmp> [fft]   render a spectrogram image\n"
           "  PEAKFREQ                        print the dominant frequency\n"
           "Other\n"
           "  UNDO, HELP, EXIT\n";
}
