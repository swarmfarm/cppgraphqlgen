// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef INPUTVALUEOBJECT_H
#define INPUTVALUEOBJECT_H

#include "IntrospectionSchema.h"

namespace graphql::introspection::object {

class [[nodiscard]] InputValue final
	: public service::Object
{
private:
	[[nodiscard]] service::AwaitableResolver resolveName(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveDescription(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveType(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveDefaultValue(service::ResolverParams&& params) const;

	[[nodiscard]] service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct [[nodiscard]] Concept
	{
		virtual ~Concept() = default;

		[[nodiscard]] virtual service::AwaitableScalar<std::string> getName() const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<std::optional<std::string>> getDescription() const = 0;
		[[nodiscard]] virtual service::AwaitableObject<std::shared_ptr<Type>> getType() const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<std::optional<std::string>> getDefaultValue() const = 0;
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

		[[nodiscard]] service::AwaitableObject<std::shared_ptr<Type>> getType() const final
		{
			return { _pimpl->getType() };
		}

		[[nodiscard]] service::AwaitableScalar<std::optional<std::string>> getDefaultValue() const final
		{
			return { _pimpl->getDefaultValue() };
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	const std::unique_ptr<const Concept> _pimpl;

	[[nodiscard]] service::TypeNames getTypeNames() const noexcept;
	[[nodiscard]] service::ResolverMap getResolvers() const noexcept;

public:
	GRAPHQLSERVICE_EXPORT InputValue(std::shared_ptr<introspection::InputValue> pimpl) noexcept;
	GRAPHQLSERVICE_EXPORT ~InputValue();
};

} // namespace graphql::introspection::object

#endif // INPUTVALUEOBJECT_H