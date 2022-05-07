// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "MutationObject.h"
#include "MutateDogResultObject.h"

#include "graphqlservice/internal/Schema.h"

#include "graphqlservice/introspection/IntrospectionSchema.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std::literals;

namespace graphql::validation {
namespace object {

Mutation::Mutation(std::unique_ptr<const Concept>&& pimpl) noexcept
	: service::Object{ getTypeNames(), getResolvers() }
	, _pimpl { std::move(pimpl) }
{
}

service::TypeNames Mutation::getTypeNames() const noexcept
{
	return {
		R"gql(Mutation)gql"sv
	};
}

service::ResolverMap Mutation::getResolvers() const noexcept
{
	return {
		{ R"gql(mutateDog)gql"sv, [this](service::ResolverParams&& params) { return resolveMutateDog(std::move(params)); } },
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } }
	};
}

void Mutation::beginSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->beginSelectionSet(params);
}

void Mutation::endSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->endSelectionSet(params);
}

service::AwaitableResolver Mutation::resolveMutateDog(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->applyMutateDog(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<MutateDogResult>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Mutation::resolve_typename(service::ResolverParams&& params) const
{
	return service::ModifiedResult<std::string>::convert(std::string{ R"gql(Mutation)gql" }, std::move(params));
}

} // namespace object

void AddMutationDetails(const std::shared_ptr<schema::ObjectType>& typeMutation, const std::shared_ptr<schema::Schema>& schema)
{
	typeMutation->AddFields({
		schema::Field::Make(R"gql(mutateDog)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType(R"gql(MutateDogResult)gql"sv))
	});
}

} // namespace graphql::validation