#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "strbuild.h"
#include "log.h"

static char *ast_serialize_control_do(struct AstCtlDo *doo)
{
	char *result = NULL;
	struct AstNode *expr;

	str_append(result, "(do");
	for (expr = doo->exprs; expr; expr = expr->next) {
		char *temp = ast_serialize(expr);
		str_append(result, " %s", temp);
		mem_free(temp);
	}
	str_append(result, ")");

	return result;
}

static char *ast_serialize_control_bind(struct AstCtlBind *bind)
{
	char *result = NULL, *pat_string, *expr_string;

	str_append(result, "(bind");

	pat_string = pattern_serialize(bind->pattern);
	str_append(result, " %s", pat_string);
	mem_free(pat_string);

	expr_string = ast_serialize(bind->expr);
	str_append(result, " %s)", expr_string);
	mem_free(expr_string);

	return result;
}

static char *ast_serialize_control_match(struct AstCtlMatch *match)
{
	char *result = NULL;
	char *expr_string;
	struct Pattern *key = match->keys;
	struct AstNode *value = match->values;

	expr_string = ast_serialize(match->expr);
	str_append(result, "(match %s", expr_string);
	mem_free(expr_string);

	while (key && value) {
		char *key_string = pattern_serialize(key);
		char *value_string = ast_serialize(value);
		str_append(result, " %s %s", key_string, value_string);
		mem_free(value_string);
		mem_free(key_string);
		key = key->next;
		value = value->next;
	}

	if (key || value) {
		LOG_ERROR("Corrupt match node");
		exit(1);
	}

	str_append(result, ")");

	return result;
}

static char *ast_serialize_control_fdef(struct AstCtlFuncDef *fdef)
{
	char *result = NULL;
	char *arg_string;
	char *expr_string;

	arg_string = pattern_serialize(fdef->formal_args);
	str_append(result, "(func (%s) ", arg_string);
	mem_free(arg_string);

	expr_string = ast_serialize(fdef->expr);
	str_append(result, ") %s)", expr_string);
	mem_free(expr_string);

	return result;
}

static char *ast_serialize_control_fcall(struct AstCtlFuncCall *fcall)
{
	char *result = NULL;
	char *func_string;
	struct AstNode *arg = fcall->actual_args;

	func_string = ast_serialize(fcall->func);
	str_append(result, "(%s", func_string);
	mem_free(func_string);

	while (arg) {
		char *arg_string = ast_serialize(arg);
		str_append(result, " %s", arg_string);
		mem_free(arg_string);
		arg = arg->next;
	}

	str_append(result, ")");

	return result;
}

static char *ast_serialize_control_reference(struct AstCtlReference *reference)
{
	int len = strlen(reference->symbol);
	char *result = mem_malloc(len + 1);
	memcpy(result, reference->symbol, len + 1);
	return result;
}

static char *ast_serialize_control(struct AstControl *control)
{
	char *result = NULL;

	switch (control->type) {
	case AST_CTL_DO:
		result = ast_serialize_control_do(&control->data.doo);
		break;

	case AST_CTL_BIND:
		result = ast_serialize_control_bind(&control->data.bind);
		break;

	case AST_CTL_MATCH:
		result = ast_serialize_control_match(&control->data.match);
		break;

	case AST_CTL_FUNC_DEF:
		result = ast_serialize_control_fdef(&control->data.fdef);
		break;

	case AST_CTL_FUNC_CALL:
		result = ast_serialize_control_fcall(&control->data.fcall);
		break;

	case AST_CTL_REFERENCE:
		result = ast_serialize_control_reference(&control->data.reference);
		break;
	}

	return result;
}

static char *ast_serialize_parafunc(struct AstParafunc *parafunc)
{
	char *result = NULL;
	char *temp = NULL;
	struct AstNode *arg = parafunc->args;

	switch (parafunc->type) {
	case AST_PARAFUNC_IF:
		str_append(result, "(if");
		break;
	case AST_PARAFUNC_WHILE:
		str_append(result, "(while");
		break;
	case AST_PARAFUNC_AND:
		str_append(result, "(and");
		break;
	case AST_PARAFUNC_OR:
		str_append(result, "(or");
		break;
	case AST_PARAFUNC_REF:
		str_append(result, "(ref");
		break;
	case AST_PARAFUNC_PEEK:
		str_append(result, "(peek");
		break;
	case AST_PARAFUNC_POKE:
		str_append(result, "(poke");
		break;
	case AST_PARAFUNC_BEGIN:
		str_append(result, "(begin");
		break;
	case AST_PARAFUNC_END:
		str_append(result, "(end");
		break;
	case AST_PARAFUNC_INC:
		str_append(result, "(inc");
		break;
	case AST_PARAFUNC_SUCC:
		str_append(result, "(succ");
		break;
	}

	while (arg) {
		char *child_str = ast_serialize(arg);
		str_append(result, " %s", child_str);
		mem_free(child_str);
		arg = arg->next;
	}

	str_append(result, " )");

	return result;
}

static char *ast_serialize_compound(struct AstCompound *compound)
{
	char *result = NULL;
	char *temp = NULL;
	struct AstNode *expr = compound->exprs;

	switch (compound->type) {
	case AST_CPD_ARRAY:
		str_append(result, "[");
		break;
	case AST_CPD_TUPLE:
		str_append(result, "{");
		break;
	}

	while (expr) {
		char *expr_str = ast_serialize(expr);
		str_append(result, " %s", expr_str);
		mem_free(expr_str);
		expr = expr->next;
	}

	switch (compound->type) {
	case AST_CPD_ARRAY:
		str_append(result, " ]");
		break;
	case AST_CPD_TUPLE:
		str_append(result, " }");
		break;
	}

	return result;
}

char *ast_serialize_literal(struct AstLiteral *literal)
{
	char *result = NULL;

	switch (literal->type) {
	case AST_LIT_UNIT:
		str_append(result, "unit");
		break;
	case AST_LIT_BOOL:
		if (literal->data.boolean) {
			str_append(result, "true");
		}
		else {
			str_append(result, "false");
		}
		break;
	case AST_LIT_STRING:
		str_append(result, "%s", literal->data.string);
		break;
	case AST_LIT_CHAR:
		str_append(result, "%c", literal->data.character);
		break;
	case AST_LIT_INT:
		str_append(result, "%d", literal->data.integer);
		break;
	case AST_LIT_REAL:
		str_append(result, "%f", literal->data.real);
		break;
	}

	return result;
}

char *ast_serialize(struct AstNode *node)
{
	char *result = NULL;
	bool first = true;

	while (node) {
		char *temp = NULL;
		switch (node->type) {
		case AST_CONTROL:
			temp = ast_serialize_control(&node->data.control);
			break;
		case AST_PARAFUNC:
			temp = ast_serialize_parafunc(&node->data.parafunc);
			break;
		case AST_COMPOUND:
			temp = ast_serialize_compound(&node->data.compound);
			break;
		case AST_LITERAL:
			temp = ast_serialize_literal(&node->data.literal);
			break;
		}

		if (first) {
			first = false;
			str_append(result, "%s", temp);
		} else {
			str_append(result, " %s", temp);
		}

		mem_free(temp);
		node = node->next;
	}
	return result;
}