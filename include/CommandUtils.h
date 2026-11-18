#pragma once

#include <string>
#include <vector>

// Parsing helpers for the command line interface.
namespace cmd {

using Args = std::vector<std::string>;

// Splits a line into words; text inside double quotes stays together
// so file names with spaces work: LOAD "my song.wav"
Args tokenize(const std::string& line);

std::string toUpper(std::string s);

// Parses a number, throwing a readable error for input like "abc".
double toNumber(const std::string& s);

// Checks that the command got between min and max arguments.
void expectArgs(const Args& args, std::size_t min, std::size_t max, const std::string& usage);

// Formats seconds as m:ss.cc
std::string formatTime(double seconds);

}  // namespace cmd
