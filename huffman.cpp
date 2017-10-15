#include "huffman.hpp"

#include <iostream>
#include <map>
#include <queue>

using namespace huffman;

void depth_print(int depth, bool is_right)
{
    for (int i = 0; i < depth; i++) std::cerr << "│   ";

    if (depth >= 0) {
        if (is_right) {
            std::cerr << "└─1─";
        } else {
            std::cerr << "├─0─";
        }
    }
}

node::~node()
{

}

unsigned leaf::get_frequency() const
{
    return frequency;
}

leaf::leaf(char _character, unsigned _frequency) :
    character(_character), frequency(_frequency)
{

}

void leaf::print(int depth, bool is_right) const
{
    depth_print(depth, is_right);

    if (frequency == 0) {
        std::cerr << "EOF";
    } else {
        std::cerr << (unsigned)character;
    }

    std::cerr << " (";

    std::vector<bool>::const_iterator it;
    for (it = new_char.begin(); it != new_char.end(); ++it) std::cerr << *it;

    std::cerr << ")" << std::endl;
}

void leaf::encode(std::vector<bool>& encoding)
{
    new_char = encoding;
}

void leaf::set_frequency(unsigned _frequency)
{
    frequency = _frequency;
}

void leaf::output_encode(std::string& str, unsigned& position)
{
    if (str.empty()) str.push_back((char)0);
    std::vector<bool>::iterator it;
    for (it = new_char.begin(); it != new_char.end(); ++it) {
        if (position == 8) {
            str.push_back((char)0);
            position = 0;
        }
        if (*it == true) {
            str.back() = str.back() | (char)(0x80 >> position);
        }
        position++;
    }
}

void leaf::tree_encode(std::string& out) const
{
    // ( ) and \ are 'special' characters and need to be escaped
    if (character == '(' || character == ')' || character == '\\' || frequency == 0) {
        out.push_back('\\');
    }

    // EOF character
    if (frequency == 0) {
        out.push_back('e'); // \e in the tree string indicates the EOF character

    } else {
        out.push_back(character);
    }
}

eof::eof() : leaf(0, 0)
{

}

branch::branch(std::shared_ptr<node> _left, std::shared_ptr<node> _right) :
    left(_left), right(_right)
{

}

unsigned branch::get_frequency() const
{
    return left->get_frequency() + right->get_frequency();
}

branch::~branch()
{

}

void branch::print(int depth, bool is_right) const
{
    depth_print(depth, is_right);
    std::cerr << "┐" << std::endl;

    left->print(depth + 1);
    right->print(depth + 1, true);
}

void branch::encode(std::vector<bool>& encoding)
{
    std::vector<bool> encoding_old(encoding);

    encoding.push_back(false);
    left->encode(encoding);

    encoding_old.push_back(true);
    right->encode(encoding_old);
}

void branch::tree_encode(std::string& out) const
{
    out.push_back('(');
    left->tree_encode(out);
    right->tree_encode(out);
    out.push_back(')');
}

bool GreaterFrequency::operator()(const std::shared_ptr<node> lhs, const std::shared_ptr<node> rhs) const
{
    return lhs->get_frequency() > rhs->get_frequency();
}

