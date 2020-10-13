#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

class NginxConfigParserTestFixture : public ::testing::Test {
 protected:
  NginxConfigParser parser;
  NginxConfig out_config;
  // void SetUp() override { }
  // void TearDown() override { }
};

TEST_F(NginxConfigParserTestFixture, Empty_Config) {
  bool success = parser.Parse("empty_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, Nonexistent_Config) {
  bool isFalse = parser.Parse("non_existent_config", &out_config);
  EXPECT_FALSE(isFalse);
}

// Test for config file with only braces (or no token)
TEST_F(NginxConfigParserTestFixture, BracesConfig) {
	bool success = parser.Parse("braces_config", &out_config);
	
	EXPECT_FALSE(success);
}

// Test for config file with uneven number of start and end braces
TEST_F(NginxConfigParserTestFixture, UnevenBracesConfig) {
	bool success = parser.Parse("uneven_braces_config", &out_config);
	
	EXPECT_FALSE(success);
}

// Test for config file with an empty block - found bug
TEST_F(NginxConfigParserTestFixture, EmptyBlockConfig) {
	bool success = parser.Parse("empty_block_config", &out_config);
	
	EXPECT_TRUE(success);
}

// Test for config file with nested block - found bug
TEST_F(NginxConfigParserTestFixture, NestedBlock) {
	bool success = parser.Parse("nested_block_config", &out_config);
	
	EXPECT_TRUE(success);
}

// Test for config file with a comment
TEST_F(NginxConfigParserTestFixture, CommentConfig) {
	bool success = parser.Parse("comment_config", &out_config);
	
	EXPECT_TRUE(success);
}

// Test for config file with consecutive tokens
TEST_F(NginxConfigParserTestFixture, ConsecutiveTokenConfig) {
	bool success = parser.Parse("consecutive_token_config", &out_config);
	
	EXPECT_TRUE(success);
}

// Test for config file with mulitple statements
TEST_F(NginxConfigParserTestFixture, MultipleStatementsConfig) {
	bool success = parser.Parse("multiple_statements_config", &out_config);
	
	EXPECT_TRUE(success);
}

// Test for config file with double quotes
TEST_F(NginxConfigParserTestFixture, DoubleQuotesConfig) {
	bool success = parser.Parse("double_quotes_config", &out_config);
	
	EXPECT_TRUE(success);
}

// Test for config file with statement with no following semicolon
TEST_F(NginxConfigParserTestFixture, NoSemicolonConfig) {
	bool success = parser.Parse("no_semicolon_config", &out_config);
	
	EXPECT_FALSE(success);
}

// Test for config file with statement with no end block
TEST_F(NginxConfigParserTestFixture, MissingEndBlockConfig) {
	bool success = parser.Parse("missing_end_block_config", &out_config);
	
	EXPECT_FALSE(success);
}

// Test for config file with statement multiple semicolons
TEST_F(NginxConfigParserTestFixture, MultipleSemicolonsConfig) {
	bool success = parser.Parse("multiple_semicolons_config", &out_config);
	
	EXPECT_FALSE(success);
}

// // Test for config file with single quote
TEST_F(NginxConfigParserTestFixture, SingleQuoteConfig) {
	bool success = parser.Parse("single_quote_config", &out_config);
	
	EXPECT_TRUE(success);
}

// Test for config file with unfinished single quote
TEST_F(NginxConfigParserTestFixture, UnfinishedQuoteConfig) {
	bool success = parser.Parse("unfinished_quote_config", &out_config);
	
	EXPECT_FALSE(success);
}

// Test for config file with unfinished double quote
TEST_F(NginxConfigParserTestFixture, UnfinishedDoubleQuoteConfig) {
	bool success = parser.Parse("unfinished_double_quote_config", &out_config);
	
	EXPECT_FALSE(success);
}

// Test if getConfigValue returns correct value
TEST_F(NginxConfigParserTestFixture, ConfigValue) {
  bool success = parser.Parse("example_config", &out_config);
	int portNum = stoi(out_config.getConfigValue("listen"));
	
	EXPECT_EQ(portNum, 80);
}

// Test if getConfigValue returns correct value with empty config file
TEST_F(NginxConfigParserTestFixture, ConfigValueEmpty) {
  bool success = parser.Parse("empty_config", &out_config);
	std::string portNum = out_config.getConfigValue("listen");
	
	EXPECT_EQ(portNum, "  ");
}

// Test if getConfigValue returns correct value with empty config file
TEST_F(NginxConfigParserTestFixture, ConfigToString) {
  bool success = parser.Parse("example_config", &out_config);
	std::string configString = out_config.ToString(1);
  std::string expectedString = "  foo \"bar\";\n  server {\n    listen 80;\n    server_name foo.com;\n    root /home/ubuntu/sites/foo/;\n  }\n";

	EXPECT_EQ(configString, expectedString);
}
