// Copyright 2023 PingCAP, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <Common/StringUtils/StringUtils.h>
#include <Parsers/CommonParsers.h>
#include <common/find_symbols.h>
#include <string.h> /// strncmp, strncasecmp


namespace DB
{

namespace ErrorCodes
{
extern const int LOGICAL_ERROR;
}


ParserKeyword::ParserKeyword(const char * s_)
    : s(s_)
{}


const char * ParserKeyword::getName() const
{
    return s;
}


bool ParserKeyword::parseImpl(Pos & pos, ASTPtr & /*node*/, Expected & expected)
{
    if (pos->type != TokenType::BareWord)
        return false;

    const char * current_word = s;

    size_t s_length = strlen(s);
    if (!s_length)
        throw Exception("Logical error: keyword cannot be empty string", ErrorCodes::LOGICAL_ERROR);

    const char * s_end = s + s_length;

    while (true)
    {
        expected.add(pos, current_word);
        if (pos->type != TokenType::BareWord)
            return false;

        const char * next_whitespace = find_first_symbols<' ', '\0'>(current_word, s_end);
        size_t word_length = next_whitespace - current_word;

        if (word_length != pos->size())
            return false;

        if (strncasecmp(pos->begin, current_word, word_length))
            return false;

        ++pos;

        if (!*next_whitespace)
            break;

        current_word = next_whitespace + 1;
    }

    return true;
}

} // namespace DB