#include "Patterns.hpp"

#include <algorithm>
#include <iostream>

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
  else if (LiteralCharacterPattern::is_this_pattern(patterns))
  {
    result = std::unique_ptr<Pattern>(new LiteralCharacterPattern(patterns));
  }

  //check for multi pattern as these affect this pattern
  if (OneMorePattern::is_this_pattern(patterns))
  {
    result->one_or_more = true;
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

  if (patterns.size() != 1)  // this isnt the last pattern
    throw std::runtime_error("End of string anchor must be the final character " + patterns);

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
  if (pos >= input.size())
    throw std::runtime_error("Attempted to check for LiteralCharacterPattern beyond the string bounds");

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
  if (pos >= input.size())
    throw std::runtime_error("Attempted to check for DigitsPattern beyond the string bounds");

  if (auto it = std::find_if(input.begin(), input.end(), ::isdigit); it != input.end())
    return distance(input.begin() + pos, it) + 1;

  return std::string::npos;
}

std::size_t AlphaNumPattern::find_first_of(std::size_t pos, const std::string& input)
{
  std::size_t newPos;
  if (pos >= input.size())
    throw std::runtime_error("Attempted to check for AlphaNumPattern beyond the string bounds");

  if (auto it = std::find_if(input.begin(), input.end(), ::isalnum); it != input.end())
    return distance(input.begin() + pos, it) + 1;

  return std::string::npos;
}

std::size_t PositiveCharGroupPattern::find_first_of(std::size_t pos, const std::string& input)
{
  std::size_t newPos;
  if (pos >= input.size())
    throw std::runtime_error("Attempted to check for PositiveCharGroupPattern beyond the string bounds");

  if ((newPos = input.find_first_of(m_characters, pos)) != std::string::npos)
    return newPos + 1;

  return std::string::npos;
}

std::size_t NegativeCharGroupPattern::find_first_of(std::size_t pos, const std::string& input)
{
  std::size_t newPos;
  if (pos >= input.size())
    throw std::runtime_error("Attempted to check for NegativeCharGroupPattern beyond the string bounds");

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
  if (pos >= input.size())
    throw std::runtime_error("Attempted to check for LiteralCharacterPattern beyond the string bounds");

  //std::cout << "Comparing at pos " << pos << " " << input[pos] << " " << m_character << std::endl;

  if (input[pos] == m_character)
    return pos + 1;

  return std::string::npos;
}

std::size_t DigitsPattern::starts_with(std::size_t pos, const std::string& input)
{
  if (pos >= input.size())
    throw std::runtime_error("Attempted to check for DigitsPattern beyond the string bounds");

  if (::isdigit(input[pos]))
    return pos + 1;

  return std::string::npos;
}

std::size_t AlphaNumPattern::starts_with(std::size_t pos, const std::string& input)
{
  if (pos >= input.size())
    throw std::runtime_error("Attempted to check for AlphaNumPattern beyond the string bounds");

  if (::isalnum(input[pos]))
    return pos + 1;

  return std::string::npos;
}

std::size_t PositiveCharGroupPattern::starts_with(std::size_t pos, const std::string& input)
{
  if (pos >= input.size())
    throw std::runtime_error("Attempted to check for PositiveCharGroupPattern beyond the string bounds");

  if (m_characters.find_first_of(input[pos]) != std::string::npos)
    return pos + 1;

  return std::string::npos;
}

std::size_t NegativeCharGroupPattern::starts_with(std::size_t pos, const std::string& input)
{
  if (pos >= input.size())
    throw std::runtime_error("Attempted to check for NegativeCharGroupPattern beyond the string bounds");

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