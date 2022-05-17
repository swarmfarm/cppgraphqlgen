// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef DIRECTIVEOBJECT_H
#define DIRECTIVEOBJECT_H

#include "IntrospectionSchema.h"

namespace graphql::introspection::object {

class [[nodiscard]] Directive final
	: public service::Object
{
private:
	[[nodiscard]] service::AwaitableResolver resolveName(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveDescription(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveLocations(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveArgs(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveIsRepeatable(service::ResolverParams&& params) const;

	[[nodiscard]] service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct [[nodiscard]] Concept
	{
		virtual ~Concept() = default;

		[[nodiscard]] virtual service::AwaitableScalar<std::string> getName() const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<std::optional<std::string>> getDescription() const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<std::vector<DirectiveLocation>> getLocations() const = 0;
		[[nodiscard]] virtual service::AwaitableObject<std::vector<std::shared_ptr<InputValue>>> getArgs() const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<bool> getIsRepeatable() const = 0;
	};

	template <class T>
	struct [[nodiscard]] Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		[[nodiscard]] service::AwaitableScalar<std::string> getName() const final
		{
			return { _pimpl->getName() };
		}

		[[nodiscard]] service::AwaitableScalar<std::optional<std::string>> getDescription() const final
		{
			return { _pimpl->getDescription() };
		}

		[[nodiscard]] service::AwaitableScalar<std::vector<DirectiveLocation>> getLocations() const final
		{
			return { _pimpl->getLocations() };
		}

		[[nodiscard]] service::AwaitableObject<std::vector<std::shared_ptr<InputValue>>> getArgs() const final
		{
			return { _pimpl->getArgs() };
		}

		[[nodiscard]] service::AwaitableScalar<bool> getIsRepeatable() const final
		{
			return { _pimpl->getIsRepeatable() };
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	const std::unique_ptr<const Concept> _pimpl;

	[[nodiscard]] service::TypeNames getTypeNames() const noexcept;
	[[nodiscard]] service::ResolverMap getResolvers() const noexcept;

public:
	GRAPHQLSERVICE_EXPORT Directive(std::shared_ptr<introspection::Directive> pimpl) noexcept;
	GRAPHQLSERVICE_EXPORT ~Directive();
};

} // namespace graphql::introspection::object

#endif // DIRECTIVEOBJECT_H
