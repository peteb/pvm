// -*- c++ -*-

#ifndef _BYTECODE_PARSER_H
#define _BYTECODE_PARSER_H

#include <cstddef>
#include <vector>
#include <functional>
#include <memory>
#include <stack>

#include "class_file.h"

namespace bytecode {
  class class_file;

  class parser {
  public:
    parser();

    /**
     * Parses so many instructions it can. Saves the leftover data
     * for the next invocation.
     */
    void parse(const char *data, size_t size);

    std::shared_ptr<class_file> release();

  private:
    /** PARSER STATES */
    class parser_state {
    public:
      virtual bool parse(parser &in) =0;
    };

    class header_state : public parser_state {
    public:
      bool parse(parser &in);
    };

    class constant_pool_state : public parser_state {
    public:
      bool parse(parser &in);

    private:
      std::size_t items_left;
      bool parsed_header = false;
    };

    class constant_pool_item_state : public parser_state {
    public:
      bool parse(parser &in);
    };

    class midriff_state : public parser_state {
    public:
      bool parse(parser &in);
    };

    class interfaces_state : public parser_state {
    public:
      bool parse(parser &in);
    };

    class fields_state : public parser_state {
    public:
      fields_state(std::vector<field> *receiver)
        : receiver(receiver)
      {}

      bool parse(parser &in);

    private:
      std::size_t items_left;
      bool parsed_header = false;
      std::vector<field> *receiver;
    };

    class field_item_state : public parser_state {
    public:
      field_item_state(std::vector<field> *receiver)
        : receiver(receiver)
      {}

      bool parse(parser &in);

    private:
      bytecode::field current_field;
      bool parsed_header = false;
      std::size_t attributes_left;
      std::vector<field> *receiver;
    };

    class attribute_items_state : public parser_state {
    public:
      attribute_items_state(std::size_t num_items, std::vector<attribute_info_t *> *receiver)
        : items_left(num_items)
        , receiver(receiver)
      {}

      bool parse(parser &in);

    private:
      std::size_t items_left;
      std::vector<attribute_info_t *> *receiver;
    };

    /** THE REST */
    void dump_buffer();

    template<typename T>
    T *peek(std::size_t bytes = sizeof(T)) {
      if (cursor + bytes > buffer.size()) {
        return nullptr;
      }

      return reinterpret_cast<T *>(buffer.data() + cursor);
    }

    template<typename T>
    T *read(std::size_t bytes = sizeof(T)) {
      if (cursor + bytes > buffer.size()) {
        return nullptr;
      }

      T *ret = reinterpret_cast<T *>(buffer.data() + cursor);
      cursor += bytes;
      return ret;
    }

    void replace_state(parser_state *new_state);
    void push_state(parser_state *new_state);
    void pop_state();


    bool try_parse();
    /*    bool parse_header();
    bool parse_cp();
    bool parse_cp_item();
    bool parse_midriff();
    bool parse_if();
    bool parse_fields();
    bool parse_field_item();
    bool parse_methods();
    bool parse_method_items();
    bool parse_attributes();
    bool parse_attribute_items();
    */
    std::stack<std::unique_ptr<parser_state>> state;
    std::vector<std::unique_ptr<parser_state>> destruct;

    std::vector<char> buffer;
    std::ptrdiff_t cursor = 0;
    std::shared_ptr<class_file> object;
    /*    std::size_t cp_items_left = 0;
    std::size_t field_items_left = 0;
    std::size_t method_items_left = 0;
    std::size_t attribute_items_left = 0;

    field_info constructing_field_info;*/
  };

}

#endif // !_BYTECODE_PARSER_H
