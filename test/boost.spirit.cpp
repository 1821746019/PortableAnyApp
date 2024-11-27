#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_string.hpp>
import std;

using namespace std;

using namespace boost;
// 定义数据结构
//struct RegEntry {
//  std::string key;
//  std::vector<std::pair<std::string, std::string>> values;
//};
//
//int main() {
//  using namespace boost::spirit::qi;
//  using boost::spirit::ascii::char_;
//  //using boost::spirit::ascii::eol;
//  using boost::spirit::ascii::space;
//  using boost::spirit::ascii::xdigit;
//
//  std::string input = R"(
//Windows Registry Editor Version 5.00
//
//[HKEY_CURRENT_USER\Software\Example]
//"StringValue"="Example String"
//"DWORDValue"=dword:00000001
//)";
//
//  RegEntry entry;
//  auto it = input.begin();
//  auto end = input.end();
//
//  bool success = phrase_parse(
//      it, end,
//      // Grammar
//      (
//          // 版本声明
//          lit("Windows Registry Editor Version") >> "5.00" >> *eol >>
//          // 键路径
//          '[' >> +(char_ - ']')[boost::phoenix::ref(entry.key) += _1] >> ']' >>
//          *eol >>
//          // 键值对
//          *('"' >>
//            +(char_ - '"')[boost::phoenix::push_back(
//                boost::phoenix::ref(entry.values),
//                boost::phoenix::construct<std::pair<std::string, std::string>>(
//                    _1, ""))] >>
//            '"' >> '=' >>
//            (
//                // 字符串值
//                '"' >> *(char_ - '"')[boost::phoenix::ref(
//                                          entry.values.back().second) += _1] >>
//                    '"' |
//                // DWORD 值
//                "dword:" >> +xdigit[boost::phoenix::ref(
//                                        entry.values.back().second) += _1]) >>
//            *eol)),
//      // 解析时跳过空白
//      space);
//
//  if (success && it == end) {
//    std::cout << "解析成功！\n";
//    std::cout << "键路径: " << entry.key << "\n";
//    for (const auto& kv : entry.values) {
//      std::cout << "键名: " << kv.first << "，值: " << kv.second << "\n";
//    }
//  } else {
//    std::cout << "解析失败！\n";
//  }
//
//  return 0;
//}
// 定义数据结构
struct RegValue {
  std::string name;
  std::string data;
};

struct RegEntry {
  std::string key;
  std::vector<RegValue> values;
};

BOOST_FUSION_ADAPT_STRUCT(RegValue, (std::string, name)(std::string, data))

BOOST_FUSION_ADAPT_STRUCT(RegEntry,
                          (std::string, key)(std::vector<RegValue>, values))

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template <typename Iterator>
struct RegParser
    : qi::grammar<Iterator, std::vector<RegEntry>(), ascii::space_type> {
  RegParser() : RegParser::grammar(start) {
    using ascii::alnum;
    using ascii::blank;
    using ascii::space;
    using qi::char_;
    using qi::eol;
    using qi::eps;
    using qi::lexeme;
    using qi::lit;
    using qi::omit;
    using qi::string;

    // 定义解析规则
    version_line =
        lexeme["Windows Registry Editor Version" >> *(char_ - eol)] >> eol;

    comment_line = lexeme[';' >> *(char_ - eol)] >> eol;

    blank_line = eol;

    ignored_line = (version_line | comment_line | blank_line);

    quoted_string %= lexeme['"' >> *(char_ - '"') >> '"'];

    key_name %= '[' >> *(char_ - ']') >> ']';

    value %= quoted_string >> '=' >> quoted_string;

    entry %= key_name >> *(value);

    start %= *(ignored_line | entry);
  }

  qi::rule<Iterator, ascii::space_type> version_line;
  qi::rule<Iterator, ascii::space_type> comment_line;
  qi::rule<Iterator, ascii::space_type> blank_line;
  qi::rule<Iterator, ascii::space_type> ignored_line;
  qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
  qi::rule<Iterator, std::string(), ascii::space_type> key_name;
  qi::rule<Iterator, RegValue(), ascii::space_type> value;
  qi::rule<Iterator, RegEntry(), ascii::space_type> entry;
  qi::rule<Iterator, std::vector<RegEntry>(), ascii::space_type> start;
};

int main() {
  std::string reg_data = R"(Windows Registry Editor Version 5.00

[HKEY_CURRENT_USER\Software\MyApp]
"Setting1"="Value1"
"Setting2"="Value2"

[HKEY_CURRENT_USER\Software\MyApp\SubKey]
"SubSetting"="SubValue"
)";

  typedef std::string::const_iterator iterator_type;
  typedef RegParser<iterator_type> parser_type;

  parser_type parser;            // 实例化解析器
  std::vector<RegEntry> result;  // 存储解析结果
  iterator_type iter = reg_data.begin();
  iterator_type end = reg_data.end();

  bool r = phrase_parse(iter, end, parser, ascii::space, result);

  if (r && iter == end) {
    std::cout << "解析成功！\n";
    // 输出解析结果
    for (const auto& entry : result) {
      std::cout << "Key: " << entry.key << "\n";
      for (const auto& val : entry.values) {
        std::cout << "  Name: " << val.name << ", Data: " << val.data << "\n";
      }
    }
  } else {
    std::cout << "解析失败！\n";
  }

  return 0;
}