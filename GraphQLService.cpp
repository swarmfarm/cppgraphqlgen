// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "GraphQLGrammar.h"

#include <graphqlservice/GraphQLService.h>

#include <iostream>
#include <algorithm>
#include <array>

namespace facebook {
namespace graphql {
namespace service {

schema_exception::schema_exception(std::vector<std::string>&& messages)
	: _errors(response::Type::List)
{
	for (auto& message : messages)
	{
		response::Value error(response::Type::Map);

		error.emplace_back("message", response::Value(std::move(message)));
		_errors.emplace_back(std::move(error));
	}

	messages.clear();
}

const response::Value& schema_exception::getErrors() const noexcept
{
	return _errors;
}

Fragment::Fragment(const peg::ast_node& fragmentDefinition)
	: _type(fragmentDefinition.children[1]->children.front()->content())
	, _selection(*(fragmentDefinition.children.back()))
{
}

const std::string& Fragment::getType() const
{
	return _type;
}

const peg::ast_node& Fragment::getSelection() const
{
	return _selection;
}

uint8_t Base64::verifyFromBase64(char ch)
{
	uint8_t result = fromBase64(ch);

	if (result > 63)
	{
		throw schema_exception({ "invalid character in base64 encoded string" });
	}

	return result;
}

std::vector<uint8_t> Base64::fromBase64(const char* encoded, size_t count)
{
	std::vector<uint8_t> result;

	if (!count)
	{
		return result;
	}

	result.reserve((count + (count % 4)) * 3 / 4);

	// First decode all of the full unpadded segments 24 bits at a time
	while (count >= 4
		&& encoded[3] != padding)
	{
		const uint32_t segment = (static_cast<uint32_t>(verifyFromBase64(encoded[0])) << 18)
			| (static_cast<uint32_t>(verifyFromBase64(encoded[1])) << 12)
			| (static_cast<uint32_t>(verifyFromBase64(encoded[2])) << 6)
			| static_cast<uint32_t>(verifyFromBase64(encoded[3]));

		result.emplace_back(static_cast<uint8_t>((segment & 0xFF0000) >> 16));
		result.emplace_back(static_cast<uint8_t>((segment & 0xFF00) >> 8));
		result.emplace_back(static_cast<uint8_t>(segment & 0xFF));

		encoded += 4;
		count -= 4;
	}

	// Get any leftover partial segment with 2 or 3 non-padding characters
	if (count > 1)
	{
		const bool triplet = (count > 2 && padding != encoded[2]);
		const uint8_t tail = (triplet ? verifyFromBase64(encoded[2]) : 0);
		const uint16_t segment = (static_cast<uint16_t>(verifyFromBase64(encoded[0])) << 10)
			| (static_cast<uint16_t>(verifyFromBase64(encoded[1])) << 4)
			| (static_cast<uint16_t>(tail) >> 2);

		if (triplet)
		{
			if (tail & 0x3)
			{
				throw schema_exception({ "invalid padding at the end of a base64 encoded string" });
			}

			result.emplace_back(static_cast<uint8_t>((segment & 0xFF00) >> 8));
			result.emplace_back(static_cast<uint8_t>(segment & 0xFF));

			encoded += 3;
			count -= 3;
		}
		else
		{
			if (segment & 0xFF)
			{
				throw schema_exception({ "invalid padding at the end of a base64 encoded string" });
			}

			result.emplace_back(static_cast<uint8_t>((segment & 0xFF00) >> 8));

			encoded += 2;
			count -= 2;
		}
	}

	// Make sure anything that's left is 0 - 2 characters of padding
	if ((count > 0 && padding != encoded[0])
		|| (count > 1 && padding != encoded[1])
		|| count > 2)
	{
		throw schema_exception({ "invalid padding at the end of a base64 encoded string" });
	}

	return result;
}

char Base64::verifyToBase64(uint8_t i)
{
	unsigned char result = toBase64(i);

	if (result == padding)
	{
		throw schema_exception({ "invalid 6-bit value" });
	}

	return result;
}

std::string Base64::toBase64(const std::vector<uint8_t>& bytes)
{
	std::string result;

	if (bytes.empty())
	{
		return result;
	}

	size_t count = bytes.size();
	const uint8_t* data = bytes.data();

	result.reserve((count + (count % 3)) * 4 / 3);

	// First encode all of the full unpadded segments 24 bits at a time
	while (count >= 3)
	{
		const uint32_t segment = (static_cast<uint32_t>(data[0]) << 16)
			| (static_cast<uint32_t>(data[1]) << 8)
			| static_cast<uint32_t>(data[2]);

		result.append({
			verifyToBase64((segment & 0xFC0000) >> 18),
			verifyToBase64((segment & 0x3F000) >> 12),
			verifyToBase64((segment & 0xFC0) >> 6),
			verifyToBase64(segment & 0x3F)
			});

		data += 3;
		count -= 3;
	}

	// Get any leftover partial segment with 1 or 2 bytes
	if (count > 0)
	{
		const bool pair = (count > 1);
		const uint16_t segment = (static_cast<uint16_t>(data[0]) << 8)
			| (pair ? static_cast<uint16_t>(data[1]) : 0);
		const std::array<char, 4> remainder {
			verifyToBase64((segment & 0xFC00) >> 10),
			verifyToBase64((segment & 0x3F0) >> 4),
			(pair ? verifyToBase64((segment & 0xF) << 2) : padding),
			padding
		};

		result.append(remainder.data(), remainder.size());
	}

	return result;
}

template <>
response::IntType ModifiedArgument<response::IntType>::convert(const response::Value& value)
{
	if (value.type() != response::Type::Int)
	{
		throw schema_exception({ "not an integer" });
	}

	return value.get<response::IntType>();
}

template <>
response::FloatType ModifiedArgument<response::FloatType>::convert(const response::Value& value)
{
	if (value.type() != response::Type::Float)
	{
		throw schema_exception({ "not a float" });
	}

	return value.get<response::FloatType>();
}

template <>
response::StringType ModifiedArgument<response::StringType>::convert(const response::Value& value)
{
	if (value.type() != response::Type::String)
	{
		throw schema_exception({ "not a string" });
	}

	return value.get<const response::StringType&>();
}

template <>
response::BooleanType ModifiedArgument<response::BooleanType>::convert(const response::Value& value)
{
	if (value.type() != response::Type::Boolean)
	{
		throw schema_exception({ "not a boolean" });
	}

	return value.get<response::BooleanType>();
}

template <>
response::Value ModifiedArgument<response::Value>::convert(const response::Value& value)
{
	if (value.type() != response::Type::Map)
	{
		throw schema_exception({ "not an object" });
	}

	return response::Value(value);
}

template <>
std::vector<uint8_t> ModifiedArgument<std::vector<uint8_t>>::convert(const response::Value& value)
{
	if (value.type() != response::Type::String)
	{
		throw schema_exception({ "not a string" });
	}

	const auto& encoded = value.get<const response::StringType&>();

	return Base64::fromBase64(encoded.c_str(), encoded.size());
}

template <>
std::future<response::Value> ModifiedResult<response::IntType>::convert(std::future<response::IntType>&& result, ResolverParams&&)
{
	return std::async(std::launch::deferred,
		[](std::future<response::IntType>&& resultFuture)
	{
		return response::Value(resultFuture.get());
	}, std::move(result));
}

template <>
std::future<response::Value> ModifiedResult<response::FloatType>::convert(std::future<response::FloatType>&& result, ResolverParams&&)
{
	return std::async(std::launch::deferred,
		[](std::future<response::FloatType>&& resultFuture)
	{
		return response::Value(resultFuture.get());
	}, std::move(result));
}

template <>
std::future<response::Value> ModifiedResult<response::StringType>::convert(std::future<response::StringType>&& result, ResolverParams&& params)
{
	return std::async(std::launch::deferred,
		[&](std::future<response::StringType>&& resultFuture, ResolverParams&& paramsFuture)
	{
		return response::Value(resultFuture.get());
	}, std::move(result), std::move(params));
}

template <>
std::future<response::Value> ModifiedResult<response::BooleanType>::convert(std::future<response::BooleanType>&& result, ResolverParams&&)
{
	return std::async(std::launch::deferred,
		[](std::future<response::BooleanType>&& resultFuture)
	{
		return response::Value(resultFuture.get());
	}, std::move(result));
}

template <>
std::future<response::Value> ModifiedResult<response::Value>::convert(std::future<response::Value>&& result, ResolverParams&&)
{
	return std::move(result);
}

template <>
std::future<response::Value> ModifiedResult<std::vector<uint8_t>>::convert(std::future<std::vector<uint8_t>>&& result, ResolverParams&& params)
{
	return std::async(std::launch::deferred,
		[](std::future<std::vector<uint8_t>>&& resultFuture, ResolverParams&& paramsFuture)
	{
		return response::Value(Base64::toBase64(resultFuture.get()));
	}, std::move(result), std::move(params));
}

template <>
std::future<response::Value> ModifiedResult<Object>::convert(std::future<std::shared_ptr<Object>> result, ResolverParams&& params)
{
	return std::async(std::launch::deferred,
		[](std::future<std::shared_ptr<Object>>&& resultFuture, ResolverParams&& paramsFuture)
	{
		auto wrappedResult = resultFuture.get();

		if (!wrappedResult || !paramsFuture.selection)
		{
			return response::Value(!wrappedResult
				? response::Type::Null
				: response::Type::Map);
		}

		return wrappedResult->resolve(paramsFuture.state, *paramsFuture.selection, paramsFuture.fragments, paramsFuture.variables).get();
	}, std::move(result), std::move(params));
}

// ValueVisitor visits the AST and builds a response::Value representation of any value
// hardcoded or referencing a variable in an operation.
class ValueVisitor
{
public:
	ValueVisitor(const response::Value& variables);

