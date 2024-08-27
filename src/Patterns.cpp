#include "Patterns.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

static std::size_t findSubsequentPatterns(const std::string& input, std::size_t pos, int pattern, const std::vector<std::unique_ptr<Pattern>>& patternList)
{
  //std::cout << "test 2 " << pos << " " << pattern << std::endl;
  std::size_t newPos;

  if (pattern >= patternList.size())
  {
    //std::cout << "test 3 pattern " << pattern << " succeded\n";
    return pos; // success
  }

  if (pos > input.size()) // position our of bounds
  {
    //std::cout << "test 4 pattern " << pattern << " failed out of bounds\n";
    return std::string::npos;
  }

  std::size_t preCheckPos = pos;
  pos = patternList[pattern]->starts_with(pos, input);

  //std::cout << "test 5 " << pos << " " << pattern << std::endl;

  if (pos == std::string::npos) // pattern not found
  {
    if (!patternList[pattern]->optional)
    {
      //std::cout << "test 6 pattern " << pattern << " failed\n";
      return pos;
    }
    pos = preCheckPos;
  }

  preCheckPos = pos;
  if (patternList[pattern]->one_or_more) // need to check for multiple?
  {
    pos = findSubsequentPatterns(input, pos, pattern, patternList);

    if (newPos != std::string::npos) // subsequent pattern was found so no need to keep checking
    {
      //std::cout << "test 6.5 after pattern " << pattern << " succeded\n";
      return newPos;
    }

    pos = preCheckPos;
  }


  //std::cout << "test 6.7 " << pos << " " << pattern << std::endl;
  // pattern was found so go to next
  newPos = findSubsequentPatterns(input, pos, pattern+1, patternList);

  return newPos;
#if 0
  if (newPos != std::string::npos) // subsequent pattern was found
  {
    //std::cout << "test 7 after pattern " << pattern << " succeded\n";
    return newPos;
  }

  //std::cout << "test 7.5 " << pos+1 << " " << pattern << std::endl;
  // check this pattern again but a little farther on
  return findSubsequentPatterns(input, pos+1, pattern, patternList);
#endif
}

static std::size_t findPattern(const std::string& input, const std::vector<std::unique_ptr<Pattern>>& patternList, bool startsWith)
{
  std::size_t pos = 0;
  int tries = 0;

  if (patternList.size() == 0)
    return 0;

  if (input.size() == 0)
    return std::string::npos;

  while (pos < input.size() && tries < 100)
  {
    tries++;
    //std::cout << "test 0 " << pos << " " << startsWith << std::endl;
    if (startsWith)
    {
      pos = patternList[0]->starts_with(pos, input);
    }
    else
      pos = patternList[0]->find_first_of(pos, input);
    //std::cout << "test 1 " << pos << std::endl;

    if (pos == std::string::npos) // first pattern failed to find any more matches
    {
      //std::cout << "test failed\n";
      return pos;
    }

    std::size_t newPos = findSubsequentPatterns(input, pos, 1, patternList);

    if (newPos != std::string::npos) // subsequent pattern was found
    {
      //std::cout << "test succeded\n";
      return newPos;
    }
  }

  throw std::runtime_error("Too many tries, the while loop was stuck");
  return pos;
}

