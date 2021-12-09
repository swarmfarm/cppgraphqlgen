// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "PageInfoObject.h"

#include "graphqlservice/introspection/Introspection.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std::literals;

namespace graphql::today {
namespace object {

PageInfo::PageInfo(std::unique_ptr<Concept>&& pimpl) noexcept
	: service::Object{ getTypeNames(), getResolvers() }
	, _pimpl { std::move(pimpl) }
{
}

service::TypeNames PageInfo::getTypeNames() const noexcept
{
	return {
		R"gql(PageInfo)gql"sv
	};
}

service::ResolverMap PageInfo::getResolvers() const noexcept
{
	return {
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ R"gql(hasNextPage)gql"sv, [this](service::ResolverParams&& params) { return resolveHasNextPage(std::move(params)); } },
		{ R"gql(hasPreviousPage)gql"sv, [this](service::ResolverParams&& params) { return resolveHasPreviousPage(std::move(params)); } }
	};
}

void PageInfo::beginSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->beginSelectionSet(params);
}

void PageInfo::endSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->endSelectionSet(params);
}

service::AwaitableResolver PageInfo::resolveHasNextPage(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getHasNextPage(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<bool>::convert(std::move(result), std::move(params));
}

service::AwaitableResolver PageInfo::resolveHasPreviousPage(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getHasPreviousPage(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<bool>::convert(std::move(result), std::move(params));
}

service::AwaitableResolver PageInfo::resolve_typename(service::ResolverParams&& params) const
{
	return service::ModifiedResult<std::string>::convert(std::string{ R"gql(PageInfo)gql" }, std::move(params));
}

} // namespace object

void AddPageInfoDetails(const std::shared_ptr<schema::ObjectType>& typePageInfo, const std::shared_ptr<schema::Schema>& schema)
{
	typePageInfo->AddFields({
		schema::Field::Make(R"gql(hasNextPage)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv))),
		schema::Field::Make(R"gql(hasPreviousPage)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Boolean)gql"sv)))
	});
}

} // namespace graphql::today
