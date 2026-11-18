#include "CommandUtils.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace cmd {

Args tokenize(const std::string& line) {
    Args tokens;
    std::string current;
    bool inQuotes = false, hasToken = false;
    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
            hasToken = true;
        } else if (std::isspace(static_cast<unsigned char>(c)) && !inQuotes) {
            if (hasToken) tokens.push_back(current);
            current.clear();
            hasToken = false;
        } else {
            current += c;
            hasToken = true;
        }
    }
    if (inQuotes) throw std::invalid_argument("missing closing quote");
    if (hasToken) tokens.push_back(current);
    return tokens;
}

std::string toUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}

double toNumber(const std::string& s) {
    std::size_t used = 0;
    double v;
    try {
        v = std::stod(s, &used);
    } catch (const std::exception&) {
        throw std::invalid_argument("'" + s + "' is not a number");
    }
    if (used != s.size() || !std::isfinite(v)) throw std::invalid_argument("'" + s + "' is not a number");
    return v;
}

void expectArgs(const Args& args, std::size_t min, std::size_t max, const std::string& usage) {
    if (args.size() < min || args.size() > max) throw std::invalid_argument("usage: " + usage);
}

std::string formatTime(double seconds) {
    std::ostringstream s;
    const int minutes = static_cast<int>(seconds / 60);
    s << minutes << ':' << std::fixed << std::setprecision(2) << std::setw(5) << std::setfill('0')
      << seconds - minutes * 60;
    return s.str();
}

}  // namespace cmd
