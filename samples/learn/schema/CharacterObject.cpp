// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "CharacterObject.h"

#include "graphqlservice/internal/Schema.h"

#include "graphqlservice/introspection/IntrospectionSchema.h"

using namespace std::literals;

namespace graphql::learn {
namespace object {

Character::Character(std::unique_ptr<const Concept>&& pimpl) noexcept
	: service::Object { pimpl->getTypeNames(), pimpl->getResolvers() }
	, _pimpl { std::move(pimpl) }
{
}

void Character::beginSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->beginSelectionSet(params);
}

void Character::endSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->endSelectionSet(params);
}

} // namespace object

void AddCharacterDetails(const std::shared_ptr<schema::InterfaceType>& typeCharacter, const std::shared_ptr<schema::Schema>& schema)
{
	typeCharacter->AddFields({
		schema::Field::Make(R"gql(id)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(ID)gql"sv))),
		schema::Field::Make(R"gql(name)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(String)gql"sv)),
		schema::Field::Make(R"gql(friends)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::LIST, schema->LookupType(R"gql(Character)gql"sv))),
		schema::Field::Make(R"gql(appearsIn)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::LIST, schema->LookupType(R"gql(Episode)gql"sv)))
	});
}

} // namespace graphql::learn
