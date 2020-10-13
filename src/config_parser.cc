// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "config_parser.h"

std::string NginxConfig::ToString(int depth) {
  std::string serialized_config;
  for (const auto& statement : statements_) {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

std::string NginxConfigStatement::ToString(int depth) {
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  } else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    default:                       return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        // TODO: the end of a quoted token should be followed by whitespace.
        // TODO: Maybe also define a QUOTED_STRING token type.
        // TODO: Allow for backslash-escaping within strings.
        *value += c;
        if (c == '\'') {
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;
        if (c == '"') {
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);
    //printf ("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } else if (token_type == TOKEN_TYPE_NORMAL) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL) {
        if (last_token_type != TOKEN_TYPE_NORMAL) {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL ) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          // new case for nested case and empty bracket
          last_token_type != TOKEN_TYPE_START_BLOCK &&
          last_token_type != TOKEN_TYPE_END_BLOCK) {
        // Error.
        break;
      }
      config_stack.pop();
    } else if (token_type == TOKEN_TYPE_EOF) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK &&
          last_token_type != TOKEN_TYPE_START ) {
        // Error.
        break;
      }
      if(config_stack.size()- 1 != 0 )
        break;
      return true;
    } else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  printf ("Bad transition from %s to %s\n",
          TokenTypeAsString(last_token_type),
          TokenTypeAsString(token_type));
  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    printf ("Failed to open config file: %s\n", file_name);
    return false;
  }

  const bool return_value =
      Parse(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();
  return return_value;
}

//https://cgit.freedesktop.org/libreoffice/online/tree/wsd/LOOLWSD.cpp
std::string NginxConfig::getConfigValue(std::string file ) const {
  for (size_t i = 0; i < statements_.size(); i++) {
    if (statements_[i]->tokens_[0] == file) {
      std::string token_val = "";
      for (size_t j = 1; j < statements_[i]->tokens_.size(); j++) {
        if (j != 1) {
          token_val += " "; // add space in between tokens
        }
        token_val += statements_[i]->tokens_[j];
      }
      return token_val;
    }
    else if (statements_[i]->child_block_ != nullptr) {
      std::string childRes = statements_[i]->child_block_->getConfigValue(file);
      std::cout << childRes << std::endl;
      if (childRes != " ")
        return childRes;
    }
  }

  return "  ";
}

bool NginxConfig::isPortValid( int port)
{
  return ((port >= this->MIN_PORT_NUM)&&(port <= this->MAX_PORT_NUM)) ? true: false; 
}
int NginxConfig::getPort() {
  for (size_t i = 0; i < statements_.size(); i++) {
    if (statements_[i]->tokens_[0] == "listen") {
      std::string str_port = "";
      for (size_t j = 1; j < statements_[i]->tokens_.size(); j++) {
        if (j != 1) {
          str_port += " "; // add space in between tokens
        }
        str_port += statements_[i]->tokens_[j];
      }
      int port_num = stoi(str_port);
      return (isPortValid(port_num))? port_num: -1;
    }
    else if (statements_[i]->child_block_ != nullptr) {
      int child_block_ret = statements_[i]->child_block_->getPort();
      std::cout << child_block_ret << std::endl;
      if (child_block_ret != -1)
        return child_block_ret;
    }
  }

  return -1; // both single block and child block retrived bad port input
}

// helper function to get mappings for handlers to paths
std::map<std::string, std::pair<std::string, std::vector<std::string>>> NginxConfig::getMappings() {
  // Vector of string
  std::map<std::string, std::pair<std::string, std::vector<std::string>>> handlerMappings;

  // parse through statements
  for (size_t i = 0; i < statements_.size(); i++) {
    // look for "location" statement
    if (statements_[i]->tokens_[0] == "location") {
      // there should really only be 3 total tokens and a child block
      if (statements_[i]->tokens_.size() == 3) {
        std::string path_name = statements_[i]->tokens_[1];
        path_name = path_name.substr(1, path_name.size()-2);  // remove quotation marks
        
        std::string handler_type = statements_[i]->tokens_[2];
        
        std::string root_val = ""; // base directory if static handler
        std::vector<std::string> temp;

        if (handler_type == "StaticHandler" && statements_[i]->child_block_ != nullptr) {
          // child block should only have "root" statement
          root_val = statements_[i]->child_block_->getConfigValue("root");
          root_val = root_val.substr(1, root_val.size()-2);  // remove quotation marks
          temp.push_back(root_val);
        } else if (handler_type == "ProxyHandler" && statements_[i]->child_block_ != nullptr) {
          // Configurable host.
          root_val = statements_[i]->child_block_->getConfigValue("host");
          root_val = root_val.substr(1, root_val.size()-2);
          temp.push_back(root_val);
          std::string root_val_2 = statements_[i]->child_block_->getConfigValue("port_number");
          root_val_2 = root_val_2.substr(1, root_val_2.size()-2);
          temp.push_back(root_val_2);
        }

        std::pair<std::string, std::vector<std::string>> path_root_pair(handler_type, temp);
        std::pair<std::string, std::pair<std::string, std::vector<std::string>>> handler_paths_pair(path_name, path_root_pair);
        handlerMappings.insert(handler_paths_pair);
      }
    }
  }

  return handlerMappings;
}
