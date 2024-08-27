#include "Patterns.hpp"

#include <iostream>
#include <string>
#include <vector>

std::size_t findSubsequentPatterns(const std::string& input, std::size_t pos, int pattern, const std::vector<std::shared_ptr<Pattern>>& patternList)
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

bool match_patterns(const std::string& input_line, const std::string& patterns)
{
  std::string workingPatterns = patterns;

  while (workingPatterns.size() > 0)
  {
    std::vector<std::shared_ptr<Pattern>> patternList;
    std::size_t pos = 0;
    int tries = 0;

    while (workingPatterns.size() > 0)
    {
      std::shared_ptr<Pattern> pattern = PatternFactory::generatePattern(workingPatterns);
      if (pattern == nullptr && workingPatterns.size() > 0) // reached an alternation pattern so try this pattern then next
        break;

      patternList.emplace_back(pattern);
      //std::cout << "test Added " << patternList.back()->print() << std::endl;
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
        break;
      }

      std::size_t newPos = findSubsequentPatterns(input_line, pos, 1, patternList);

      if (newPos != std::string::npos) // subsequent pattern was found
      {
        //std::cout << "test succeded\n";
        return true;
      }
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
