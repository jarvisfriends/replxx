#include <gtest/gtest.h>
#include <replxx.hxx>

#include <memory>
#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace replxx {

  class replxx_cpp_interface_test : public ::testing::Test {

  public:
    replxx_cpp_interface_test() = default;

    ~replxx_cpp_interface_test() override = default;

// prototypes
    static Replxx::completions_t hook_completion(std::string const& context, int index, void* user_data);
    static Replxx::hints_t hook_hint(std::string const& context, int index, Replxx::Color& color, void* user_data);
    static void hook_color(std::string const& context, Replxx::colors_t& colors, void* user_data);

  protected:
    void TearDown() override {
    }

    void SetUp() override {
    }

  };

  Replxx::completions_t replxx_cpp_interface_test::hook_completion(std::string const& context, int index, void* user_data) {
    auto* examples = static_cast<std::vector<std::string>*>(user_data);
    Replxx::completions_t completions;

    std::string prefix {context.substr(index)};
    for (auto const& e : *examples) {
      if (e.compare(0, prefix.size(), prefix) == 0) {
        completions.emplace_back(e.c_str());
      }
    }

    return completions;
  }

  Replxx::hints_t replxx_cpp_interface_test::hook_hint(std::string const& context, int index, Replxx::Color& color, void* user_data) {
    auto* examples = static_cast<std::vector<std::string>*>(user_data);
    Replxx::hints_t hints;

    // only show hint if prefix is at least 'n' chars long
    // or if prefix begins with a specific character
    std::string prefix {context.substr(index)};
    if (prefix.size() >= 2 || (! prefix.empty() && prefix.at(0) == '.')) {
      for (auto const& e : *examples) {
        if (e.compare(0, prefix.size(), prefix) == 0) {
          hints.emplace_back(e.substr(prefix.size()).c_str());
        }
      }
    }

    // set hint color to green if single match found
    if (hints.size() == 1) {
      color = Replxx::Color::GREEN;
    }

    return hints;
  }

  void replxx_cpp_interface_test::hook_color(std::string const& context, Replxx::colors_t& colors, void* user_data) {
    auto* regex_color = static_cast<std::vector<std::pair<std::string, Replxx::Color>>*>(user_data);

    // highlight matching regex sequences
    for (auto const& e : *regex_color) {
      size_t pos {0};
      std::string str = context;
      std::smatch match;

      while(std::regex_search(str, match, std::regex(e.first))) {
        std::string c {match[0]};
        pos += std::string(match.prefix()).size();

        for (size_t i = 0; i < c.size(); ++i) {
          colors.at(pos + i) = e.second;
        }

        pos += c.size();
        str = match.suffix();
      }
    }
  }

  TEST(replxx_cpp_interface_test, cpp_example) {

    // words to be completed
    std::vector<std::string> examples {
        ".help", ".history", ".quit", ".exit", ".clear", ".prompt ",
        "hello", "world", "db", "data", "drive", "print", "put",
        "color_black", "color_red", "color_green", "color_brown", "color_blue",
        "color_magenta", "color_cyan", "color_lightgray", "color_gray",
        "color_brightred", "color_brightgreen", "color_yellow", "color_brightblue",
        "color_brightmagenta", "color_brightcyan", "color_white", "color_normal",
    };

    // highlight specific words
    // a regex string, and a color
    // the order matters, the last match will take precedence
    using cl = Replxx::Color;
    std::vector<std::pair<std::string, cl>> regex_color {
        // single chars
        {"\\`", cl::BRIGHTCYAN},
        {"\\'", cl::BRIGHTBLUE},
        {"\\\"", cl::BRIGHTBLUE},
        {"\\-", cl::BRIGHTBLUE},
        {"\\+", cl::BRIGHTBLUE},
        {"\\=", cl::BRIGHTBLUE},
        {"\\/", cl::BRIGHTBLUE},
        {"\\*", cl::BRIGHTBLUE},
        {"\\^", cl::BRIGHTBLUE},
        {"\\.", cl::BRIGHTMAGENTA},
        {"\\(", cl::BRIGHTMAGENTA},
        {"\\)", cl::BRIGHTMAGENTA},
        {"\\[", cl::BRIGHTMAGENTA},
        {"\\]", cl::BRIGHTMAGENTA},
        {"\\{", cl::BRIGHTMAGENTA},
        {"\\}", cl::BRIGHTMAGENTA},

        // color keywords
        {"color_black", cl::BLACK},
        {"color_red", cl::RED},
        {"color_green", cl::GREEN},
        {"color_brown", cl::BROWN},
        {"color_blue", cl::BLUE},
        {"color_magenta", cl::MAGENTA},
        {"color_cyan", cl::CYAN},
        {"color_lightgray", cl::LIGHTGRAY},
        {"color_gray", cl::GRAY},
        {"color_brightred", cl::BRIGHTRED},
        {"color_brightgreen", cl::BRIGHTGREEN},
        {"color_yellow", cl::YELLOW},
        {"color_brightblue", cl::BRIGHTBLUE},
        {"color_brightmagenta", cl::BRIGHTMAGENTA},
        {"color_brightcyan", cl::BRIGHTCYAN},
        {"color_white", cl::WHITE},
        {"color_normal", cl::NORMAL},

        // commands
        {"\\.help", cl::BRIGHTMAGENTA},
        {"\\.history", cl::BRIGHTMAGENTA},
        {"\\.quit", cl::BRIGHTMAGENTA},
        {"\\.exit", cl::BRIGHTMAGENTA},
        {"\\.clear", cl::BRIGHTMAGENTA},
        {"\\.prompt", cl::BRIGHTMAGENTA},

        // numbers
        {"[\\-|+]{0,1}[0-9]+", cl::YELLOW}, // integers
        {"[\\-|+]{0,1}[0-9]*\\.[0-9]+", cl::YELLOW}, // decimals
        {"[\\-|+]{0,1}[0-9]+e[\\-|+]{0,1}[0-9]+", cl::YELLOW}, // scientific notation

        // strings
        {"\".*?\"", cl::BRIGHTGREEN}, // double quotes
        {"\'.*?\'", cl::BRIGHTGREEN}, // single quotes
    };

    // init the repl
    std::unique_ptr<Replxx> p_replxx{new Replxx()};
    EXPECT_EQ(0, p_replxx->install_window_change_handler());

    // the path to the history file
    std::string history_file {"./replxx_history.txt"};

    // touch the file for this test
    std::ofstream outfile(history_file);
    outfile << "My first history" << std::endl;
    outfile.close();

    // load the history file if it exists
    EXPECT_EQ(0, p_replxx->history_load(history_file));

    EXPECT_EQ(1, p_replxx->history_size());
    // History_line is 0 based
    EXPECT_EQ("My first history", p_replxx->history_line(0));
    // Clear History
    p_replxx->set_max_history_size(0);
    EXPECT_EQ(0, p_replxx->history_size());

    // set the max history size
    p_replxx->set_max_history_size(12);

    // set the max input line size
    p_replxx->set_max_line_size(128);

    // set the max number of hint rows to show
    p_replxx->set_max_hint_rows(8);

    // set the callbacks
    p_replxx->set_completion_callback(replxx_cpp_interface_test::hook_completion, static_cast<void*>(&examples));
    p_replxx->set_highlighter_callback(replxx_cpp_interface_test::hook_color, static_cast<void*>(&regex_color));
    p_replxx->set_hint_callback(replxx_cpp_interface_test::hook_hint, static_cast<void*>(&examples));

    std::string prompt {"\x1b[1;32mreplxx\x1b[0m> "};

    p_replxx->history_add("first history value");
    p_replxx->history_add("second \"history\" value");
    p_replxx->history_add("int 42");
    p_replxx->history_add("double 53.8");

    EXPECT_EQ(4, p_replxx->history_size());
    // History_line is 0 based
    EXPECT_EQ("int 42", p_replxx->history_line(2));

    // clear the screen
    //p_replxx->clear_screen();
  }

}