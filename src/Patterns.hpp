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
    PatternHandler(const std::string& input, const std::string& patterns, bool startsWith = false);
    ~PatternHandler() = default;

    operator std::size_t() const { return m_result; };
    operator bool() const { return m_result != std::string::npos; };

  private:
    std::size_t findPatterns(const std::string& input, std::size_t pos, int pattern, bool startsWith);
    void addPatternFromPatternString(std::string& patterns);

    std::size_t m_result = false;
    std::vector<std::unique_ptr<Pattern>> m_patternList;
    std::vector<std::shared_ptr<std::string>> m_patternReferences;
    std::vector<std::shared_ptr<std::size_t>> m_referenceStarts;
    std::vector<int> m_referenceIndexs;
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
    AlternationPattern(std::string& patterns);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Alternative Pattern Option 1: " + m_option1 + " Option 2: " + m_option2);};

    static bool is_this_pattern(const std::string& patterns);

  private:
    std::string m_option1, m_option2;
};

class ReferencePattern : public Pattern
{
  public:
    ReferencePattern(std::string& patterns, std::shared_ptr<std::size_t> referenceStart, int index);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("Reference Pattern " + std::to_string(m_index));};

    static bool is_this_pattern(const std::string& patterns);

  private:
    std::shared_ptr<std::size_t> m_referenceStart;
    int m_index;
};

class EndReferencePattern : public Pattern
{
  public:
    EndReferencePattern(std::string& patterns, std::shared_ptr<std::string> referencePattern, std::shared_ptr<std::size_t> referenceStart);

    std::size_t find_first_of(std::size_t pos, const std::string& input);
    std::size_t starts_with(std::size_t pos, const std::string& input);

    std::string print() {return std::string("End Reference Pattern");};

    static bool is_this_pattern(const std::string& patterns);

  private:
    std::shared_ptr<std::string> m_referencePattern;
    std::shared_ptr<std::size_t> m_referenceStart;
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
