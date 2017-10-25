#include <vector>
#include <string>
#include <memory>

namespace huffman {
    class node {
    public:
        virtual unsigned get_frequency() const = 0;
        virtual void print(int depth = -1, bool right = false) const = 0;
        virtual void encode(std::vector<bool>& encoding) = 0;
        virtual void tree_encode(std::string& out) const = 0;

        virtual void output_decode(const std::string& input,
            std::string::iterator it, std::string& output, unsigned position,
            const std::shared_ptr<node>& root) const = 0;

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

        virtual void output_decode(const std::string& input,
            std::string::iterator it, std::string& output, unsigned position,
            const std::shared_ptr<node>& root) const override;
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
        std::shared_ptr<node> left; // Left is always the smaller frequency
        std::shared_ptr<node> right;
    public:
        branch(std::shared_ptr<node> _left, std::shared_ptr<node> _right);

        virtual unsigned get_frequency() const override;
        virtual void print(int depth = -1, bool is_right = false) const override;
        virtual void encode(std::vector<bool>& encoding);
        virtual void tree_encode(std::string& out) const override;

        ~branch();

        virtual void output_decode(const std::string& input,
            std::string::iterator it, std::string& output, unsigned position,
            const std::shared_ptr<node>& root) const override;
    };

    class GreaterFrequency
    {
    public:
        bool operator()(const std::shared_ptr<node> lhs,
            const std::shared_ptr<node> rhs) const;
    };

    void encode(const std::string& input, std::string& output, int verbosity);
    void decode(const std::string& input, std::string& output, int verbosity);

    void match_bracket(const std::string& str, std::string::const_iterator& it);
    std::shared_ptr<node> tree_from_str(const std::string& str);

}