std::size_t PatternFactory::match_patterns(const std::string& input, const std::string& patterns, bool startsWith)
{
  //check for split by alternate not in brackets
  for (int i = 0; i < patterns.size(); i++)
  {
    // ignore everything between brackets
    if (patterns[i] == '(')
      i = patterns.find_last_of(')');

    if (patterns[i] == '|')
    {
      //std::cout << "test 1 Alternate Pattern " << patterns.substr(0, i) << " input " << input << std::endl;
      std::size_t endPos = match_patterns(input, patterns.substr(0, i), startsWith); //1st option
      if (endPos != std::string::npos)
        return endPos;
      //std::cout << "test 2 Alternate Pattern " << patterns.substr(i+1) << " input " << input << std::endl;

      endPos = match_patterns(input, patterns.substr(i+1), startsWith); //2nd option
      return endPos;
    }
  }

  std::string workingPatterns = patterns;
  std::vector<std::unique_ptr<Pattern>> patternList;

  while (workingPatterns.size() > 0)
  {
    if (BracketPattern::is_this_pattern(workingPatterns))
    {
      std::size_t endPreBracketPos = 0;
      //pre-bracket
      if (patternList.size() > 0)
      {
        //std::cout << "test Pre Bracket input " << input << std::endl;
        endPreBracketPos = findPattern(input, patternList, startsWith);
        if (endPreBracketPos == std::string::npos)
          return endPreBracketPos;

        startsWith = true; // force starts with as this "A(B)" is the same as "AB"
      }

      //in bracket
      std::string bracketPatterns = workingPatterns.substr(0, workingPatterns.find_last_of(")"));
      //std::cout << "test Bracket Pattern " << bracketPatterns << " input " << input.substr(endPreBracketPos) << std::endl;

      std::size_t endBracketPos = match_patterns(input.substr(endPreBracketPos), bracketPatterns, startsWith);
      if (endBracketPos == std::string::npos)
        return endBracketPos;

      std::size_t beginePostBracketPos = endBracketPos + endPreBracketPos; // endBracketPos is relative to endPreBracketPos so add them together
      //post-bracket
      bracketPatterns = workingPatterns.substr(workingPatterns.find_last_of(")")+1);
      if (bracketPatterns.size() == 0) // no post bracket stuff
        return endBracketPos;

      //std::cout << "test Post Bracket Pattern " << bracketPatterns << " input " << input.substr(beginePostBracketPos) << std::endl;

      return match_patterns(input.substr(beginePostBracketPos), bracketPatterns, true);
    }

    patternList.emplace_back(PatternFactory::generatePattern(workingPatterns));
    //std::cout << "test Added " << patternList.back()->print() << std::endl;
  }

  return findPattern(input, patternList, startsWith);
}

/**********************************************************************
* generatePattern
*
* Description: Generates the appropriate pattern based on the initial
*     pattern in the patterns string. The used pattern is removed
*
* Parameters:
*   patterns: string of all patterns desired, will have used pattern
*       removed
*
* Returns: a unique pointer to the pattern, the caller is responsible
*     for deletion
**********************************************************************/
std::unique_ptr<Pattern> PatternFactory::generatePattern(std::string& patterns)
{
  std::unique_ptr<Pattern> result = nullptr;
  // order is important here
  if (StartAnchorPattern::is_this_pattern(patterns))
  {
    result = std::unique_ptr<Pattern>(new StartAnchorPattern(patterns));
  }
  else if (EndAnchorPattern::is_this_pattern(patterns))
  {
    result = std::unique_ptr<Pattern>(new EndAnchorPattern(patterns));
  }
  else if (DigitsPattern::is_this_pattern(patterns))
  {
    result = std::unique_ptr<Pattern>(new DigitsPattern(patterns));
  }
  else if (AlphaNumPattern::is_this_pattern(patterns))
  {
    result = std::unique_ptr<Pattern>(new AlphaNumPattern(patterns));
  }
  else if (NegativeCharGroupPattern::is_this_pattern(patterns))
  {
    result = std::unique_ptr<Pattern>(new NegativeCharGroupPattern(patterns));
  }
  else if (PositiveCharGroupPattern::is_this_pattern(patterns))
  {
    result = std::unique_ptr<Pattern>(new PositiveCharGroupPattern(patterns));
  }
  else if (WildcardPattern::is_this_pattern(patterns))
  {
    result = std::unique_ptr<Pattern>(new WildcardPattern(patterns));
  }
  else if (LiteralCharacterPattern::is_this_pattern(patterns)) // needs to be last
  {
    result = std::unique_ptr<Pattern>(new LiteralCharacterPattern(patterns));
  }

  //check for multi pattern as these affect this pattern
  if (OneMorePattern::is_this_pattern(patterns))
  {
    result->one_or_more = true;
  }
  else if (OptionalPattern::is_this_pattern(patterns))
  {
    result->optional = true;
  }

  if (result == nullptr)
    throw std::runtime_error("Unhandled pattern " + patterns);

  return result;
}

/**********************************************************************
* Pattern Comparisons is_this_pattern
*
* Description: check for the pattern at the begining of the provide
*     patterns
*
* Parameters:
*   patterns: string of all patterns desired
**********************************************************************/
bool LiteralCharacterPattern::is_this_pattern(const std::string& patterns)
{
  return ::isprint(patterns[0]);
}

bool DigitsPattern::is_this_pattern(const std::string& patterns)
{
  return patterns.compare(0, 2, "\\d") == 0;
}