void huffman::encode(const std::string& input, std::string& output, int verbosity)
{
    std::map<char, std::shared_ptr<leaf>> leaves;

    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        if (leaves.find(*it) == leaves.end()) {
            leaves[*it] = std::make_shared<leaf>(*it, 1);
            if (verbosity > 1) {
                std::cerr << "Created Leaf " << (unsigned)*it << " at ";
                std::cerr << leaves[*it] << std::endl;
            }
        } else {
            leaves[*it]->set_frequency(leaves[*it]->get_frequency() + 1);
        }
    }

    if (verbosity > 0) std::cerr << leaves.size() << " leaves created" << std::endl;

    std::priority_queue<std::shared_ptr<node>, std::vector<std::shared_ptr<node>>, GreaterFrequency> frequency_list;

    std::map<char, std::shared_ptr<leaf>>::const_iterator it;
    for (it = leaves.begin(); it != leaves.end(); ++it) {
        frequency_list.push(it->second);
    }

    // Add terminator to frequency list
    std::shared_ptr<leaf> terminator = std::make_shared<eof>();
    frequency_list.push(terminator);

    // When the size of the list is 1, we have the base node of the tree
    while(frequency_list.size() > 1) {
        // Merge the first 2 nodes
        std::shared_ptr<node> left = frequency_list.top();
        frequency_list.pop();
        std::shared_ptr<node> right = frequency_list.top();
        frequency_list.pop();

        if (verbosity > 1) {
            std::cerr << "Combining nodes " << left << " and "  << right << std::endl;
        }

        frequency_list.push(std::make_shared<branch>(left, right));
    }

    // Single pointer left is the root of our tree
    std::shared_ptr<node> tree = frequency_list.top();
    std::vector<bool> encoding;

    tree->encode(encoding);

    if (verbosity > 0) {
        std::cerr << "Output Huffman tree:" << std::endl;
        tree->print();
    }

    if (verbosity > 0) std::cerr << "Encoding Huffman tree leaves" << std::endl;

    std::string output_tree;
    tree->tree_encode(output_tree);

    if (verbosity > 0) std::cerr << "Encoding input" << std::endl;

    output = output_tree;
    output.push_back((char)0); // Push an empty character before encoding

    unsigned position = 0; // First bit of a character
    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        leaves[*it]->output_encode(output, position);
    }
    terminator->output_encode(output, position);
}

void huffman::decode(const std::string& input, std::string& output, int verbosity)
{
    // Match input bracket, to determine where the string ends
    if (input.front() != '(') {
        std::cerr << "Invalid input file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string::const_iterator divider = input.begin();
    match_bracket(input, divider);

    if (divider == input.end()) {
        std::cerr << "Invalid input file" << std::endl;
        exit(EXIT_FAILURE);
    }

    // divider now points to the last bracket in the tree representation
    std::string tree_str(input.begin(), std::next(divider));

    std::shared_ptr<node> tree = tree_from_str(tree_str);

    std::vector<bool> encoding;
    tree->encode(encoding);

    tree->print();
}

// if it == str.end() then the bracket could not be matched
// it should point to the bracket from which to start searching
void huffman::match_bracket(const std::string& str, std::string::const_iterator& it) {
    int depth = 1;
    bool escaped = false;
    while (depth != 0 && it != str.end()) {
        it++;
        if (*it == '(' && !escaped) {
            depth++;
        }
        if (*it == ')' && !escaped) {
            depth--;
        }

        if (*it == '\\') {
            escaped = !escaped;
        } else {
            escaped = false;
        }
    }
}

std::shared_ptr<node> huffman::tree_from_str(const std::string& str)
{
    std::cerr << std::endl;
    std::cerr << str << std::endl;
    std::string::const_iterator divider = str.begin();

    bool enclosed = false;

    if (*divider == '(') {
        // Left hand side starts with a bracket
        match_bracket(str, divider);

        if (divider == std::prev(str.end())) {
            // Remove enclosing brackets
            return tree_from_str(std::string(std::next(str.begin()), std::prev(str.end())));
        }
    } else {
        // Left hand size starts with a character
        if (*divider == '\\') {
            divider++;
        }
    }

    if (divider == std::prev(str.end())) {
        // This is not a branch but a single character
        if (*divider == 'e' && *std::prev(divider) == '\\') {
            std::cerr << "eof" << std::endl;
            return std::make_shared<eof>();
        }
        return std::make_shared<leaf>(*divider, 1);
    }

    // divider now points to the last character in the left hand size of the
    // string representation of the tree

    const std::string& l_str = std::string(str.begin(), std::next(divider));
    const std::string& r_str = std::string(std::next(divider), str.end());

    std::shared_ptr<node> left = tree_from_str(l_str);
    std::shared_ptr<node> right = tree_from_str(r_str);

    return std::make_shared<branch>(left, right);
}
