#include <stdlib.h>
#include <string.h>

#include "pattern.h"
#include "strbuild.h"

static char *pattern_serialize_literal_atom(struct PatternLiteralAtom *literal_atom)
{
	char *result = NULL;

	switch (literal_atom->type) {
	case PATTERN_LITERAL_ATOM_UNIT:
		str_append(result, "unit");
		break;

	case PATTERN_LITERAL_ATOM_BOOL:
		if (literal_atom->data.boolean) {
			str_append(result, "true");
		} else {
			str_append(result, "false");
		}
		break;

	case PATTERN_LITERAL_ATOM_INT:
		str_append(result, "%d", literal_atom->data.integer);
		break;

	case PATTERN_LITERAL_ATOM_REAL:
		str_append(result, "%f", literal_atom->data.real);
		break;

	case PATTERN_LITERAL_ATOM_CHAR:
		str_append(result, "%c", literal_atom->data.character);
		break;

	case PATTERN_LITERAL_ATOM_STRING:
		str_append(result, "\"%s\"", literal_atom->data.string);
		break;
	}

	return result;
}

static char *pattern_serialize_literal_compound(struct PatternLiteralCompound *literal_compound)
{
	char *result = NULL;
	char *temp = NULL;
	struct Pattern *child = literal_compound->children;

	switch (child->type)
	{
	case PATTERN_LITERAL_CPD_ARRAY:
		str_append(result, "[");
		break;
	case PATTERN_LITERAL_CPD_TUPLE:
		str_append(result, "{");
		break;
	}

	while (child)
	{
		char *child_str = pattern_serialize(child);
		str_append(result, " %s", child_str);
		mem_free(child_str);
		child = child->next;
	}

	switch (child->type)
	{
	case PATTERN_LITERAL_CPD_ARRAY:
		str_append(result, " ]");
		break;
	case PATTERN_LITERAL_CPD_TUPLE:
		str_append(result, " }");
		break;
	}

	return result;
}

static char *pattern_serialize_datatype(struct PatternDataType *data_type)
{
	char *result = NULL;
	char *temp;

	switch (data_type->type)
	{
	case PATTERN_DATATYPE_UNIT:
		str_append(result, "UNIT");
		break;

	case PATTERN_DATATYPE_BOOLEAN:
		str_append(result, "BOOLEAN");
		break;

	case PATTERN_DATATYPE_INTEGER:
		str_append(result, "INTEGER");
		break;

	case PATTERN_DATATYPE_REAL:
		str_append(result, "REAL");
		break;

	case PATTERN_DATATYPE_CHARACTER:
		str_append(result, "CHARACTER");
		break;

	case PATTERN_DATATYPE_ARRAY_OF:
		str_append(result, "(ARRAY-OF");
		temp = pattern_serialize(data_type->children);
		str_append(result, " %s)", temp);
		mem_free(temp);
		break;

	case PATTERN_DATATYPE_REFERENCE_TO:
		str_append(result, "REFERENCE-TO");
		temp = pattern_serialize(data_type->children);
		str_append(result, " %s)", temp);
		mem_free(temp);
		break;

	case PATTERN_DATATYPE_FUNCTION:
		str_append(result, "FUNCTION");
		break;
	}

	return result;
}

char *pattern_serialize(struct Pattern *pattern)
{
	char *result = NULL;
	bool first = true;

	while (pattern) {
		char *temp = NULL;

		switch (pattern->type) {
		case PATTERN_DONT_CARE:
			str_append(temp, "_");
			break;

		case PATTERN_SYMBOL:
			str_append(temp, "%s", pattern->data.symbol.symbol);
			break;

		case PATTERN_LITERAL_ATOM:
			temp = pattern_serialize_literal_atom(&pattern->data.literal_atom);
			break;

		case PATTERN_LITERAL_COMPOUND:
			temp = pattern_serialize_literal_compound(&pattern->data.literal_compound);
			break;

		case PATTERN_DATATYPE:
			temp = pattern_serialize_datatype(&pattern->data.datatype);
			break;
		}

		if (first) {
			first = false;
			str_append(result, "%s", temp);
		} else {
			str_append(result, " %s", temp);
		}

		mem_free(temp);
		pattern = pattern->next;
	}

	return result;
}