#include "Patterns.hpp"

#define DEBUGGING 0

#include <algorithm>
#include <iostream>


/**********************************************************************
 * findMatchingEndBracket
 *
 * Description: Finds the end (closing ')') bracket position of the
 *      given bracket in the string
 *
 * Parameters:
 *   pos: the position of the opening bracket (assumes open bracket)
 *   input: the string with both brackets
 *
 * Returns: the position of closing bracket corresponding to the given
 *      open bracket, npos if not found
 *********************************************************************/
std::size_t findMatchingEndBracket(std::size_t pos, const std::string& input)
{
  int tries = 100;
  std::size_t newBracketPos = pos;

#if DEBUGGING
  std::cout << "searching end bracket for one at pos " << pos << " in " + input << std::endl;
#endif

  while (pos < std::string::npos && tries > 0)
  {
    pos = input.find_first_of(")", pos+1); // check for end bracket
    newBracketPos = input.find_first_of("(", newBracketPos+1); // check for a different starting bracket

#if DEBUGGING
    std::cout << "found end bracket at " << pos << " begin bracket at " << newBracketPos << " result " << (newBracketPos > pos) << std::endl;
#endif
    --tries;

    if (newBracketPos > pos) // if there is a different
      return pos;
  }

  return pos;
}

/**********************************************************************
 * findAlternateMarker
 *
 * Description: Finds the alternative marker '|' withing the current
 *      bracket scope
 *
 * Parameters:
 *   pos: the position of the opening bracket (assumes open bracket)
 *   input: the string with both brackets and possible marker
 *
 * Returns: the position of alternative marker in scope of the given
 *      open bracket, npos if not found
 *********************************************************************/
std::size_t findAlternateMarker(std::size_t pos, const std::string& input)
{
  int tries = 100;
  std::size_t newBracketPos = pos;

#if DEBUGGING
  std::cout << "searching for alternative marker from pos " << pos << " in " + input << std::endl;
#endif

  while (pos < std::string::npos && tries > 0)
  {
    pos = input.find_first_of("|", pos+1); // check for alternative marker
    newBracketPos = input.find_first_of("(", newBracketPos+1); // check for a different starting bracket
    std::size_t newEndBracketPos = findMatchingEndBracket(newBracketPos, input); // and its corresponding end bracket

    // if there are no brackets before the alternative marker this is the correct marker
    if (pos < newBracketPos)
      return pos;

    // check if its beyond the nested brackets
    if(pos > newEndBracketPos)
      return pos;

    // the alternative marker is in a nested bracket so keep looking
    --tries;
  }

  return std::string::npos;
}

/**********************************************************************
 * PatternHandler
 *
 * Description: Grabs all of the patterns then calls a recursive parser
 *
 * Parameters:
 *   input: the string to search for the patterns
 *   patterns: the string with all of the patterns
 *   startsWith: Whether or not the input must start with the pattern
 *
 * Returns: the position after all of the patterns have been matched
 *      npos if no match
 *********************************************************************/
PatternHandler::PatternHandler(const std::string& input, const std::string& patterns, bool startsWith)
: m_patternList(), m_patternReferences(), m_referenceStarts(), m_referenceIndexs()
{
  std::string workingPatterns = patterns;

  while (workingPatterns.size() > 0)
  {
    addPatternFromPatternString(workingPatterns);
  }

  m_result = findPatterns(input, 0, 0, startsWith);
}

/**********************************************************************
 * findPatterns
 *
 * Description: Recursive function that parses all patterns within the
 *      input
 *
 * Parameters:
 *    input: the string to search for the patterns
 *    pos: the position to start looking for patterns
 *    pattern: the index of the pattern currently being checked
 *    patternList: reference to vector of all patterns
 *    startsWith: Whether or not the input must start with the pattern
 *
 * Returns: the position after this and the rest of the patterns have
 *      been matched, npos if no match
 *********************************************************************/
