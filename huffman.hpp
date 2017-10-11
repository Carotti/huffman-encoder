#include <vector>
#include <string>

namespace huffman {
    class node {
    public:
        virtual unsigned get_frequency() const = 0;
        virtual void print(int depth = -1, bool right = false) const = 0;
        virtual void encode(std::vector<bool>& encoding) = 0;
        virtual void tree_encode(std::string& out) const = 0;

        virtual ~node() = 0;
    private:

    };

    class leaf : public node {
    private:
        char character;
        unsigned frequency;

        std::vector<bool> new_char; // The new representation of the character
    public:
        leaf(char _character, unsigned _frequency = 1);

        virtual unsigned get_frequency() const override;
        virtual void print(int depth = -1, bool is_right = false) const override;
        virtual void encode(std::vector<bool>& encoding) override;
        virtual void tree_encode(std::string& out) const override;

        void set_frequency(unsigned _frequency);
        void output_encode(std::string& str, unsigned& position);
    };

    // A leaf node with no character, represents the end of the file
    // EOF is necessary because the file may end at an unaligned byte
    class eof : public leaf {
    private:

    public:
        eof();
    };

    class branch : public node {
    private:
        node* left; // Left is always the smaller frequency
        node* right;
    public:
        branch(node* _left, node* _right);

        virtual unsigned get_frequency() const override;
        virtual void print(int depth = -1, bool is_right = false) const override;
        virtual void encode(std::vector<bool>& encoding);
        virtual void tree_encode(std::string& out) const override;

        ~branch();
    };

    class GreaterFrequency
    {
    public:
        bool operator()(const node* lhs, const node* rhs) const;
    };

    void encode(std::string& input, std::string& output, int verbosity);
    void decode(std::string& input, std::string& output, int verbosity);
}