	void visit(const peg::ast_node& value);

	response::Value getValue();

private:
	void visitVariable(const peg::ast_node& variable);
	void visitIntValue(const peg::ast_node& intValue);
	void visitFloatValue(const peg::ast_node& floatValue);
	void visitStringValue(const peg::ast_node& stringValue);
	void visitBooleanValue(const peg::ast_node& booleanValue);
	void visitNullValue(const peg::ast_node& nullValue);
	void visitEnumValue(const peg::ast_node& enumValue);
	void visitListValue(const peg::ast_node& listValue);
	void visitObjectValue(const peg::ast_node& objectValue);

	const response::Value& _variables;
	response::Value _value;
};

ValueVisitor::ValueVisitor(const response::Value& variables)
	: _variables(variables)
{
}

response::Value ValueVisitor::getValue()
{
	auto result = std::move(_value);

	return result;
}

void ValueVisitor::visit(const peg::ast_node& value)
{
	if (value.is<peg::variable_value>())
	{
		visitVariable(value);
	}
	else if (value.is<peg::integer_value>())
	{
		visitIntValue(value);
	}
	else if (value.is<peg::float_value>())
	{
		visitFloatValue(value);
	}
	else if (value.is<peg::string_value>())
	{
		visitStringValue(value);
	}
	else if (value.is<peg::true_keyword>()
		|| value.is<peg::false_keyword>())
	{
		visitBooleanValue(value);
	}
	else if (value.is<peg::null_keyword>())
	{
		visitNullValue(value);
	}
	else if (value.is<peg::enum_value>())
	{
		visitEnumValue(value);
	}
	else if (value.is<peg::list_value>())
	{
		visitListValue(value);
	}
	else if (value.is<peg::object_value>())
	{
		visitObjectValue(value);
	}
}

void ValueVisitor::visitVariable(const peg::ast_node& variable)
{
	const std::string name(variable.content().c_str() + 1);
	auto itr = _variables.find(name);

	if (itr == _variables.get<const response::MapType&>().cend())
	{
		auto position = variable.begin();
		std::ostringstream error;

		error << "Unknown variable name: " << name
			<< " line: " << position.line
			<< " column: " << position.byte_in_line;

		throw schema_exception({ error.str() });
	}

	_value = response::Value(itr->second);
}

void ValueVisitor::visitIntValue(const peg::ast_node& intValue)
{
	_value = response::Value(std::atoi(intValue.content().c_str()));
}

void ValueVisitor::visitFloatValue(const peg::ast_node& floatValue)
{
	_value = response::Value(std::atof(floatValue.content().c_str()));
}

void ValueVisitor::visitStringValue(const peg::ast_node& stringValue)
{
	_value = response::Value(std::string(stringValue.unescaped));
}

void ValueVisitor::visitBooleanValue(const peg::ast_node& booleanValue)
{
	_value = response::Value(booleanValue.is<peg::true_keyword>());
}

void ValueVisitor::visitNullValue(const peg::ast_node& /*nullValue*/)
{
	_value = {};
}

void ValueVisitor::visitEnumValue(const peg::ast_node& enumValue)
{
	_value = response::Value(enumValue.content());
}

void ValueVisitor::visitListValue(const peg::ast_node& listValue)
{
	_value = response::Value(response::Type::List);
	_value.reserve(listValue.children.size());

	ValueVisitor visitor(_variables);

	for (const auto& child : listValue.children)
	{
		visitor.visit(*child->children.back());
		_value.emplace_back(visitor.getValue());
	}
}

void ValueVisitor::visitObjectValue(const peg::ast_node& objectValue)
{
	_value = response::Value(response::Type::Map);
	_value.reserve(objectValue.children.size());

	ValueVisitor visitor(_variables);

	for (const auto& field : objectValue.children)
	{
		visitor.visit(*field->children.back());
		_value.emplace_back(field->children.front()->content(), visitor.getValue());
	}
}

// SelectionVisitor visits the AST and resolves a field or fragment, unless it's skipped by
// a directive or type condition.
class SelectionVisitor
{
public:
	SelectionVisitor(const std::shared_ptr<RequestState>& state, const FragmentMap& fragments, const response::Value& variables, const TypeNames& typeNames, const ResolverMap& resolvers);