bool AlphaNumPattern::is_this_pattern(const std::string& patterns)
{
  return patterns.compare(0, 2, "\\w") == 0;
}

bool PositiveCharGroupPattern::is_this_pattern(const std::string& patterns)
{
  if (patterns.compare(0, 1, "[") != 0)
    return false;

  int endPos = patterns.find("]");
  if (endPos == std::string::npos)
    throw std::runtime_error("Pattern missing end bracket");

  return true;
}

bool NegativeCharGroupPattern::is_this_pattern(const std::string& patterns)
{
  if (patterns.compare(0, 2, "[^") != 0)
    return false;

  int endPos = patterns.find("]");
  if (endPos == std::string::npos)
    throw std::runtime_error("Pattern missing end bracket");

  return true;
}

bool StartAnchorPattern::is_this_pattern(const std::string& patterns)
{
  return patterns.compare(0, 1, "^") == 0;
}

bool EndAnchorPattern::is_this_pattern(const std::string& patterns)
{
  return patterns.compare(0, 1, "$") == 0;
}

bool OneMorePattern::is_this_pattern(std::string& patterns)
{
  if (patterns.compare(0, 1, "+") != 0)
    return false;

  patterns = patterns.substr(1);
  return true;
}

bool OptionalPattern::is_this_pattern(std::string& patterns)
{
  if (patterns.compare(0, 1, "?") != 0)
    return false;

  patterns = patterns.substr(1);
  return true;
}

bool WildcardPattern::is_this_pattern(const std::string& patterns)
{
  return patterns.compare(0, 1, ".") == 0;
}

bool AlternationPattern::is_this_pattern(std::string& patterns)
{
  if (patterns.compare(0, 1, "|") != 0)
    return false;

  patterns = patterns.substr(1);
  return true;
}

bool BracketPattern::is_this_pattern(std::string& patterns)
{
  if (patterns.compare(0, 1, "(") != 0)
    return false;

  int endPos = patterns.find(")");
  if (endPos == std::string::npos)
    throw std::runtime_error("Pattern missing end bracket");

  patterns = patterns.substr(1);
  return true;
}


/**********************************************************************
* Pattern Constructors
*
* Description: all constructors need to check for the pattern then
*     remove said pattern
*
* Parameters:
*   patterns: string of all patterns desired, will have used pattern
*       removed
**********************************************************************/
LiteralCharacterPattern::LiteralCharacterPattern(std::string& patterns)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create LiteralCharacterPattern without proper pattern in " + patterns);

  m_character = patterns[0];

  patterns = patterns.substr(1);
}

DigitsPattern::DigitsPattern(std::string& patterns)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create DigitsPattern without proper pattern in " + patterns);
  patterns = patterns.substr(2);
}

AlphaNumPattern::AlphaNumPattern(std::string& patterns)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create AlphaNumPattern without proper pattern in " + patterns);
  patterns = patterns.substr(2);
}

PositiveCharGroupPattern::PositiveCharGroupPattern(std::string& patterns)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create PositiveCharGroupPattern without proper pattern in " + patterns);

  int endPos = patterns.find("]");

  m_characters = patterns.substr(1, endPos-1);

  patterns = patterns.substr(endPos+1);

  //std::cout << m_characters << " " << patterns << std::endl;
}

NegativeCharGroupPattern::NegativeCharGroupPattern(std::string& patterns)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create NegativeCharGroupPattern without proper pattern in " + patterns);

  int endPos = patterns.find("]");

  m_characters = patterns.substr(2, endPos-2);

  patterns = patterns.substr(endPos+1);

  //std::cout << m_characters << " " << patterns << std::endl;
}

StartAnchorPattern::StartAnchorPattern(std::string& patterns)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create StartAnchorPattern without proper pattern in " + patterns);

  patterns = patterns.substr(1);
}

EndAnchorPattern::EndAnchorPattern(std::string& patterns)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create EndAnchorPattern without proper pattern in " + patterns);

  patterns = patterns.substr(1);
}

WildcardPattern::WildcardPattern(std::string& patterns)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create WildcardPattern without proper pattern in " + patterns);

  patterns = patterns.substr(1);
}

