#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <cstdlib>
#include <map>

#include "huffman.hpp"

void get_stdin(std::string& str)
{
    // Don't skip whitespace
    std::cin >> std::noskipws;

    str.assign((std::istreambuf_iterator<char>(std::cin)),
                std::istreambuf_iterator<char>());
}

int main(int argc, char* argv[])
{
    bool decode = false;
    bool encode = false;

    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        std::map<char, int> short_args;

        // Handle single letter arguments
        if (arg[0] == '-' && arg[1] != '-') {
            for (int j = 1; j < arg.size(); j++) {
                if (short_args.find(arg[j]) == short_args.end()) {
                    short_args[arg[j]] = 1;
                } else {
                    short_args[arg[j]]++;
                }
            }
        }

        bool valid_arg = true;

        if (arg == "--decode" || short_args.find('d') != short_args.end()) {
            decode = true;
            short_args.erase('d');
        } else if (arg == "--encode" || short_args.find('e') != short_args.end()) {
            encode = true;
            short_args.erase('e');
        } else {
            valid_arg = false;
        }

        if (!short_args.empty() || !valid_arg) {
            std::cerr << "Unrecognised argument(s):";
            for (const auto& unrecognised : short_args) {
                std::cerr << " -" << unrecognised.first << std::endl;
            }
            if (!valid_arg) std::cerr << " " << arg << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    std::string input;
    get_stdin(input);

    if (encode) {

    } else if (decode) {

    } else {
        std::cerr << "Please specify either --encode, -e or --decode, -d" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string output;

    std::cout << output;
}