std::size_t PatternHandler::findPatterns(const std::string& input, std::size_t pos, int pattern, bool startsWith)
{
  std::size_t newPos = 0, initialPos = pos;

  if (pattern >= m_patternList.size())
  {
#if DEBUGGING
    std::cout << "3 pattern " << pattern << " succeded\n";
#endif
    return pos; // success
  }

#if DEBUGGING
  std::cout << "2 pos " << pos << " pattern " << pattern << " " + m_patternList[pattern]->print() << std::endl;
#endif

  if (pos > input.size()) // position our of bounds
  {
#if DEBUGGING
    std::cout << "4 pattern " << m_patternList[pattern]->print() << " failed out of bounds\n";
#endif
    return std::string::npos;
  }

  std::size_t preCheckPos = pos;
  if (startsWith)
    pos = m_patternList[pattern]->starts_with(pos, input);
  else
    pos = m_patternList[pattern]->find_first_of(pos, input);

#if DEBUGGING
  std::cout << "5 pos " << pos << " pattern " << pattern << " " + m_patternList[pattern]->print() << std::endl;
#endif

  if (pos == std::string::npos) // pattern not found
  {
    if (!m_patternList[pattern]->optional)
    {
#if DEBUGGING
      std::cout << "6 pattern " << pattern << " failed\n";
#endif
      return pos;
    }
    pos = preCheckPos;
  }
  else if (m_patternList[pattern]->one_or_more) // need to check for multiple?
  {
    preCheckPos = pos;
    pos = findPatterns(input, pos, pattern, true);

    if (pos != std::string::npos) // subsequent pattern was found so no need to keep checking
    {
#if DEBUGGING
      std::cout << "6.5 after pattern " << pattern << " succeded\n";
#endif
      return pos;
    }

    pos = preCheckPos;
  }
  else if (m_patternList[pattern]->optional)
  {
    startsWith |= initialPos != pos;
    pos = findPatterns(input, pos, pattern+1, startsWith);

    if (pos != std::string::npos) // subsequent pattern was found with the optional existing
    {
#if DEBUGGING
      std::cout << "6.6 after pattern " << pattern << " succeded\n";
#endif
      return pos;
    }

    // no matching pattern worked with the optional existing so try without
    pos = preCheckPos;
  }


#if DEBUGGING
  std::cout << "6.7 " << pos << " " << pattern << std::endl;
#endif
  // if this one didnt need to start and we got a non matching pattern (didnt check the input)
  // the next one doesnt either
  startsWith |= initialPos != pos;

  if (m_patternList[pattern]->forceStart)
    startsWith = true;

  // pattern was found so go to next
  newPos = findPatterns(input, pos, pattern+1, startsWith);

  return newPos;
}

/**********************************************************************
 * addPatternFromPatternString
 *
 * Description: Generates the appropriate pattern based on the initial
 *     pattern in the patterns string. The used pattern is removed
 *
 * Parameters:
 *   patterns: string of all patterns desired, will have used pattern
 *       removed
 *********************************************************************/
