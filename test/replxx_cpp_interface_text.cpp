#include <gtest/gtest.h>
#include <replxx.hxx>

#include <memory>
#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace replxx {


int context_len( char const* prefix ) {
  char const wb[] = " \t\n\r\v\f-=+*&^%$#@!,./?<>;:`~'\"[]{}()\\|";
  int i = strlen( prefix ) - 1;
  int cl = 0;
  while ( i >= 0 ) {
    if ( strchr( wb, prefix[i] ) != nullptr ) {
      break;
    }
    ++ cl;
    -- i;
  }
  return ( cl );
}
int utf8str_codepoint_len( char const* s, int utf8len ) {
  int codepointLen = 0;
  unsigned char m4 = 128 + 64 + 32 + 16;
  unsigned char m3 = 128 + 64 + 32;
  unsigned char m2 = 128 + 64;
  for ( int i = 0; i < utf8len; ++ i, ++ codepointLen ) {
    char c = s[i];
    if ( ( c & m4 ) == m4 ) {
      i += 3;
    } else if ( ( c & m3 ) == m3 ) {
      i += 2;
    } else if ( ( c & m2 ) == m2 ) {
      i += 1;
    }
  }
  return ( codepointLen );
}

  class replxx_cpp_interface_test : public ::testing::Test {

  public:
    replxx_cpp_interface_test() = default;

    ~replxx_cpp_interface_test() override = default;

// prototypes
    static Replxx::completions_t hook_completion(std::string const& context, int index, const std::vector<std::string>& examples);
    static Replxx::hints_t hook_hint(std::string const& input, int& contextLen, Replxx::Color& color, const std::vector<std::string>& examples);
    static void hook_color(std::string const& input, Replxx::colors_t& colors, const std::vector<std::pair<std::string, Replxx::Color>>& regex_color);

  protected:
    void TearDown() override {
    }

    void SetUp() override {
    }

  };

  Replxx::completions_t replxx_cpp_interface_test::hook_completion(std::string const& context, int index, const std::vector<std::string>& examples) {
    Replxx::completions_t completions;

    std::string prefix {context.substr(index)};
    for (auto const& e : examples) {
      if (e.compare(0, prefix.size(), prefix) == 0) {
        completions.emplace_back(e.c_str());
      }
    }

    return completions;
  }

  Replxx::hints_t replxx_cpp_interface_test::hook_hint(std::string const& input, int& contextLen, Replxx::Color& color, const std::vector<std::string>& examples) {
    Replxx::hints_t hints;

    // only show hint if prefix is at least 'n' chars long
    // or if prefix begins with a specific character
    int utf8ContextLen( context_len( input.c_str() ) );
    int prefixLen( input.length() - utf8ContextLen );
    contextLen = utf8str_codepoint_len( input.c_str() + prefixLen, utf8ContextLen );
    std::string prefix {input.substr(prefixLen)};
    if (prefix.size() >= 2 || (! prefix.empty() && prefix.at(0) == '.')) {
      for (auto const& e : examples) {
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

  void replxx_cpp_interface_test::hook_color(std::string const& input, Replxx::colors_t& colors, const std::vector<std::pair<std::string, Replxx::Color>>& regex_color) {
    // highlight matching regex sequences
    for (auto const& e : regex_color) {
      size_t pos {0};
      std::string str = input;
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
    auto p_replxx = std::make_unique<Replxx>();
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

    // set the max number of hint rows to show
    p_replxx->set_max_hint_rows(8);

    // set the callbacks
    p_replxx->set_completion_callback([examples](std::string const& input, int& contextLen) {
      return replxx_cpp_interface_test::hook_completion(input, contextLen, examples);
    } );
    p_replxx->set_highlighter_callback([regex_color](std::string const& input, Replxx::colors_t& colors) {
      replxx_cpp_interface_test::hook_color(input, colors, regex_color);
    } );
    p_replxx->set_hint_callback([examples](std::string const& input, int& contextLen, Replxx::Color& color) {
      return replxx_cpp_interface_test::hook_hint(input, contextLen, color, examples);
    } );

    // other api calls
    p_replxx->set_word_break_characters( " \t.,-%!;:=*~^'\"/?<>|[](){}" );
    //p_replxx->set_special_prefixes( "\\" );
    p_replxx->set_completion_count_cutoff( 128 );
    p_replxx->set_double_tab_completion( false );
    p_replxx->set_complete_on_empty( true );
    p_replxx->set_beep_on_ambiguous_completion( false );
    p_replxx->set_no_color( false );

    std::string prompt {"\x1b[1;32mreplxx\x1b[0m> "};

    p_replxx->history_add("first history value");
    p_replxx->history_add("second \"history\" value");
    p_replxx->history_add("int 42");
    p_replxx->history_add("double 53.8");
    std::string fake_user_result = "test output verified";
    p_replxx->print("%s\n", fake_user_result.c_str());

    EXPECT_EQ(4, p_replxx->history_size());
    // History_line is 0 based
    EXPECT_EQ("int 42", p_replxx->history_line(2));

    // clear the screen
    //p_replxx->clear_screen();
  }

}