	void visit(const peg::ast_node& selection);

	std::future<response::Value> getValues();

private:
	response::Value getDirectives(const peg::ast_node& directives) const;
	bool shouldSkip(const response::Value& directives) const;

	void visitField(const peg::ast_node& field);
	void visitFragmentSpread(const peg::ast_node& fragmentSpread);
	void visitInlineFragment(const peg::ast_node& inlineFragment);

	const std::shared_ptr<RequestState>& _state;
	const FragmentMap& _fragments;
	const response::Value& _variables;
	const TypeNames& _typeNames;
	const ResolverMap& _resolvers;

	std::queue<std::pair<std::string, std::future<response::Value>>> _values;
};

SelectionVisitor::SelectionVisitor(const std::shared_ptr<RequestState>& state, const FragmentMap& fragments, const response::Value& variables, const TypeNames& typeNames, const ResolverMap& resolvers)
	: _state(state)
	, _fragments(fragments)
	, _variables(variables)
	, _typeNames(typeNames)
	, _resolvers(resolvers)
{
}

std::future<response::Value> SelectionVisitor::getValues()
{
	return std::async(std::launch::deferred,
		[](std::queue<std::pair<std::string, std::future<response::Value>>>&& values)
		{
			response::Value result(response::Type::Map);

			while (!values.empty())
			{
				auto& entry = values.front();

				result.emplace_back(std::move(entry.first), entry.second.get());
				values.pop();
			}

			return result;
		}, std::move(_values));
}

void SelectionVisitor::visit(const peg::ast_node& selection)
{
	if (selection.is<peg::field>())
	{
		visitField(selection);
	}
	else if (selection.is<peg::fragment_spread>())
	{
		visitFragmentSpread(selection);
	}
	else if (selection.is<peg::inline_fragment>())
	{
		visitInlineFragment(selection);
	}
}

void SelectionVisitor::visitField(const peg::ast_node& field)
{
	std::string name;

	peg::on_first_child<peg::field_name>(field,
		[&name](const peg::ast_node& child)
		{
			name = child.content();
		});

	std::string alias;

	peg::on_first_child<peg::alias_name>(field,
		[&alias](const peg::ast_node& child)
		{
			alias = child.content();
		});

	if (alias.empty())
	{
		alias = name;
	}

	const auto itr = _resolvers.find(name);

	if (itr == _resolvers.cend())
	{
		auto position = field.begin();
		std::ostringstream error;

		error << "Unknown field name: " << name
			<< " line: " << position.line
			<< " column: " << position.byte_in_line;

		throw schema_exception({ error.str() });
	}

	bool skip = false;

	response::Value directives(response::Type::Map);

	peg::on_first_child<peg::directives>(field,
		[this, &directives](const peg::ast_node& child)
		{
			directives = getDirectives(child);
		});

	if (shouldSkip(directives))
	{
		return;
	}

	response::Value arguments(response::Type::Map);

	peg::on_first_child<peg::arguments>(field,
		[this, &arguments](const peg::ast_node& child)
		{
			ValueVisitor visitor(_variables);

			for (auto& argument : child.children)
			{
				visitor.visit(*argument->children.back());

				arguments.emplace_back(argument->children.front()->content(), visitor.getValue());
			}
		});

	const peg::ast_node* selection = nullptr;

	peg::on_first_child<peg::selection_set>(field,
		[&selection](const peg::ast_node& child)
		{
			selection = &child;
		});

	_values.push({
		std::move(alias),
		itr->second({ _state, std::move(arguments), std::move(directives), selection, _fragments, _variables })
		});
}

void SelectionVisitor::visitFragmentSpread(const peg::ast_node& fragmentSpread)
{
	const std::string name(fragmentSpread.children.front()->content());
	auto itr = _fragments.find(name);

	if (itr == _fragments.cend())
	{
		auto position = fragmentSpread.begin();
		std::ostringstream error;

		error << "Unknown fragment name: " << name
			<< " line: " << position.line
			<< " column: " << position.byte_in_line;

		throw schema_exception({ error.str() });
	}

	bool skip = (_typeNames.count(itr->second.getType()) == 0);

	if (!skip)
	{
		response::Value directives(response::Type::Map);

		peg::on_first_child<peg::directives>(fragmentSpread,
			[this, &directives](const peg::ast_node& child)
			{
				directives = getDirectives(child);
			});

		skip = shouldSkip(directives);
	}

	if (skip)
	{
		return;
	}

	for (const auto& selection : itr->second.getSelection().children)
	{
		visit(*selection);
	}
}

void SelectionVisitor::visitInlineFragment(const peg::ast_node& inlineFragment)
{
	response::Value directives(response::Type::Map);

	peg::on_first_child<peg::directives>(inlineFragment,
		[this, &directives](const peg::ast_node& child)
		{
			directives = getDirectives(child);
		});

	if (shouldSkip(directives))
	{
		return;
	}

	const peg::ast_node* typeCondition = nullptr;

	peg::on_first_child<peg::type_condition>(inlineFragment,
		[&typeCondition](const peg::ast_node& child)
		{
			typeCondition = &child;
		});

	if (typeCondition == nullptr
		|| _typeNames.count(typeCondition->children.front()->content()) > 0)
	{
		peg::on_first_child<peg::selection_set>(inlineFragment,
			[this](const peg::ast_node& child)
			{
				for (const auto& selection : child.children)
				{
					visit(*selection);
				}
			});
	}
}

response::Value SelectionVisitor::getDirectives(const peg::ast_node& directives) const
{
	response::Value result(response::Type::Map);

	for (const auto& directive : directives.children)
	{
		std::string directiveName;

		peg::on_first_child<peg::directive_name>(*directive,
			[&directiveName](const peg::ast_node& child)
			{
				directiveName = child.content();
			});

		if (directiveName.empty())
		{
			continue;
		}

		response::Value directiveArguments(response::Type::Map);

		peg::on_first_child<peg::arguments>(*directive,
			[this, &directiveArguments](const peg::ast_node& child)
			{
				ValueVisitor visitor(_variables);

				for (auto& argument : child.children)
				{
					visitor.visit(*argument->children.back());

					directiveArguments.emplace_back(argument->children.front()->content(), visitor.getValue());
				}
			});

		result.emplace_back(std::move(directiveName), std::move(directiveArguments));
	}

	return result;
}

bool SelectionVisitor::shouldSkip(const response::Value& directives) const
{
	static const std::array<std::pair<bool, std::string>, 2> skippedNames = {
		std::make_pair<bool, std::string>(true, "skip"),
		std::make_pair<bool, std::string>(false, "include"),
	};

	for (const auto& entry : skippedNames)
	{
		const bool skip = entry.first;
		auto itrDirective = directives.find(entry.second);

		if (itrDirective == directives.end())
		{
			continue;
		}

		auto& arguments = itrDirective->second;

		if (arguments.type() != response::Type::Map)
		{
			std::ostringstream error;

			error << "Invalid arguments to directive: " << entry.second;

			throw schema_exception({ error.str() });
		}

		bool argumentTrue = false;
		bool argumentFalse = false;

		for (auto& argument : arguments)
		{
			if (argumentTrue
				|| argumentFalse
				|| argument.second.type() != response::Type::Boolean
				|| argument.first != "if")
			{
				std::ostringstream error;

				error << "Invalid argument to directive: " << entry.second
					<< " name: " << argument.first;

				throw schema_exception({ error.str() });
			}

			argumentTrue = argument.second.get<response::BooleanType>();
			argumentFalse = !argumentTrue;
		}

		if (argumentTrue)
		{
			return skip;
		}
		else if (argumentFalse)
		{
			return !skip;
		}
		else
		{
			std::ostringstream error;

			error << "Missing argument to directive: " << entry.second
				<< " name: if";

			throw schema_exception({ error.str() });
		}
	}

	return false;
}

Object::Object(TypeNames&& typeNames, ResolverMap&& resolvers)
	: _typeNames(std::move(typeNames))
	, _resolvers(std::move(resolvers))
{
}

std::future<response::Value> Object::resolve(const std::shared_ptr<RequestState>& state, const peg::ast_node& selection, const FragmentMap& fragments, const response::Value& variables) const
{
	std::queue<std::future<response::Value>> selections;

	beginSelectionSet(state);

	for (const auto& child : selection.children)
	{
		SelectionVisitor visitor(state, fragments, variables, _typeNames, _resolvers);

		visitor.visit(*child);
		selections.push(visitor.getValues());
	}

	endSelectionSet(state);

	return std::async(std::launch::deferred,
		[](std::queue<std::future<response::Value>>&& promises)
	{
		response::Value result(response::Type::Map);

		while (!promises.empty())
		{
			auto values = promises.front().get();

			promises.pop();

			if (values.type() == response::Type::Map)
			{
				auto members = values.release<response::MapType>();

				for (auto& entry : members)
				{
					result.emplace_back(std::move(entry.first), std::move(entry.second));
				}
			}
		}

		return result;
	}, std::move(selections));
}

void Object::beginSelectionSet(const std::shared_ptr<RequestState>&) const
{
}

void Object::endSelectionSet(const std::shared_ptr<RequestState>&) const
{
}

OperationData::OperationData(std::shared_ptr<RequestState>&& state, response::Value&& variables, FragmentMap&& fragments)
	: state(std::move(state))
	, variables(std::move(variables))
	, fragments(std::move(fragments))
{
}

// FragmentDefinitionVisitor visits the AST and collects all of the fragment
// definitions in the document.
class FragmentDefinitionVisitor
{
public:
	FragmentDefinitionVisitor();

