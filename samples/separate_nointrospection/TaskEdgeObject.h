// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef TASKEDGEOBJECT_H
#define TASKEDGEOBJECT_H

#include "TodaySchema.h"

namespace graphql::today::object {

namespace TaskEdgeStubs {

template <class TImpl>
concept HasNode = requires (TImpl impl, service::FieldParams params) 
{
	{ impl.getNode(std::move(params)) } -> std::convertible_to<service::FieldResult<std::shared_ptr<Task>>>;
};

template <class TImpl>
concept HasCursor = requires (TImpl impl, service::FieldParams params) 
{
	{ impl.getCursor(std::move(params)) } -> std::convertible_to<service::FieldResult<response::Value>>;
};

} // namespace TaskEdgeStubs

class TaskEdge
	: public service::Object
{
private:
	service::AwaitableResolver resolveNode(service::ResolverParams&& params);
	service::AwaitableResolver resolveCursor(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::shared_ptr<Task>> getNode(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<response::Value> getCursor(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::shared_ptr<Task>> getNode(service::FieldParams&& params) const final
		{
			if constexpr (TaskEdgeStubs::HasNode<T>)
			{
				return _pimpl->getNode(std::move(params));
			}
			else
			{
				throw std::runtime_error(R"ex(TaskEdge::getNode is not implemented)ex");
			}
		}

		service::FieldResult<response::Value> getCursor(service::FieldParams&& params) const final
		{
			if constexpr (TaskEdgeStubs::HasCursor<T>)
			{
				return _pimpl->getCursor(std::move(params));
			}
			else
			{
				throw std::runtime_error(R"ex(TaskEdge::getCursor is not implemented)ex");
			}
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	TaskEdge(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	TaskEdge(std::shared_ptr<T> pimpl)
		: TaskEdge { std::unique_ptr<Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
	}
};

} // namespace graphql::today::object

#endif // TASKEDGEOBJECT_H
