#include <memory>
#include <string>
#include <vector>

class Pattern
{
  public:
    Pattern() = default;

    virtual std::size_t find_first_of(std::size_t pos, const std::string& input) = 0;
    virtual std::size_t starts_with(std::size_t pos, const std::string& input) = 0;

    virtual std::string print() { return std::string(); };

    bool one_or_more = false;
    bool optional = false;
};

class PatternHandler
{
  public:
    PatternHandler() = default;
    ~PatternHandler() = default;

    std::size_t match_patterns(const std::string& input, const std::string& patterns, bool startsWith = false);

  private:
    std::unique_ptr<Pattern> generatePattern(std::string& patterns);
    std::size_t findPatterns(const std::string& input, std::size_t pos, int pattern, const std::vector<std::unique_ptr<Pattern>>& patternList, bool startsWith);

    std::vector<std::shared_ptr<std::string>> m_patternReferences;
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

class StartAnchorPattern : public Pattern
{
  public:
    StartAnchorPattern(std::string& patterns);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Start Anchor Pattern");};

    static bool is_this_pattern(const std::string& patterns);
};

class EndAnchorPattern : public Pattern
{
  public:
    EndAnchorPattern(std::string& patterns);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("End Anchor Pattern");};

    static bool is_this_pattern(const std::string& patterns);
};

class OneMorePattern
{
  public:
    static bool is_this_pattern(std::string& patterns);
};

class OptionalPattern
{
  public:
    static bool is_this_pattern(std::string& patterns);
};

class WildcardPattern : public Pattern
{
  public:
    WildcardPattern(std::string& patterns);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Wild Card Pattern");};

    static bool is_this_pattern(const std::string& patterns);
};

class AlternationPattern : public Pattern
{
  public:
    AlternationPattern(std::string& patterns, std::shared_ptr<std::string> referencePattern, PatternHandler* handler);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Alternative Pattern Option 1: " + m_option1 + " Option 2: " + m_option2);};

    static bool is_this_pattern(const std::string& patterns);

  private:
    PatternHandler* m_handler;
    std::string m_option1, m_option2;
    std::shared_ptr<std::string> m_referencePattern;
    std::string m_pattern;
};

class ReferencePattern : public Pattern
{
  public:
    ReferencePattern(std::string& patterns, std::shared_ptr<std::string> referencePattern, PatternHandler* handler);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Reference Pattern " + m_pattern);};

    static bool is_this_pattern(const std::string& patterns);

  private:
    PatternHandler* m_handler;
    std::shared_ptr<std::string> m_referencePattern;
    std::string m_pattern;
};

class BackreferencePattern : public Pattern
{
  public:
    BackreferencePattern(std::string& patterns, std::vector<std::shared_ptr<std::string>>* referencedPatterns);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Backreference Pattern " + std::to_string(m_index));};

    static bool is_this_pattern(const std::string& patterns);

  private:
    std::vector<std::shared_ptr<std::string>>* m_referencedPatterns;
    int m_index;
};