	FragmentMap getFragments();

	void visit(const peg::ast_node& fragmentDefinition);

private:
	FragmentMap _fragments;
};

FragmentDefinitionVisitor::FragmentDefinitionVisitor()
{
}

FragmentMap FragmentDefinitionVisitor::getFragments()
{
	FragmentMap result(std::move(_fragments));
	return result;
}

void FragmentDefinitionVisitor::visit(const peg::ast_node& fragmentDefinition)
{
	_fragments.insert({ fragmentDefinition.children.front()->content(), Fragment(fragmentDefinition) });
}

// OperationDefinitionVisitor visits the AST and executes the one with the specified
// operation name.
class OperationDefinitionVisitor
{
public:
	OperationDefinitionVisitor(std::shared_ptr<RequestState> state, const TypeMap& operations, const std::string& operationName, response::Value&& variables, FragmentMap&& fragments);

	std::future<response::Value> getValue();

	void visit(const peg::ast_node& operationDefinition);

private:
	std::shared_ptr<OperationData> _params;
	const TypeMap& _operations;
	const std::string& _operationName;
	std::future<response::Value> _result;
};

OperationDefinitionVisitor::OperationDefinitionVisitor(std::shared_ptr<RequestState> state, const TypeMap& operations, const std::string& operationName, response::Value&& variables, FragmentMap&& fragments)
	: _params(std::make_shared<OperationData>(
		std::move(state),
		std::move(variables),
		std::move(fragments)))
	, _operations(operations)
	, _operationName(operationName)
{
}

std::future<response::Value> OperationDefinitionVisitor::getValue()
{
	auto result = std::move(_result);

	try
	{
		if (!result.valid())
		{
			std::ostringstream error;

			error << "Missing operation";

			if (!_operationName.empty())
			{
				error << " name: " << _operationName;
			}

			throw schema_exception({ error.str() });
		}
	}
	catch (const schema_exception& ex)
	{
		std::promise<response::Value> promise;
		response::Value document(response::Type::Map);

		document.emplace_back("data", response::Value());
		document.emplace_back("errors", response::Value(ex.getErrors()));
		promise.set_value(std::move(document));

		result = promise.get_future();
	}

	return result;
}

void OperationDefinitionVisitor::visit(const peg::ast_node& operationDefinition)
{
	std::string operation;

	peg::on_first_child<peg::operation_type>(operationDefinition,
		[&operation](const peg::ast_node& child)
		{
			operation = child.content();
		});

	if (operation.empty())
	{
		operation = "query";
	}
	else if (operation == "subscription")
	{
		// Skip subscription operations, they should use subscribe instead of resolve.
		return;
	}

	auto position = operationDefinition.begin();
	std::string name;

	peg::on_first_child<peg::operation_name>(operationDefinition,
		[&name](const peg::ast_node& child)
		{
			name = child.content();
		});

	if (!_operationName.empty()
		&& name != _operationName)
	{
		// Skip the operations that don't match the name
		return;
	}

	try
	{
		if (_result.valid())
		{
			std::ostringstream error;

			if (_operationName.empty())
			{
				error << "No operationName specified with extra operation";
			}
			else
			{
				error << "Duplicate operation";
			}

			if (!name.empty())
			{
				error << " name: " << name;
			}

			error << " line: " << position.line
				<< " column: " << position.byte_in_line;

			throw schema_exception({ error.str() });
		}

		auto itr = _operations.find(operation);

		if (itr == _operations.cend())
		{
			std::ostringstream error;

			error << "Unknown operation type: " << operation;

			if (!name.empty())
			{
				error << " name: " << name;
			}

			error << " line: " << position.line
				<< " column: " << position.byte_in_line;

			throw schema_exception({ error.str() });
		}

		// Filter the variable definitions down to the ones referenced in this operation
		response::Value operationVariables(response::Type::Map);

		peg::for_each_child<peg::variable>(operationDefinition,
			[this, &operationVariables](const peg::ast_node& variable)
			{
				std::string variableName;

				peg::on_first_child<peg::variable_name>(variable,
					[&variableName](const peg::ast_node& name)
					{
						// Skip the $ prefix
						variableName = name.content().c_str() + 1;
					});

				auto itrVar = _params->variables.find(variableName);
				response::Value valueVar;

				if (itrVar != _params->variables.get<const response::MapType&>().cend())
				{
					valueVar = response::Value(itrVar->second);
				}
				else
				{
					peg::on_first_child<peg::default_value>(variable,
						[this, &valueVar](const peg::ast_node& defaultValue)
						{
							ValueVisitor visitor(_params->variables);

							visitor.visit(*defaultValue.children.front());
							valueVar = visitor.getValue();
						});
				}

				operationVariables.emplace_back(std::move(variableName), std::move(valueVar));
			});

		_params->variables = std::move(operationVariables);

		// Keep the params alive until the deferred lambda has executed
		auto params = std::move(_params);

		_result = std::async(std::launch::deferred,
			[params](std::future<response::Value> data)
			{
				response::Value document(response::Type::Map);

				document.emplace_back("data", data.get());

				return document;
			}, itr->second->resolve(params->state, *operationDefinition.children.back(), params->fragments, params->variables));
	}
	catch (const schema_exception& ex)
	{
		std::promise<response::Value> promise;
		response::Value document(response::Type::Map);

		document.emplace_back("data", response::Value());
		document.emplace_back("errors", response::Value(ex.getErrors()));
		promise.set_value(std::move(document));

		_result = promise.get_future();
	}
}

SubscriptionData::SubscriptionData(std::shared_ptr<OperationData>&& data, std::unordered_set<SubscriptionName>&& fieldNames,
	std::unique_ptr<peg::ast<std::string>>&& query, std::string&& operationName, SubscriptionCallback&& callback,
	const peg::ast_node& selection)
	: data(std::move(data))
	, fieldNames(std::move(fieldNames))
	, query(std::move(query))
	, operationName(std::move(operationName))
	, callback(std::move(callback))
	, selection(selection)
{
}

// SubscriptionDefinitionVisitor visits the AST collects the fields referenced in the subscription at the point
// where we create a subscription.
class SubscriptionDefinitionVisitor
{
public:
	SubscriptionDefinitionVisitor(SubscriptionParams&& params, SubscriptionCallback&& callback, FragmentMap&& fragments);

