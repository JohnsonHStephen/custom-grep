#include <iostream>
#include <memory>
#include <string>

class Pattern
{
  public:
    Pattern() = default;

    virtual std::size_t find_first_of(std::size_t pos, const std::string& input) = 0;
    virtual std::size_t starts_with(std::size_t pos, const std::string& input) = 0;

    virtual std::string print() { return std::string(); };
};

class PatternFactory
{
  public:
    static std::unique_ptr<Pattern> generatePattern(std::string& patterns);

};

class LiteralCharacterPattern : public Pattern
{
  public:
    LiteralCharacterPattern(std::string& patterns);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Character Pattern ") + std::string(1, m_character);};

    static bool is_this_pattern(const std::string& patterns);

  private:
    char m_character = 0;
};

class DigitsPattern : public Pattern
{
  public:
    DigitsPattern(std::string& patterns);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Digit Pattern");};

    static bool is_this_pattern(const std::string& patterns);
};

class AlphaNumPattern : public Pattern
{
  public:
    AlphaNumPattern(std::string& patterns);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("AlphaNum Pattern");};

    static bool is_this_pattern(const std::string& patterns);
};

class PositiveCharGroupPattern : public Pattern
{
  public:
    PositiveCharGroupPattern(std::string& patterns);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Positive Character Group Pattern ") + m_characters;};

    static bool is_this_pattern(const std::string& patterns);

  private:
    std::string m_characters = "";
};

class NegativeCharGroupPattern : public Pattern
{
  public:
    NegativeCharGroupPattern(std::string& patterns);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Negative Character Group Pattern ") + m_characters;};

    static bool is_this_pattern(const std::string& patterns);

  private:
    std::string m_characters = "";
};
