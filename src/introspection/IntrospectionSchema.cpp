// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "graphqlservice/internal/Introspection.h"

#include <algorithm>
#include <array>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

using namespace std::literals;

namespace graphql {
namespace service {

static const auto s_namesTypeKind = introspection::getTypeKindNames();
static const auto s_valuesTypeKind = introspection::getTypeKindValues();

template <>
introspection::TypeKind ModifiedArgument<introspection::TypeKind>::convert(const response::Value& value)
{
	if (!value.maybe_enum())
	{
		throw service::schema_exception { { R"ex(not a valid __TypeKind value)ex" } };
	}

	const auto result = internal::sorted_map_lookup<internal::shorter_or_less>(
		s_valuesTypeKind,
		std::string_view { value.get<std::string>() });

	if (!result)
	{
		throw service::schema_exception { { R"ex(not a valid __TypeKind value)ex" } };
	}

	return *result;
}

template <>
service::AwaitableResolver ModifiedResult<introspection::TypeKind>::convert(service::AwaitableScalar<introspection::TypeKind> result, ResolverParams params)
{
	return resolve(std::move(result), std::move(params),
		[](introspection::TypeKind value, const ResolverParams&)
		{
			response::Value result(response::Type::EnumValue);

			result.set<std::string>(std::string { s_namesTypeKind[static_cast<size_t>(value)] });

			return result;
		});
}

template <>
void ModifiedResult<introspection::TypeKind>::validateScalar(const response::Value& value)
{
	if (!value.maybe_enum())
	{
		throw service::schema_exception { { R"ex(not a valid __TypeKind value)ex" } };
	}

	const auto [itr, itrEnd] = internal::sorted_map_equal_range<internal::shorter_or_less>(
		s_valuesTypeKind.begin(),
		s_valuesTypeKind.end(),
		std::string_view { value.get<std::string>() });

	if (itr == itrEnd)
	{
		throw service::schema_exception { { R"ex(not a valid __TypeKind value)ex" } };
	}
}

static const auto s_namesDirectiveLocation = introspection::getDirectiveLocationNames();
static const auto s_valuesDirectiveLocation = introspection::getDirectiveLocationValues();

template <>
introspection::DirectiveLocation ModifiedArgument<introspection::DirectiveLocation>::convert(const response::Value& value)
{
	if (!value.maybe_enum())
	{
		throw service::schema_exception { { R"ex(not a valid __DirectiveLocation value)ex" } };
	}

	const auto result = internal::sorted_map_lookup<internal::shorter_or_less>(
		s_valuesDirectiveLocation,
		std::string_view { value.get<std::string>() });

	if (!result)
	{
		throw service::schema_exception { { R"ex(not a valid __DirectiveLocation value)ex" } };
	}

	return *result;
}

template <>
service::AwaitableResolver ModifiedResult<introspection::DirectiveLocation>::convert(service::AwaitableScalar<introspection::DirectiveLocation> result, ResolverParams params)
{
	return resolve(std::move(result), std::move(params),
		[](introspection::DirectiveLocation value, const ResolverParams&)
		{
			response::Value result(response::Type::EnumValue);

			result.set<std::string>(std::string { s_namesDirectiveLocation[static_cast<size_t>(value)] });

			return result;
		});
}

template <>
void ModifiedResult<introspection::DirectiveLocation>::validateScalar(const response::Value& value)
{
	if (!value.maybe_enum())
	{
		throw service::schema_exception { { R"ex(not a valid __DirectiveLocation value)ex" } };
	}

	const auto [itr, itrEnd] = internal::sorted_map_equal_range<internal::shorter_or_less>(
		s_valuesDirectiveLocation.begin(),
		s_valuesDirectiveLocation.end(),
		std::string_view { value.get<std::string>() });

	if (itr == itrEnd)
	{
		throw service::schema_exception { { R"ex(not a valid __DirectiveLocation value)ex" } };
	}
}

} // namespace service

namespace introspection {

void AddTypesToSchema(const std::shared_ptr<schema::Schema>& schema)
{
	schema->AddType(R"gql(Boolean)gql"sv, schema::ScalarType::Make(R"gql(Boolean)gql"sv, R"md(Built-in type)md"sv, R"url(https://spec.graphql.org/October2021/#sec-Boolean)url"sv));
	schema->AddType(R"gql(Float)gql"sv, schema::ScalarType::Make(R"gql(Float)gql"sv, R"md(Built-in type)md"sv, R"url(https://spec.graphql.org/October2021/#sec-Float)url"sv));
	schema->AddType(R"gql(ID)gql"sv, schema::ScalarType::Make(R"gql(ID)gql"sv, R"md(Built-in type)md"sv, R"url(https://spec.graphql.org/October2021/#sec-ID)url"sv));
	schema->AddType(R"gql(Int)gql"sv, schema::ScalarType::Make(R"gql(Int)gql"sv, R"md(Built-in type)md"sv, R"url(https://spec.graphql.org/October2021/#sec-Int)url"sv));
	schema->AddType(R"gql(String)gql"sv, schema::ScalarType::Make(R"gql(String)gql"sv, R"md(Built-in type)md"sv, R"url(https://spec.graphql.org/October2021/#sec-String)url"sv));
	auto typeTypeKind = schema::EnumType::Make(R"gql(__TypeKind)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(__TypeKind)gql"sv, typeTypeKind);
	auto typeDirectiveLocation = schema::EnumType::Make(R"gql(__DirectiveLocation)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(__DirectiveLocation)gql"sv, typeDirectiveLocation);
	auto typeSchema = schema::ObjectType::Make(R"gql(__Schema)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(__Schema)gql"sv, typeSchema);
	auto typeType = schema::ObjectType::Make(R"gql(__Type)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(__Type)gql"sv, typeType);
	auto typeField = schema::ObjectType::Make(R"gql(__Field)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(__Field)gql"sv, typeField);
	auto typeInputValue = schema::ObjectType::Make(R"gql(__InputValue)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(__InputValue)gql"sv, typeInputValue);
	auto typeEnumValue = schema::ObjectType::Make(R"gql(__EnumValue)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(__EnumValue)gql"sv, typeEnumValue);
	auto typeDirective = schema::ObjectType::Make(R"gql(__Directive)gql"sv, R"md()md"sv);
	schema->AddType(R"gql(__Directive)gql"sv, typeDirective);

	typeTypeKind->AddEnumValues({
		{ service::s_namesTypeKind[static_cast<size_t>(introspection::TypeKind::SCALAR)], R"md()md"sv, std::nullopt },
		{ service::s_namesTypeKind[static_cast<size_t>(introspection::TypeKind::OBJECT)], R"md()md"sv, std::nullopt },
		{ service::s_namesTypeKind[static_cast<size_t>(introspection::TypeKind::INTERFACE)], R"md()md"sv, std::nullopt },
		{ service::s_namesTypeKind[static_cast<size_t>(introspection::TypeKind::UNION)], R"md()md"sv, std::nullopt },
		{ service::s_namesTypeKind[static_cast<size_t>(introspection::TypeKind::ENUM)], R"md()md"sv, std::nullopt },
		{ service::s_namesTypeKind[static_cast<size_t>(introspection::TypeKind::INPUT_OBJECT)], R"md()md"sv, std::nullopt },
		{ service::s_namesTypeKind[static_cast<size_t>(introspection::TypeKind::LIST)], R"md()md"sv, std::nullopt },
		{ service::s_namesTypeKind[static_cast<size_t>(introspection::TypeKind::NON_NULL)], R"md()md"sv, std::nullopt }
	});
	typeDirectiveLocation->AddEnumValues({
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::QUERY)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::MUTATION)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::SUBSCRIPTION)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::FIELD)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::FRAGMENT_DEFINITION)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::FRAGMENT_SPREAD)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::INLINE_FRAGMENT)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::VARIABLE_DEFINITION)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::SCHEMA)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::SCALAR)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::OBJECT)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::FIELD_DEFINITION)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::ARGUMENT_DEFINITION)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::INTERFACE)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::UNION)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::ENUM)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::ENUM_VALUE)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::INPUT_OBJECT)], R"md()md"sv, std::nullopt },
		{ service::s_namesDirectiveLocation[static_cast<size_t>(introspection::DirectiveLocation::INPUT_FIELD_DEFINITION)], R"md()md"sv, std::nullopt }
	});

	AddSchemaDetails(typeSchema, schema);
	AddTypeDetails(typeType, schema);
	AddFieldDetails(typeField, schema);
	AddInputValueDetails(typeInputValue, schema);
	AddEnumValueDetails(typeEnumValue, schema);
	AddDirectiveDetails(typeDirective, schema);

	schema->AddDirective(schema::Directive::Make(R"gql(skip)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::FIELD,
		introspection::DirectiveLocation::FRAGMENT_SPREAD,
		introspection::DirectiveLocation::INLINE_FRAGMENT
	}, {
		schema::InputValue::Make(R"gql(if)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv)), R"gql()gql"sv)
	}, false));
	schema->AddDirective(schema::Directive::Make(R"gql(include)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::FIELD,
		introspection::DirectiveLocation::FRAGMENT_SPREAD,
		introspection::DirectiveLocation::INLINE_FRAGMENT
	}, {
		schema::InputValue::Make(R"gql(if)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv)), R"gql()gql"sv)
	}, false));
	schema->AddDirective(schema::Directive::Make(R"gql(deprecated)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::FIELD_DEFINITION,
		introspection::DirectiveLocation::ENUM_VALUE
	}, {
		schema::InputValue::Make(R"gql(reason)gql"sv, R"md()md"sv, schema->LookupType(R"gql(String)gql"sv), R"gql("No longer supported")gql"sv)
	}, false));
	schema->AddDirective(schema::Directive::Make(R"gql(specifiedBy)gql"sv, R"md()md"sv, {
		introspection::DirectiveLocation::SCALAR
	}, {
		schema::InputValue::Make(R"gql(url)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(String)gql"sv)), R"gql()gql"sv)
	}, false));
}

} // namespace introspection
} // namespace graphql