	const peg::ast_node& getRoot() const;
	std::shared_ptr<SubscriptionData> getRegistration();

	void visit(const peg::ast_node& operationDefinition);

private:
	SubscriptionParams _params;
	SubscriptionCallback _callback;
	FragmentMap _fragments;
	std::shared_ptr<SubscriptionData> _result;
};

SubscriptionDefinitionVisitor::SubscriptionDefinitionVisitor(SubscriptionParams&& params, SubscriptionCallback&& callback, FragmentMap&& fragments)
	: _params(std::move(params))
	, _callback(std::move(callback))
	, _fragments(std::move(fragments))
{
}

const peg::ast_node& SubscriptionDefinitionVisitor::getRoot() const
{
	return *_params.query->root;
}

std::shared_ptr<SubscriptionData> SubscriptionDefinitionVisitor::getRegistration()
{
	if (!_result)
	{
		std::ostringstream error;

		error << "Missing operation";

		if (!_params.operationName.empty())
		{
			error << " name: " << _params.operationName;
		}

		throw schema_exception({ error.str() });
	}

	auto result = std::move(_result);

	_result.reset();

	return result;
}

void SubscriptionDefinitionVisitor::visit(const peg::ast_node& operationDefinition)
{
	std::string operation;

	peg::on_first_child<peg::operation_type>(operationDefinition,
		[&operation](const peg::ast_node& child)
		{
			operation = child.content();
		});

	if (operation != "subscription")
	{
		// Skip operations other than subscription.
		return;
	}

	auto position = operationDefinition.begin();
	std::string name;

	peg::on_first_child<peg::operation_name>(operationDefinition,
		[&name](const peg::ast_node& child)
		{
			name = child.content();
		});

	if (!_params.operationName.empty()
		&& name != _params.operationName)
	{
		// Skip the subscriptions that don't match the name
		return;
	}

	if (_result)
	{
		std::ostringstream error;

		if (_params.operationName.empty())
		{
			error << "No operationName specified with extra subscription";
		}
		else
		{
			error << "Duplicate subscription";
		}

		if (!name.empty())
		{
			error << " name: " << name;
		}

		error << " line: " << position.line
			<< " column: " << position.byte_in_line;

		throw schema_exception({ error.str() });
	}

	const auto& selection = *operationDefinition.children.back();
	std::unordered_set<SubscriptionName> fieldNames;

	peg::for_each_child<peg::field>(selection,
		[this, &fieldNames](const peg::ast_node& field)
		{
			peg::on_first_child<peg::field_name>(field,
				[&fieldNames](const peg::ast_node& child)
				{
					fieldNames.insert(child.content());
				});
		});

	_result = std::make_shared<SubscriptionData>(
		std::make_shared<OperationData>(
			std::move(_params.state),
			std::move(_params.variables),
			std::move(_fragments)),
		std::move(fieldNames),
		std::move(_params.query),
		std::move(_params.operationName),
		std::move(_callback),
		selection);
}

Request::Request(TypeMap&& operationTypes)
	: _operations(std::move(operationTypes))
{
}

std::future<response::Value> Request::resolve(const std::shared_ptr<RequestState>& state, const peg::ast_node& root, const std::string& operationName, response::Value&& variables) const
{
	FragmentDefinitionVisitor fragmentVisitor;

	peg::for_each_child<peg::fragment_definition>(root,
		[&fragmentVisitor](const peg::ast_node& child)
	{
		fragmentVisitor.visit(child);
	});

	auto fragments = fragmentVisitor.getFragments();
	OperationDefinitionVisitor operationVisitor(state, _operations, operationName, std::move(variables), std::move(fragments));

	peg::for_each_child<peg::operation_definition>(root,
		[&operationVisitor](const peg::ast_node& child)
	{
		operationVisitor.visit(child);
	});

	return operationVisitor.getValue();
}

SubscriptionKey Request::subscribe(SubscriptionParams&& params, SubscriptionCallback&& callback)
{
	FragmentDefinitionVisitor fragmentVisitor;

	peg::for_each_child<peg::fragment_definition>(*params.query->root,
		[&fragmentVisitor](const peg::ast_node& child)
		{
			fragmentVisitor.visit(child);
		});

	auto fragments = fragmentVisitor.getFragments();
	SubscriptionDefinitionVisitor subscriptionVisitor(std::move(params), std::move(callback), std::move(fragments));

	peg::for_each_child<peg::operation_definition>(subscriptionVisitor.getRoot(),
		[&subscriptionVisitor](const peg::ast_node& child)
		{
			subscriptionVisitor.visit(child);
		});

	auto registration = subscriptionVisitor.getRegistration();
	auto key = _nextKey++;

	for (const auto& name : registration->fieldNames)
	{
		_listeners[name].insert(key);
	}

	_subscriptions.emplace(key, std::move(registration));

	return key;
}

void Request::unsubscribe(SubscriptionKey key)
{
	auto itrSubscription = _subscriptions.find(key);

	if (itrSubscription == _subscriptions.cend())
	{
		return;
	}

	for (const auto& name : itrSubscription->second->fieldNames)
	{
		auto itrListener = _listeners.find(name);
		
		itrListener->second.erase(key);
		if (itrListener->second.empty())
		{
			_listeners.erase(itrListener);
		}
	}

	_subscriptions.erase(itrSubscription);

	if (_subscriptions.empty())
	{
		_nextKey = 0;
	}
	else
	{
		_nextKey = _subscriptions.crbegin()->first + 1;
	}
}

void Request::deliver(const SubscriptionName& name, const std::shared_ptr<Object>& subscriptionObject) const
{
	const auto& optionalOrDefaultSubscription = subscriptionObject
		? subscriptionObject
		: _operations.find("subscription")->second;

	auto itrListeners = _listeners.find(name);

	if (itrListeners == _listeners.cend())
	{
		return;
	}

	for (const auto& key : itrListeners->second)
	{
		auto itrSubscription = _subscriptions.find(key);
		auto registration = itrSubscription->second;
		std::future<response::Value> result;

		try
		{
			result = std::async(std::launch::deferred,
				[registration](std::future<response::Value> data)
				{
					response::Value document(response::Type::Map);

					document.emplace_back("data", data.get());

					return document;
				}, optionalOrDefaultSubscription->resolve(registration->data->state, registration->selection, registration->data->fragments, registration->data->variables));
		}
		catch (const schema_exception& ex)
		{
			std::promise<response::Value> promise;
			response::Value document(response::Type::Map);

			document.emplace_back("data", response::Value());
			document.emplace_back("errors", response::Value(ex.getErrors()));

			result = promise.get_future();
		}

		registration->callback(std::move(result));
	}
}

} /* namespace service */
} /* namespace graphql */
} /* namespace facebook */