void PatternHandler::addPatternFromPatternString(std::string& patterns)
{
  // referenceIndex tracks the total references added
  static int referenceIndex = 0;
  std::size_t prevSize = m_patternList.size();

  // order is important here
  if (StartAnchorPattern::is_this_pattern(patterns))
  {
    m_patternList.emplace_back(new StartAnchorPattern(patterns));
  }
  else if (EndAnchorPattern::is_this_pattern(patterns))
  {
    m_patternList.emplace_back(new EndAnchorPattern(patterns));
  }
  else if (DigitsPattern::is_this_pattern(patterns))
  {
    m_patternList.emplace_back(new DigitsPattern(patterns));
  }
  else if (AlphaNumPattern::is_this_pattern(patterns))
  {
    m_patternList.emplace_back(new AlphaNumPattern(patterns));
  }
  else if (NegativeCharGroupPattern::is_this_pattern(patterns))
  {
    m_patternList.emplace_back(new NegativeCharGroupPattern(patterns));
  }
  else if (PositiveCharGroupPattern::is_this_pattern(patterns))
  {
    m_patternList.emplace_back(new PositiveCharGroupPattern(patterns));
  }
  else if (WildcardPattern::is_this_pattern(patterns))
  {
    m_patternList.emplace_back(new WildcardPattern(patterns));
  }
  else if (AlternationPattern::is_this_pattern(patterns))
  {
    m_patternReferences.emplace_back(std::make_shared<std::string>());

    std::shared_ptr<std::size_t> referenceStart = std::make_shared<std::size_t>();
    m_referenceStarts.emplace_back(referenceStart);

    m_referenceIndexs.emplace_back(referenceIndex);

#if DEBUGGING
    std::cout << "starting reference " << referenceIndex << std::endl;
#endif

    m_patternList.emplace_back(new ReferencePattern(patterns, referenceStart, referenceIndex++));
    m_patternList.emplace_back(new AlternationPattern(patterns));
  }
  else if (ReferencePattern::is_this_pattern(patterns))
  {
    m_patternReferences.emplace_back(std::make_shared<std::string>());

    std::shared_ptr<std::size_t> referenceStart = std::make_shared<std::size_t>();
    m_referenceStarts.emplace_back(referenceStart);

    m_referenceIndexs.emplace_back(referenceIndex);

#if DEBUGGING
    std::cout << "starting reference " << referenceIndex << std::endl;
#endif

    m_patternList.emplace_back(new ReferencePattern(patterns, referenceStart, referenceIndex++));
  }
  else if (EndReferencePattern::is_this_pattern(patterns))
  {
    if (m_referenceIndexs.size() <= 0)
      throw std::runtime_error("Unexpected closing bracket");

#if DEBUGGING
    std::cout << "ending reference " << m_referenceIndexs.back() << std::endl;
#endif

    m_patternList.emplace_back(new EndReferencePattern(patterns, m_patternReferences[m_referenceIndexs.back()], m_referenceStarts[m_referenceIndexs.back()]));
    m_referenceIndexs.pop_back();
  }
  else if (BackreferencePattern::is_this_pattern(patterns))
  {
    m_patternList.emplace_back(new BackreferencePattern(patterns, m_patternReferences));
  }
  else if (LiteralCharacterPattern::is_this_pattern(patterns)) // needs to be last
  {
    m_patternList.emplace_back(new LiteralCharacterPattern(patterns));
  }

  //check for multi pattern as these affect this pattern
  if (OneMorePattern::is_this_pattern(patterns))
  {
    m_patternList.back()->one_or_more = true;
#if DEBUGGING
    std::cout << "made pattern 1 or more" << std::endl;
#endif
  }
  else if (OptionalPattern::is_this_pattern(patterns))
  {
    m_patternList.back()->optional = true;
#if DEBUGGING
    std::cout << "made pattern optional" << std::endl;
#endif

  }

  if (prevSize == m_patternList.size())
    throw std::runtime_error("Unhandled pattern " + patterns);

#if DEBUGGING
    std::cout << "Added " << m_patternList.back()->print() << std::endl;
#endif
}

/**********************************************************************
 * Pattern Comparisons is_this_pattern
 *
 * Description: check for the pattern at the begining of the provide
 *     patterns
 *
 * Parameters:
 *   patterns: string of all patterns desired
 *********************************************************************/
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

bool AlternationPattern::is_this_pattern(const std::string& patterns)
{
  if (patterns.compare(0, 1, "(") != 0)
    return false;

  if (findAlternateMarker(0, patterns) == std::string::npos)
    return false;

  if (findMatchingEndBracket(0, patterns) == std::string::npos)
    throw std::runtime_error("Alternation pattern missing end bracket ')'");

  return true;
}

bool ReferencePattern::is_this_pattern(const std::string& patterns)
{
  if (patterns.compare(0, 1, "(") != 0)
    return false;

  if (findMatchingEndBracket(0, patterns) == std::string::npos)
    throw std::runtime_error("Reference pattern missing end bracket ')'");

  return true;
}

bool EndReferencePattern::is_this_pattern(const std::string& patterns)
{
  return patterns.compare(0, 1, ")") == 0;
}

bool BackreferencePattern::is_this_pattern(const std::string& patterns)
{
  if (patterns.size() < 2 || patterns.compare(0, 1, "\\") != 0)
    return false;

  if (::isdigit(patterns[1]))
    return true;

  return false;
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
 *********************************************************************/
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

  forceStart = true;

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

AlternationPattern::AlternationPattern(std::string& patterns)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create AlternationPattern without proper pattern in " + patterns);

  std::size_t endPos = findMatchingEndBracket(0, patterns);
  std::size_t dividerPos = findAlternateMarker(0, patterns);
  m_option1 = patterns.substr(0, dividerPos);
  m_option2 = patterns.substr(dividerPos+1, endPos-1-dividerPos);

  // leave in closing bracket to finish adding the reference
  patterns = patterns.substr(endPos);

#if DEBUGGING
  std::cout << "adding alternation pattern option 1: " + m_option1 + " option 2: " + m_option2 + " leftover patterns: " + patterns << std::endl;
#endif
}

