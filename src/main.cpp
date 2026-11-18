#include <fstream>
#include <iostream>
#include <string>

#include "Editor.h"

#ifdef _WIN32
#include <io.h>
#define IS_TTY() _isatty(_fileno(stdin))
#else
#include <unistd.h>
#define IS_TTY() isatty(fileno(stdin))
#endif

// Usage:
//   audio_editor               interactive mode
//   audio_editor script.txt    run the commands in a script file
//   audio_editor < script.txt  same, via stdin
int main(int argc, char* argv[]) {
    Editor editor;

    if (argc > 2) {
        std::cerr << "usage: " << argv[0] << " [script.txt]\n";
        return 1;
    }

    std::ifstream script;
    if (argc == 2) {
        script.open(argv[1]);
        if (!script) {
            std::cerr << "cannot open script '" << argv[1] << "'\n";
            return 1;
        }
    }
    std::istream& in = argc == 2 ? static_cast<std::istream&>(script) : std::cin;
    const bool interactive = argc == 1 && IS_TTY();

    if (interactive) std::cout << "CLI Audio Editor - type HELP for commands\n";

    std::string line;
    while (true) {
        if (interactive) std::cout << "> " << std::flush;
        if (!std::getline(in, line)) break;
        if (!editor.execute(line, std::cout)) break;
    }
    return 0;
}
