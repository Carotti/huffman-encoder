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
        std::cerr << (int)character;
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

branch::branch(node* _left, node* _right) :
    left(_left), right(_right)
{

}

unsigned branch::get_frequency() const
{
    return left->get_frequency() + right->get_frequency();
}

branch::~branch()
{
    delete left;
    delete right;
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

bool GreaterFrequency::operator()(const node* lhs, const node* rhs) const
{
    return lhs->get_frequency() > rhs->get_frequency();
}

void huffman::encode(std::string& input, std::string& output)
{
    std::map<char, leaf*> leaves;

    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        if (leaves.find(*it) == leaves.end()) {
            leaves[*it] = new leaf(*it, 1);
        } else {
            leaves[*it]->set_frequency(leaves[*it]->get_frequency() + 1);
        }
    }

    std::priority_queue<node*, std::vector<node*>, GreaterFrequency> frequency_list;

    std::map<char, leaf*>::const_iterator it;
    for (it = leaves.begin(); it != leaves.end(); ++it) {
        frequency_list.push(it->second);
    }

    // Add terminator to frequency list
    leaf* terminator = new eof;
    frequency_list.push(terminator);

    // When the size of the list is 1, we have the base node of the tree
    while(frequency_list.size() > 1) {
        // Merge the first 2 nodes
        node* left = frequency_list.top();
        frequency_list.pop();
        node* right = frequency_list.top();
        frequency_list.pop();

        frequency_list.push(new branch(left, right));
    }

    // Single pointer left is the root of our tree
    node* tree = frequency_list.top();
    std::vector<bool> encoding;

    tree->encode(encoding);
    //tree->print();

    std::string output_tree;
    tree->tree_encode(output_tree);

    output = output_tree;
    output.push_back((char)0); // Push an empty character before encoding

    unsigned position = 0; // First bit of a character
    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        leaves[*it]->output_encode(output, position);
    }
    terminator->output_encode(output, position);
}

void huffman::decode(std::string& input, std::string& output)
{


}
