#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <cstdlib>

#include "huffman.hpp"

// \\ for good measure

void get_stdin(std::string& str)
{
    // Don't skip whitespace
    std::cin >> std::noskipws;

    str.assign((std::istreambuf_iterator<char>(std::cin)),
                std::istreambuf_iterator<char>());
}

int main(int argc, char* argv[])
{
    std::string input;
    get_stdin(input);

    std::string output;

    if (argc < 2) {
        std::cerr << "Please specify either --encode, -e or --decode, -d" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string arg(argv[1]);

    if (arg == "--decode" || arg == "-d") {
        huffman::decode(input, output);
    } else if (arg == "--encode" || arg == "-e") {
        huffman::encode(input, output);
    } else {
        std::cerr << "Unrecognised argument" << std::endl;
    }

    std::cout << output;
}