/**********************************************************************
* Pattern find_first_of
*
* Description: Finds the end position of the first substring that
*     matches the pattern
*
* Parameters:
*   pos: which position to start at
*   input: the string to search for the pattern
*
* Returns: the position after the final character of the pattern in
*      the input string, npos if not found
**********************************************************************/
std::size_t LiteralCharacterPattern::find_first_of(std::size_t pos, const std::string& input)
{
  std::size_t newPos;

  //std::cout << "checking at pos " << pos  << " and beyond from " << input << " looking for " << m_character << " found at " << input.find(m_character, pos) << std::endl;
  if ((newPos = input.find_first_of(m_character, pos)) != std::string::npos)
  {
    //std::cout << "found at pos " << newPos  << " character " << input[newPos] << " from " << input << " looking for " << m_character << std::endl;
    return newPos + 1;
  }

  return std::string::npos;
}

std::size_t DigitsPattern::find_first_of(std::size_t pos, const std::string& input)
{
  std::size_t newPos;

  if (auto it = std::find_if(input.begin(), input.end(), ::isdigit); it != input.end())
    return distance(input.begin() + pos, it) + 1;

  return std::string::npos;
}

std::size_t AlphaNumPattern::find_first_of(std::size_t pos, const std::string& input)
{
  std::size_t newPos;

  if (auto it = std::find_if(input.begin(), input.end(), ::isalnum); it != input.end())
    return distance(input.begin() + pos, it) + 1;

  return std::string::npos;
}

std::size_t PositiveCharGroupPattern::find_first_of(std::size_t pos, const std::string& input)
{
  std::size_t newPos;

  if ((newPos = input.find_first_of(m_characters, pos)) != std::string::npos)
    return newPos + 1;

  return std::string::npos;
}

std::size_t NegativeCharGroupPattern::find_first_of(std::size_t pos, const std::string& input)
{
  std::size_t newPos;

  if ((newPos = input.find_first_not_of(m_characters, pos)) != std::string::npos)
    return newPos + 1;

  return std::string::npos;
}

std::size_t StartAnchorPattern::find_first_of(std::size_t pos, const std::string& input)
{
  if (pos != 0)
    return std::string::npos;

  return 0;
}

std::size_t EndAnchorPattern::find_first_of(std::size_t pos, const std::string& input)
{
  return input.size();
}

std::size_t WildcardPattern::find_first_of(std::size_t pos, const std::string& input)
{
  if (pos >= input.size())
    return std::string::npos;

  return pos + 1;
}

/**********************************************************************
* Pattern starts_with
*
* Description: checks if the input string at pos starts with the
*     pattern
*
* Parameters:
*   pos: which position to start at
*   input: the string to check for the pattern
*
* Returns: the position after the final character of the pattern in
*     the input string, npos if it doesn't start with the pattern
*
* Note: this is similar to find_first_of however doesn't check the
*     whole string just the start of (at given pos) and can therefore
*     be faster
**********************************************************************/
std::size_t LiteralCharacterPattern::starts_with(std::size_t pos, const std::string& input)
{
  //std::cout << "Comparing at pos " << pos << " " << input[pos] << " " << m_character << std::endl;

  if (input[pos] == m_character)
    return pos + 1;

  return std::string::npos;
}

std::size_t DigitsPattern::starts_with(std::size_t pos, const std::string& input)
{
  if (::isdigit(input[pos]))
    return pos + 1;

  return std::string::npos;
}

std::size_t AlphaNumPattern::starts_with(std::size_t pos, const std::string& input)
{
  if (::isalnum(input[pos]))
    return pos + 1;

  return std::string::npos;
}

std::size_t PositiveCharGroupPattern::starts_with(std::size_t pos, const std::string& input)
{
  if (m_characters.find_first_of(input[pos]) != std::string::npos)
    return pos + 1;

  return std::string::npos;
}

std::size_t NegativeCharGroupPattern::starts_with(std::size_t pos, const std::string& input)
{
  if (m_characters.find_first_of(input[pos]) == std::string::npos)
    return pos + 1;

  return std::string::npos;
}

std::size_t StartAnchorPattern::starts_with(std::size_t pos, const std::string& input)
{
  throw std::runtime_error("Start of string anchor mus be the first character");
}

std::size_t EndAnchorPattern::starts_with(std::size_t pos, const std::string& input)
{
  if (pos != input.size())
    return std::string::npos;

  return input.size();
}

std::size_t WildcardPattern::starts_with(std::size_t pos, const std::string& input)
{
  if (pos >= input.size())
    return std::string::npos;

  return pos + 1;
}