ReferencePattern::ReferencePattern(std::string& patterns, const std::shared_ptr<std::size_t>& referenceStart, int index)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create ReferencePattern without proper pattern in " + patterns);

  m_index = index;
  m_referenceStart = referenceStart;

  patterns = patterns.substr(1);

#if DEBUGGING
  std::cout << "adding reference pattern " << m_index << std::endl;
#endif
}

EndReferencePattern::EndReferencePattern(std::string& patterns, const std::shared_ptr<std::string>& referencePattern, const std::shared_ptr<std::size_t>& referenceStart)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create EndReferencePattern without proper pattern in " + patterns);

  m_referencePattern = referencePattern;
  m_referenceStart = referenceStart;

  patterns = patterns.substr(1);

#if DEBUGGING
  std::cout << "adding end of reference pattern" << std::endl;
#endif
}

BackreferencePattern::BackreferencePattern(std::string& patterns, const std::vector<std::shared_ptr<std::string>>& referencedPatterns)
{
  if (!is_this_pattern)
    throw std::runtime_error("Attempted to create BackreferencePattern without proper pattern in " + patterns);

  m_index = patterns[1] - '0' - 1;
#if DEBUGGING
  std::cout << "creating backreference with from " + patterns.substr(1, 1) + " to get index " << m_index << std::endl;
#endif

  if (m_index >= referencedPatterns.size())
    throw std::runtime_error("Attempted to create BackreferencePattern to an undeclared pattern");

  m_referencedPattern = referencedPatterns.at(m_index);

  patterns = patterns.substr(2);
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
 *********************************************************************/
std::size_t LiteralCharacterPattern::find_first_of(std::size_t pos, const std::string& input)
{
  std::size_t newPos;

#if DEBUGGING
  std::cout << "first checking at pos " << pos  << " and beyond from " << input << " looking for " << m_character << " found at " << input.find(m_character, pos) << std::endl;
#endif
  if ((newPos = input.find_first_of(m_character, pos)) != std::string::npos)
  {
#if DEBUGGING
    std::cout << "first found at pos " << newPos  << " character " << input[newPos] << " from " << input << " looking for " << m_character << std::endl;
#endif
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

std::size_t AlternationPattern::find_first_of(std::size_t pos, const std::string& input)
{
#if DEBUGGING
  std::cout << "first entering option 1 with input " << input.substr(pos) << " patterns " << m_option1 << std::endl;
#endif

  // check if the first option succeeds
  std::size_t result = PatternHandler(input.substr(pos), m_option1, false);

#if DEBUGGING
  std::cout << "first leaving option 1 with input " << input.substr(pos) << " patterns " << m_option1 << " result " << result << std::endl;
#endif

  // first option succeeded and result is relative to pos
  if (result != std::string::npos)
    return result+pos;

#if DEBUGGING
  std::cout << "first entering option 2 with input " << input.substr(pos) << " patterns " << m_option2 << std::endl;
#endif
  // check if the second option succeeds
  result = PatternHandler(input.substr(pos), m_option2, false);

#if DEBUGGING
  std::cout << "first leaving option 2 with input " << input.substr(pos) << " patterns " << m_option2 << " result " << result << std::endl;
#endif

  // second option succeeded and result is relative to pos
  if (result != std::string::npos)
    return result+pos;

  return result;
}

std::size_t ReferencePattern::find_first_of(std::size_t pos, const std::string& input)
{
  if (m_referenceStart.expired())
    throw std::runtime_error("Attempted to access expired memory");

  std::shared_ptr<std::size_t> referenceStart = m_referenceStart.lock();
  *referenceStart = pos;

#if DEBUGGING
  std::cout << "first reference pattern " << m_index << " started at " << pos << " " + input.substr(pos) << std::endl;
#endif

  return pos;
}

std::size_t EndReferencePattern::find_first_of(std::size_t pos, const std::string& input)
{
  if (m_referencePattern.expired())
    throw std::runtime_error("Attempted to access expired memory");

  if (m_referenceStart.expired())
    throw std::runtime_error("Attempted to access expired memory");

  std::shared_ptr<std::size_t> referenceStart = m_referenceStart.lock();
  std::shared_ptr<std::string> referencePattern = m_referencePattern.lock();

#if DEBUGGING
  std::cout << "first found reference that started at " << *referenceStart << " ended at " << pos << std::endl;
#endif

  // save the input that matched
  *referencePattern = input.substr(*referenceStart, pos - *referenceStart);

#if DEBUGGING
  std::cout << "first found reference to check later: " + *referencePattern + " started at " << *referencePattern << " ended at " << pos << std::endl;
#endif

  return pos;
}

std::size_t BackreferencePattern::find_first_of(std::size_t pos, const std::string& input)
{
  if (m_referencedPattern.expired())
    throw std::runtime_error("Attempted to access expired memory");

  std::shared_ptr<std::string> referencedPattern = m_referencedPattern.lock();

  std::size_t newPos;

#if DEBUGGING
  std::cout << "first checking at pos " << pos  << " and beyond from " << input << " looking for " << *referencedPattern << " found at " << input.find_first_of(*referencedPattern, pos) << std::endl;
#endif
  if ((newPos = input.find_first_of(*referencedPattern, pos)) != std::string::npos)
  {
#if DEBUGGING
    std::cout << "first found at pos " << newPos  << " string " << *referencedPattern << " from " << input << " leftovers " << input.substr(newPos + referencedPattern->size()) << std::endl;
#endif
    return newPos + referencedPattern->size();
  }

  return std::string::npos;
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
#if DEBUGGING
  std::cout << "starts  Comparing at pos " << pos << " " << input[pos] << " " << m_character << std::endl;
#endif

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
  throw std::runtime_error("Start of string anchor must be the first character");
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

std::size_t AlternationPattern::starts_with(std::size_t pos, const std::string& input)
{
#if DEBUGGING
  std::cout << "starts  entering option 1 with input " << input.substr(pos) << " patterns " << m_option1 << std::endl;
#endif

  // check if the first option succeeds
  std::size_t result = PatternHandler(input.substr(pos), m_option1, true);

#if DEBUGGING
  std::cout << "starts  leaving option 1 with input " << input.substr(pos) << " patterns " << m_option1 << " result " << result << std::endl;
#endif

  // first option succeeded and result is relative to pos
  if (result != std::string::npos)
    return result+pos;

#if DEBUGGING
  std::cout << "starts  entering option 2 with input " << input.substr(pos) << " patterns " << m_option2 << std::endl;
#endif
  // check if the second option succeeds
  result = PatternHandler(input.substr(pos), m_option2, true);

#if DEBUGGING
  std::cout << "starts  leaving option 2 with input " << input.substr(pos) << " patterns " << m_option2 << " result " << result << std::endl;
#endif

  // second option succeeded and result is relative to pos
  if (result != std::string::npos)
    return result+pos;

  return result;
}

std::size_t ReferencePattern::starts_with(std::size_t pos, const std::string& input)
{
  if (m_referenceStart.expired())
    throw std::runtime_error("Attempted to access expired memory");

  std::shared_ptr<std::size_t> referenceStart = m_referenceStart.lock();

  *referenceStart = pos;

#if DEBUGGING
  std::cout << "starts  reference pattern " << m_index << " started at " << pos << " " + input.substr(pos) << std::endl;
#endif

  return pos;
}

std::size_t EndReferencePattern::starts_with(std::size_t pos, const std::string& input)
{
  if (m_referencePattern.expired())
    throw std::runtime_error("Attempted to access expired memory");

  if (m_referenceStart.expired())
    throw std::runtime_error("Attempted to access expired memory");

  std::shared_ptr<std::size_t> referenceStart = m_referenceStart.lock();
  std::shared_ptr<std::string> referencePattern = m_referencePattern.lock();

#if DEBUGGING
  std::cout << "starts  found reference that started at " << *referenceStart << " ended at " << pos << std::endl;
#endif

  // save the input that matched
  *referencePattern = input.substr(*referenceStart, pos - *referenceStart);

#if DEBUGGING
  std::cout << "starts  found reference to check later: " + *referencePattern + " started at " << *referenceStart << " ended at " << pos << std::endl;
#endif

  return pos;
}

std::size_t BackreferencePattern::starts_with(std::size_t pos, const std::string& input)
{
  if (m_referencedPattern.expired())
    throw std::runtime_error("Attempted to access expired memory");

  std::shared_ptr<std::string> referencedPattern = m_referencedPattern.lock();

#if DEBUGGING
  std::cout << "starts  comparing " << input.substr(pos, referencedPattern->size()) << " to " << *referencedPattern << std::endl;
#endif
  if (input.compare(pos, referencedPattern->size(), *referencedPattern) == 0)
    return pos + referencedPattern->size();

  return std::string::npos;
}
