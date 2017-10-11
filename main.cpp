#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <cstdlib>
#include <map>

#include "huffman.hpp"

const static std::string decode_arg = "--decode";
const static std::string encode_arg = "--encode";
const static std::string verbose_arg = "--verbose";
const static std::string file_arg = "--file";

const static char decode_short_arg = 'd';
const static char encode_short_arg = 'e';
const static char verbose_short_arg = 'v';
const static char file_short_arg = 'f';

void get_input(std::string& str, std::istream& from)
{
    // Don't skip whitespace
    from >> std::noskipws;

    str.assign((std::istreambuf_iterator<char>(from)),
                std::istreambuf_iterator<char>());
}

int main(int argc, char* argv[])
{
    bool decode = false;
    bool encode = false;

    bool from_file = false;
    std::string file_name;

    int verbosity = 0;

    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        std::map<char, int> short_args;

        bool valid_arg = false;

        // Handle single letter arguments
        if (arg[0] == '-' && arg[1] != '-') {
            valid_arg = true;
            for (int j = 1; j < arg.size(); j++) {
                if (short_args.find(arg[j]) == short_args.end()) {
                    short_args[arg[j]] = 1;
                } else {
                    short_args[arg[j]]++;
                }
            }
        }

        // --decode or -d
        if (arg == decode_arg || short_args.find(decode_short_arg) != short_args.end()) {
            decode = true;
            short_args.erase(decode_short_arg);
            valid_arg = true;
        }

        // --encode or -e
        if (arg == encode_arg || short_args.find(encode_short_arg) != short_args.end()) {
            encode = true;
            short_args.erase(encode_short_arg);
            valid_arg = true;
        }

        // --verbose or -v
        if (arg == verbose_arg || short_args.find(verbose_short_arg) != short_args.end()) {
            if (arg == verbose_arg) {
                verbosity++;
            } else {
                verbosity += short_args[verbose_short_arg];
            }

            short_args.erase(verbose_short_arg);
            valid_arg = true;
        }

        // --file or -f
        if (arg == file_arg || short_args.find(file_short_arg) != short_args.end()) {
            from_file = true;

            // file name is the next argument
            file_name = std::string(argv[++i]);
            short_args.erase(file_short_arg);
            valid_arg = true;
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
    std::string output;

    // - indicates to read from stdin
    if (!from_file || file_name == "-") {
        get_input(input, std::cin);
    } else {
        // Read from file
        std::ifstream file;
        file.open(file_name.c_str());

        get_input(input, file);

        file.close();
    }

    if (encode) {
        huffman::encode(input, output, verbosity);
    } else if (decode) {
        huffman::decode(input, output, verbosity);
    } else {
        std::cerr << "Please specify either --encode, -e or --decode, -d" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << output;
}
