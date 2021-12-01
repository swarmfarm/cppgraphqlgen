// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef HUMANOBJECT_H
#define HUMANOBJECT_H

#include "StarWarsSchema.h"

namespace graphql::learn::object {
namespace methods::HumanMethod {

template <class TImpl>
concept WithParamsId = requires (TImpl impl, service::FieldParams params) 
{
	{ service::FieldResult<response::StringType> { impl.getId(std::move(params)) } };
};

template <class TImpl>
concept NoParamsId = requires (TImpl impl) 
{
	{ service::FieldResult<response::StringType> { impl.getId() } };
};

template <class TImpl>
concept WithParamsName = requires (TImpl impl, service::FieldParams params) 
{
	{ service::FieldResult<std::optional<response::StringType>> { impl.getName(std::move(params)) } };
};

template <class TImpl>
concept NoParamsName = requires (TImpl impl) 
{
	{ service::FieldResult<std::optional<response::StringType>> { impl.getName() } };
};

template <class TImpl>
concept WithParamsFriends = requires (TImpl impl, service::FieldParams params) 
{
	{ service::FieldResult<std::optional<std::vector<std::shared_ptr<service::Object>>>> { impl.getFriends(std::move(params)) } };
};

template <class TImpl>
concept NoParamsFriends = requires (TImpl impl) 
{
	{ service::FieldResult<std::optional<std::vector<std::shared_ptr<service::Object>>>> { impl.getFriends() } };
};

template <class TImpl>
concept WithParamsAppearsIn = requires (TImpl impl, service::FieldParams params) 
{
	{ service::FieldResult<std::optional<std::vector<std::optional<Episode>>>> { impl.getAppearsIn(std::move(params)) } };
};

template <class TImpl>
concept NoParamsAppearsIn = requires (TImpl impl) 
{
	{ service::FieldResult<std::optional<std::vector<std::optional<Episode>>>> { impl.getAppearsIn() } };
};

template <class TImpl>
concept WithParamsHomePlanet = requires (TImpl impl, service::FieldParams params) 
{
	{ service::FieldResult<std::optional<response::StringType>> { impl.getHomePlanet(std::move(params)) } };
};

template <class TImpl>
concept NoParamsHomePlanet = requires (TImpl impl) 
{
	{ service::FieldResult<std::optional<response::StringType>> { impl.getHomePlanet() } };
};

template <class TImpl>
concept HasBeginSelectionSet = requires (TImpl impl, const service::SelectionSetParams params) 
{
	{ impl.beginSelectionSet(params) };
};

template <class TImpl>
concept HasEndSelectionSet = requires (TImpl impl, const service::SelectionSetParams params) 
{
	{ impl.endSelectionSet(params) };
};

} // namespace methods::HumanMethod

class Human
	: public service::Object
{
private:
	service::AwaitableResolver resolveId(service::ResolverParams&& params);
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolveFriends(service::ResolverParams&& params);
	service::AwaitableResolver resolveAppearsIn(service::ResolverParams&& params);
	service::AwaitableResolver resolveHomePlanet(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
		: Character
	{
		virtual ~Concept() = default;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;

		virtual service::FieldResult<std::optional<response::StringType>> getHomePlanet(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<response::StringType> getId(service::FieldParams&& params) const final
		{
			if constexpr (methods::HumanMethod::WithParamsId<T>)
			{
				return { _pimpl->getId(std::move(params)) };
			}
			else
			{
				static_assert(methods::HumanMethod::NoParamsId<T>);
				return { _pimpl->getId() };
			}
		}

		service::FieldResult<std::optional<response::StringType>> getName(service::FieldParams&& params) const final
		{
			if constexpr (methods::HumanMethod::WithParamsName<T>)
			{
				return { _pimpl->getName(std::move(params)) };
			}
			else
			{
				static_assert(methods::HumanMethod::NoParamsName<T>);
				return { _pimpl->getName() };
			}
		}

		service::FieldResult<std::optional<std::vector<std::shared_ptr<service::Object>>>> getFriends(service::FieldParams&& params) const final
		{
			if constexpr (methods::HumanMethod::WithParamsFriends<T>)
			{
				return { _pimpl->getFriends(std::move(params)) };
			}
			else
			{
				static_assert(methods::HumanMethod::NoParamsFriends<T>);
				return { _pimpl->getFriends() };
			}
		}

		service::FieldResult<std::optional<std::vector<std::optional<Episode>>>> getAppearsIn(service::FieldParams&& params) const final
		{
			if constexpr (methods::HumanMethod::WithParamsAppearsIn<T>)
			{
				return { _pimpl->getAppearsIn(std::move(params)) };
			}
			else
			{
				static_assert(methods::HumanMethod::NoParamsAppearsIn<T>);
				return { _pimpl->getAppearsIn() };
			}
		}

		service::FieldResult<std::optional<response::StringType>> getHomePlanet(service::FieldParams&& params) const final
		{
			if constexpr (methods::HumanMethod::WithParamsHomePlanet<T>)
			{
				return { _pimpl->getHomePlanet(std::move(params)) };
			}
			else
			{
				static_assert(methods::HumanMethod::NoParamsHomePlanet<T>);
				return { _pimpl->getHomePlanet() };
			}
		}

		void beginSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::HumanMethod::HasBeginSelectionSet<T>)
			{
				_pimpl->beginSelectionSet(params);
			}
		}

		void endSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::HumanMethod::HasEndSelectionSet<T>)
			{
				_pimpl->endSelectionSet(params);
			}
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Human(std::unique_ptr<Concept>&& pimpl);

	void beginSelectionSet(const service::SelectionSetParams& params) const final;
	void endSelectionSet(const service::SelectionSetParams& params) const final;

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Human(std::shared_ptr<T> pimpl)
		: Human { std::unique_ptr<Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
	}
};

} // namespace graphql::learn::object

#endif // HUMANOBJECT_H
