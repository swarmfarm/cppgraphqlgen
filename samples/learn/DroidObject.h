// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef DROIDOBJECT_H
#define DROIDOBJECT_H

#include "StarWarsSchema.h"

namespace graphql::learn::object {
namespace methods::DroidMethod {

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
concept WithParamsPrimaryFunction = requires (TImpl impl, service::FieldParams params) 
{
	{ service::FieldResult<std::optional<response::StringType>> { impl.getPrimaryFunction(std::move(params)) } };
};

template <class TImpl>
concept NoParamsPrimaryFunction = requires (TImpl impl) 
{
	{ service::FieldResult<std::optional<response::StringType>> { impl.getPrimaryFunction() } };
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

} // namespace methods::DroidMethod

class Droid
	: public service::Object
{
private:
	service::AwaitableResolver resolveId(service::ResolverParams&& params);
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolveFriends(service::ResolverParams&& params);
	service::AwaitableResolver resolveAppearsIn(service::ResolverParams&& params);
	service::AwaitableResolver resolvePrimaryFunction(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
		: Character
	{
		virtual ~Concept() = default;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;

		virtual service::FieldResult<std::optional<response::StringType>> getPrimaryFunction(service::FieldParams&& params) const = 0;
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
			if constexpr (methods::DroidMethod::WithParamsId<T>)
			{
				return { _pimpl->getId(std::move(params)) };
			}
			else
			{
				static_assert(methods::DroidMethod::NoParamsId<T>);
				return { _pimpl->getId() };
			}
		}

		service::FieldResult<std::optional<response::StringType>> getName(service::FieldParams&& params) const final
		{
			if constexpr (methods::DroidMethod::WithParamsName<T>)
			{
				return { _pimpl->getName(std::move(params)) };
			}
			else
			{
				static_assert(methods::DroidMethod::NoParamsName<T>);
				return { _pimpl->getName() };
			}
		}

		service::FieldResult<std::optional<std::vector<std::shared_ptr<service::Object>>>> getFriends(service::FieldParams&& params) const final
		{
			if constexpr (methods::DroidMethod::WithParamsFriends<T>)
			{
				return { _pimpl->getFriends(std::move(params)) };
			}
			else
			{
				static_assert(methods::DroidMethod::NoParamsFriends<T>);
				return { _pimpl->getFriends() };
			}
		}

		service::FieldResult<std::optional<std::vector<std::optional<Episode>>>> getAppearsIn(service::FieldParams&& params) const final
		{
			if constexpr (methods::DroidMethod::WithParamsAppearsIn<T>)
			{
				return { _pimpl->getAppearsIn(std::move(params)) };
			}
			else
			{
				static_assert(methods::DroidMethod::NoParamsAppearsIn<T>);
				return { _pimpl->getAppearsIn() };
			}
		}

		service::FieldResult<std::optional<response::StringType>> getPrimaryFunction(service::FieldParams&& params) const final
		{
			if constexpr (methods::DroidMethod::WithParamsPrimaryFunction<T>)
			{
				return { _pimpl->getPrimaryFunction(std::move(params)) };
			}
			else
			{
				static_assert(methods::DroidMethod::NoParamsPrimaryFunction<T>);
				return { _pimpl->getPrimaryFunction() };
			}
		}

		void beginSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::DroidMethod::HasBeginSelectionSet<T>)
			{
				_pimpl->beginSelectionSet(params);
			}
		}

		void endSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::DroidMethod::HasEndSelectionSet<T>)
			{
				_pimpl->endSelectionSet(params);
			}
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Droid(std::unique_ptr<Concept>&& pimpl);

	void beginSelectionSet(const service::SelectionSetParams& params) const final;
	void endSelectionSet(const service::SelectionSetParams& params) const final;

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Droid(std::shared_ptr<T> pimpl)
		: Droid { std::unique_ptr<Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
	}
};

} // namespace graphql::learn::object

#endif // DROIDOBJECT_H
