#include <algorithm>
#include <iostream>
#include <string>

bool match_pattern(const std::string& input_line, const std::string& pattern)
{
  if (pattern.length() == 1) // literal character
  {
    return input_line.find(pattern) != std::string::npos;
  }
  else if (pattern.compare("\\d") == 0) // any digit
  {
    return std::any_of(input_line.begin(), input_line.end(), ::isdigit);
  }
  else if (pattern.compare("\\w") == 0) // any alpha numeric
  {
    return std::any_of(input_line.begin(), input_line.end(), ::isalnum);
  }
  else if (int startPos = pattern.find("[") != std::string::npos) // positive character group
  {
    struct Contains
    {
        const std::string d;
        Contains(const std::string &n) : d(n) {}
        bool operator()(char n) const { return d.find(n) != std::string::npos; }
    };

    int endPos = pattern.find("]");
    return std::any_of(input_line.begin(), input_line.end(), Contains(pattern.substr(startPos, endPos - startPos)));
  }
  else
  {
    throw std::runtime_error("Unhandled pattern " + pattern);
  }
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
    if (match_pattern(input_line, pattern))
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
