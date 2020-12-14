// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef QUERYOBJECT_H
#define QUERYOBJECT_H

#include "TodaySchema.h"

namespace graphql::today::object {

class Query
	: public service::Object
{
protected:
	explicit Query();

public:
	virtual service::FieldResult<std::shared_ptr<service::Object>> getNode(service::FieldParams&& params, response::IdType&& idArg) const;
	virtual service::FieldResult<std::shared_ptr<AppointmentConnection>> getAppointments(service::FieldParams&& params, std::optional<response::IntType>&& firstArg, std::optional<response::Value>&& afterArg, std::optional<response::IntType>&& lastArg, std::optional<response::Value>&& beforeArg) const;
	virtual service::FieldResult<std::shared_ptr<TaskConnection>> getTasks(service::FieldParams&& params, std::optional<response::IntType>&& firstArg, std::optional<response::Value>&& afterArg, std::optional<response::IntType>&& lastArg, std::optional<response::Value>&& beforeArg) const;
	virtual service::FieldResult<std::shared_ptr<FolderConnection>> getUnreadCounts(service::FieldParams&& params, std::optional<response::IntType>&& firstArg, std::optional<response::Value>&& afterArg, std::optional<response::IntType>&& lastArg, std::optional<response::Value>&& beforeArg) const;
	virtual service::FieldResult<std::vector<std::shared_ptr<Appointment>>> getAppointmentsById(service::FieldParams&& params, std::vector<response::IdType>&& idsArg) const;
	virtual service::FieldResult<std::vector<std::shared_ptr<Task>>> getTasksById(service::FieldParams&& params, std::vector<response::IdType>&& idsArg) const;
	virtual service::FieldResult<std::vector<std::shared_ptr<Folder>>> getUnreadCountsById(service::FieldParams&& params, std::vector<response::IdType>&& idsArg) const;
	virtual service::FieldResult<std::shared_ptr<NestedType>> getNested(service::FieldParams&& params) const;
	virtual service::FieldResult<response::StringType> getUnimplemented(service::FieldParams&& params) const;
	virtual service::FieldResult<std::vector<std::shared_ptr<Expensive>>> getExpensive(service::FieldParams&& params) const;

private:
	std::future<service::ResolverResult> resolveNode(service::ResolverParams&& params);
	std::future<service::ResolverResult> resolveAppointments(service::ResolverParams&& params);
	std::future<service::ResolverResult> resolveTasks(service::ResolverParams&& params);
	std::future<service::ResolverResult> resolveUnreadCounts(service::ResolverParams&& params);
	std::future<service::ResolverResult> resolveAppointmentsById(service::ResolverParams&& params);
	std::future<service::ResolverResult> resolveTasksById(service::ResolverParams&& params);
	std::future<service::ResolverResult> resolveUnreadCountsById(service::ResolverParams&& params);
	std::future<service::ResolverResult> resolveNested(service::ResolverParams&& params);
	std::future<service::ResolverResult> resolveUnimplemented(service::ResolverParams&& params);
	std::future<service::ResolverResult> resolveExpensive(service::ResolverParams&& params);

	std::future<service::ResolverResult> resolve_typename(service::ResolverParams&& params);
};

} /* namespace graphql::today::object */

#endif // QUERYOBJECT_H