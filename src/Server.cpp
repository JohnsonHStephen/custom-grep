#include "Patterns.hpp"

#include <iostream>
#include <string>
#include <vector>

#if 0
std::string getFirstPattern(std::string& patterns)
{
  int length = patterns.length();
  std::string result;

  if (length == 0)
  {
    throw std::runtime_error("Empty pattern");
  }

  if (length >= 2 && patterns.compare(0, 2, "\\d") == 0) // any digit
  {
    result = "\\d";
  }
  else if (length >= 2 && patterns.compare(0, 2, "\\w") == 0) // any alpha numeric
  {
    result = "\\w";
  }
  else if (length >= 3 && patterns.compare(0, 2, "[^") == 0) // negative character group
  {
    int endPos = patterns.find("]");
    if (endPos == std::string::npos)
      throw std::runtime_error("Pattern missing end bracket");

    result = patterns.substr(0, endPos);
  }
  else if (length >= 2 && patterns.compare(0, 2, "[") == 0) // positive character group
  {
    int endPos = patterns.find("]");
    if (endPos == std::string::npos)
      throw std::runtime_error("Pattern missing end bracket");

    result = patterns.substr(0, endPos);
  }
  else if (::isprint(patterns[0])) // literal character
  {
    result = patterns[0];
  }
  else
    throw std::runtime_error("Unhandled pattern " + patterns);

  patterns = patterns.substr(result.size());
  return result;
}

std::size_t match_pattern(const std::string& input_line, const std::string& pattern, bool initialSearch = false)
{
  struct ContainsCharacters
  {
    const std::string d;
    ContainsCharacters(const std::string& n) : d(n) {}
    bool operator()(char n) const { return d.find(n) != std::string::npos; }
  };

  auto startItter = input_line.begin();
  auto endItter = input_line.end();

  if (pattern.length() == 1) // literal character
  {
    return input_line.find(pattern, pos);
  }
  else if (pattern.compare("\\d") == 0) // any digit
  {
    return distance(input_line.begin() + pos, std::find_if(input_line.begin(), input_line.end(), ::isdigit));
  }
  else if (pattern.compare("\\w") == 0) // any alpha numeric
  {
    return distance(input_line.begin(), std::find_if(input_line.begin(), input_line.end(), ::isalnum));
  }
  else if (int startPos = pattern.find("[^") != std::string::npos) // negative character group
  {
    startPos++;
    int endPos = pattern.find("]");
    if (std::none_of(input_line.begin(), input_line.end(), ContainsCharacters(pattern.substr(startPos, endPos - startPos))))
      return 0; // the characters dont exist so the whole string is good
    return std::string::npos; // the characters do exist and therefore none of the string is good
  }
  else if (int startPos = pattern.find("[") != std::string::npos) // positive character group
  {
    int endPos = pattern.find("]");
    return std::any_of(input_line.begin(), input_line.end(), ContainsCharacters(pattern.substr(startPos, endPos - startPos)));
  }
  else
  {
    throw std::runtime_error("Unhandled pattern " + pattern);
  }
}
#endif
std::size_t findSubsequentPatterns(const std::string& input, std::size_t pos, int pattern, const std::vector<std::unique_ptr<Pattern>>& patternList)
{
  //std::cout << "test 2 " << pos << " " << pattern << std::endl;
  std::size_t newPos;

  if (pattern >= patternList.size())
  {
    //std::cout << "test 3 pattern " << pattern << " succeded\n";
    return pos; // success
  }

  if (pos >= input.size()) // position our of bounds
  {
    //std::cout << "test 4 pattern " << pattern << " succeded\n";
    return std::string::npos;
  }

  pos = patternList[pattern]->starts_with(pos, input);

  //std::cout << "test 5 " << pos << " " << pattern << std::endl;

  if (pos == std::string::npos) // pattern not found
  {
    //std::cout << "test 6 pattern " << pattern << " failed\n";
    return pos;
  }

  // pattern was found so go to next
  newPos = findSubsequentPatterns(input, pos, pattern+1, patternList);

  if (newPos != std::string::npos) // subsequent pattern was found
  {
    //std::cout << "test 7 after pattern " << pattern << " succeded\n";
    return newPos;
  }

  // check this pattern again but a little farther on
  return findSubsequentPatterns(input, pos+1, pattern, patternList);
}

bool match_patterns(const std::string& input_line, const std::string& patterns)
{
  std::string workingPatterns = patterns;
/*
  while (workingPatterns.size() > 0)
  {
    std::string pattern = getFirstPattern(workingPatterns);
    std::cout << getFirstPattern(workingPatterns) << std::endl;
  }*/
  std::vector<std::unique_ptr<Pattern>> patternList;
  std::size_t pos = 0;
  int tries = 0;

  while (workingPatterns.size() > 0)
  {
    patternList.emplace_back(PatternFactory::generatePattern(workingPatterns));
    //std::cout << "Added " << patternList.back()->print() << std::endl;
  }

  while (pos < input_line.size() && tries < 100)
  {
    tries++;
    //std::cout << "test 0 " << pos << std::endl;
    pos = patternList[0]->find_first_of(pos, input_line);
    //std::cout << "test 1 " << pos << std::endl;

    if (pos == std::string::npos) // first pattern failed to find any more matches
    {
      //std::cout << "test failed\n";
      return false;
    }

    std::size_t newPos = findSubsequentPatterns(input_line, pos, 1, patternList);

    if (newPos != std::string::npos) // subsequent pattern was found
    {
      //std::cout << "test succeded\n";
      return true;
    }
  }

  return false;
}

int main(int argc, char* argv[])
{
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here" << std::endl;

  if (argc != 3)
  {
    std::cerr << "Expected two arguments" << std::endl;
    return 1;
  }

  std::string flag = argv[1];
  std::string pattern = argv[2];

  if (flag != "-E")
  {
    std::cerr << "Expected first argument to be '-E'" << std::endl;
    return 1;
  }

  std::string input_line;
  std::getline(std::cin, input_line);

  try
  {
    if (match_patterns(input_line, pattern))
    {
      std::cout << "found\n";
      return 0;
    }
    else
    {
      std::cout << "not found\n";
      return 1;
    }
  }
  catch (const std::runtime_error& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
