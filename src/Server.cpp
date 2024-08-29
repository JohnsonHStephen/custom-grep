#include "Patterns.hpp"

#include <iostream>
#include <string>

/**********************************************************************
 * TODO: support multiple possible answers for references (and alternatives)
 *        The following are examples that dont work but should
 *          echo -n "aa" | ./your_program.sh -E "(a+)a)"
 *          echo -n "aa" | ./your_program.sh -E "(aa.)a)"
 *          echo -n "aa" | ./your_program.sh -E "(a+a.)a)"
 *          echo -n "abc-def is abc-def, not efg, abc, or def" | ./your_program.sh -E "(([abc]+)-([def]+)) is \1, not ([^xyz]+), \2, or \3"
 *********************************************************************/

/**********************************************************************
 * Tests that currently work and shouldn't break:
 *  found:      echo -n "'cat and cat' is the same as 'cat and cat'" | ./your_program.sh -E "('(cat) and \2') is the same as \1"
 *  not found:  echo -n "'cat and cat' is the same as 'cat and dog'" | ./your_program.sh -E "('(cat) and \2') is the same as \1"
 *  found:      echo -n "grep 101 is doing grep 101 times, and again grep 101 times" | ./your_program.sh -E "((\w\w\w\w) (\d\d\d)) is doing \2 \3 times, and again \1 times"
 *  not found:  echo -n "$?! 101 is doing $?! 101 times, and again $?! 101 times" | ./your_program.sh -E "((\w\w\w) (\d\d\d)) is doing \2 \3 times, and again \1 times"
 *  not found:  echo -n "grep yes is doing grep yes times, and again grep yes times" | ./your_program.sh -E "((\w\w\w\w) (\d\d\d)) is doing \2 \3 times, and again \1 times"
 *********************************************************************/


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
    PatternHandler handler = PatternHandler();
    if (handler.match_patterns(input_line, pattern) != std::string::npos)
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
