//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//

%{
// sdf/path.yy

#include "wabi/wabi.h"
#include "wabi/usd/sdf/pathParser.h"
#include "wabi/base/tf/diagnostic.h"
#include "wabi/base/tf/staticData.h"

#include <string>
#include <utility>
#include <vector>

using std::pair;
using std::string;
using std::vector;

WABI_NAMESPACE_USING

//--------------------------------------------------------------------
// Extern declarations to scanner data and functions
//--------------------------------------------------------------------
extern int pathYylex(YYSTYPE *yylval_param, yyscan_t yyscanner);
static void pathYyerror(Sdf_PathParserContext *context, const char *msg);

#define yyscanner context->scanner

%}

%token TOK_MAPPER
%token TOK_EXPRESSION
%token TOK_IDENTIFIER
%token TOK_NAMESPACED_IDENTIFIER
%token TOK_PRIM_NAME
%token TOK_VARIANT_NAME
%token TOK_DOTDOT
%token TOK_WHITESPACE

%define api.pure
%lex-param { yyscan_t yyscanner }
%parse-param { Sdf_PathParserContext *context }

%%

start : 
    path {
        swap(context->path, $1.path);
    };

keyword:
    TOK_MAPPER
    | TOK_EXPRESSION
    ;

// We accept general identifiers as well as our full set of keywords
// to ensure that we don't prevent people from using keywords for
// identifiers where it would not be ambiguous.
identifier:
    TOK_IDENTIFIER
    | keyword
    ;

namespaced_identifier:
    TOK_NAMESPACED_IDENTIFIER
    | identifier
    ;

path :
    '.' {
            $$.path = SdfPath::ReflexiveRelativePath();
        }
    | '/' {
            $$.path = SdfPath::AbsoluteRootPath();
        }
    | prim_path
    | dotdot_path
    | property_path
    ;

dotdot_path : 
    TOK_DOTDOT {
            $$.path = SdfPath::ReflexiveRelativePath().GetParentPath();
        }
    | dotdot_path '/' TOK_DOTDOT {
            $$.path = ($1).path.GetParentPath();
        }
    ;

prim_path : 
    prim_path_with_variant_selection
    | prim_path_sans_variant_selection
    ;

prim_path_sans_variant_selection : 
    prim_name {
            $$.path = SdfPath::ReflexiveRelativePath().AppendChild(($1).token);
        }
    | '/' prim_name {
            $$.path = SdfPath::AbsoluteRootPath().AppendChild(($2).token);
        }
    | dotdot_path '/' prim_name {
            $$.path = ($1).path.AppendChild(($3).token);
        }
    | prim_path_sans_variant_selection '/' prim_name {
            $$.path = ($1).path.AppendChild(($3).token);
        }
    | prim_path_with_variant_selection prim_name {
            $$.path = ($1).path.AppendChild(($2).token);
        }
    ;

prim_path_with_variant_selection :
    prim_path_sans_variant_selection {
            context->variantSelectionStack.push_back(Sdf_PathVariantSelections());
        }
    whitespace_opt variant_selection_list whitespace_opt {
            SdfPath newPath = $1.path;
            const Sdf_PathVariantSelections& selections =
                context->variantSelectionStack.back();
            for (size_t i = 0, n = selections.size(); i != n; ++i) {
                newPath = newPath.AppendVariantSelection(selections[i].first,
                                                         selections[i].second);
            }
            context->variantSelectionStack.pop_back();
            $$.path = std::move(newPath);
        }
    ;

variant_selection_list :
    variant_selection
    | variant_selection_list whitespace_opt variant_selection
    ;

variant_selection :
    '{' whitespace_opt
    prim_name whitespace_opt '=' whitespace_opt variant_name_opt '}' {
            context->variantSelectionStack.back().push_back(
                std::make_pair( $3.token, $7.token ) );
        }
    ;

prim_name : 
    TOK_PRIM_NAME 
    | identifier 
    ;

variant_name_opt :
    /* empty */ {
        $$.token = TfToken();
    }
    | variant_name whitespace_opt {
        $$.token = $1.token;
    }

/* Allow variant names to start with a dot */
variant_name : 
    TOK_VARIANT_NAME
    | prim_name
    | '.' TOK_VARIANT_NAME {
            $$.token = TfToken('.' + $2.token.GetString());
        }
    | '.' prim_name {
            $$.token = TfToken('.' + $2.token.GetString());
        }
    ;

property_path:
    prim_property_path
    | target_path
    | relational_attribute_path
    | relational_attribute_target_path
    | mapper_path
    | mapper_arg_path
    | expression_path
    ;

prim_property_path : 
    '.' namespaced_identifier {
            $$.path = SdfPath::ReflexiveRelativePath().AppendProperty($2.token);
        }
    | prim_path '.' namespaced_identifier {
            $$.path = $1.path.AppendProperty($3.token);
        }
    | dotdot_path '/' '.' namespaced_identifier {
            $$.path = $1.path.AppendProperty($4.token);
        }
    | '.' error {
            yyerror(context, "expected property name after '.'");
            YYABORT;
        }
    ;
    
target_path:
    prim_property_path '[' path ']' {
            $$.path = $1.path.AppendTarget($3.path);
        }
    | prim_property_path '[' error ']' {
            yyerror(context, "expected a path within [ ]"); 
            YYABORT;
        }
    ;

relational_attribute_path:
    target_path '.' namespaced_identifier {
            // The grammar does not know if the target path is already on
            // a relational attribute (ie is the connection target of a rel attr)
            // but SdfPath will detect this and return invalid path.
            // So we check for that here and make an error explicitly in that
            // case.
            $$.path = $1.path.AppendRelationalAttribute($3.token);
        }
    | target_path error { 
            yyerror(context, "expected a property for relationship target"); 
            YYABORT;
        }
    ;

relational_attribute_target_path:
    relational_attribute_path '[' path ']' {
            $$.path = $1.path.AppendTarget($3.path);
        }
    | relational_attribute_path '[' error ']' { 
            yyerror(context, "expected a path within [ ]"); 
            YYABORT;
        }
    ;
    
mapper_path:
    prim_property_path '.' TOK_MAPPER '[' path ']' {
            $$.path = $1.path.AppendMapper($5.path);
        }
    | relational_attribute_path '.' TOK_MAPPER '[' path ']' {
            $$.path = $1.path.AppendMapper($5.path);
        }
    ;

mapper_arg_path:
    mapper_path '.' identifier {
            $$.path = $1.path.AppendMapperArg($3.token);
        }
    ;

expression_path:
    prim_property_path '.' TOK_EXPRESSION {
            $$.path = $1.path.AppendExpression();
        }
    | relational_attribute_path '.' TOK_EXPRESSION {
            $$.path = $1.path.AppendExpression();
        }
    ;

whitespace_opt : 
    /* empty */
    | TOK_WHITESPACE
    ;

%%

static void 
pathYyerror(Sdf_PathParserContext *context, const char *msg) 
{
    TF_AXIOM(context);
    context->path = SdfPath();
    TF_AXIOM(msg);
    context->errStr = msg;
    context->variantSelectionStack.clear();
}

