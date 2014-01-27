/* Copyright (C) 2014 Krzysztof Stachowiak */

/*
 * This file is part of moon.
 *
 * moon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * moon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with moon. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>

#include "common/logg.h"
#include "sexpr/tok.h"
#include "sexpr/dom.h"
#include "ast/ast.h"

int main()
{
        char *source;

        struct tok_def *tokens;
        int tok_count;

        struct dom_node *dom_root;

        struct ast_module *module;

        source = "(module Main\n"
                 "(func main () \n"
                 "        (PRINT \"Hello, World\\n\")\n"
                 "        0\n"
                 ")\n"
                 ")";

        LOG_DEBUG("Tokenizing source:\n%s\n", source);

        tokens = NULL;
        tok_count = -1;
        if (!tokenize(source, &tokens, &tok_count)) {
                LOG_DEBUG("Tokenizer error.");
                return EXIT_FAILURE;
        }

        LOG_DEBUG("Parsed %d tokens.\n", tok_count);

        dom_root = NULL;
        if (!domize(tokens, tok_count, &dom_root)) {
                LOG_DEBUG("Domize error.");
                free(tokens);
                return EXIT_FAILURE;
        }

        if (dom_root)
                LOG_DEBUG("Parsing DOM [SUCCESS].\n");
        else
                LOG_DEBUG("Parsing DOM [FAILURE].\n");

        free(tokens);

        module = ast_parse_module(dom_root);

        if (module)
                LOG_DEBUG("Parsing module [SUCCESS].\n");
        else
                LOG_DEBUG("Parsing module [FAILURE].\n");

        dom_delete_node(dom_root);

        ast_delete_module(module);

        return EXIT_SUCCESS;
}
