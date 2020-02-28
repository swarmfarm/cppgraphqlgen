// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ValidationSchema.h"

#include <graphqlservice/Introspection.h>

#include <algorithm>
#include <functional>
#include <sstream>
#include <unordered_map>
#include <exception>
#include <array>

namespace graphql {
namespace service {

static const std::array<std::string_view, 3> s_namesDogCommand = {
	"SIT",
	"DOWN",
	"HEEL"
};

template <>
validation::DogCommand ModifiedArgument<validation::DogCommand>::convert(const response::Value& value)
{
	if (!value.maybe_enum())
	{
		throw service::schema_exception { { "not a valid DogCommand value" } };
	}

	auto itr = std::find(s_namesDogCommand.cbegin(), s_namesDogCommand.cend(), value.get<response::StringType>());

	if (itr == s_namesDogCommand.cend())
	{
		throw service::schema_exception { { "not a valid DogCommand value" } };
	}

	return static_cast<validation::DogCommand>(itr - s_namesDogCommand.cbegin());
}

template <>
std::future<response::Value> ModifiedResult<validation::DogCommand>::convert(service::FieldResult<validation::DogCommand>&& result, ResolverParams&& params)
{
	return resolve(std::move(result), std::move(params),
		[](validation::DogCommand&& value, const ResolverParams&)
		{
			response::Value result(response::Type::EnumValue);

			result.set<response::StringType>(std::string(s_namesDogCommand[static_cast<size_t>(value)]));

			return result;
		});
}

static const std::array<std::string_view, 1> s_namesCatCommand = {
	"JUMP"
};

template <>
validation::CatCommand ModifiedArgument<validation::CatCommand>::convert(const response::Value& value)
{
	if (!value.maybe_enum())
	{
		throw service::schema_exception { { "not a valid CatCommand value" } };
	}

	auto itr = std::find(s_namesCatCommand.cbegin(), s_namesCatCommand.cend(), value.get<response::StringType>());

	if (itr == s_namesCatCommand.cend())
	{
		throw service::schema_exception { { "not a valid CatCommand value" } };
	}

	return static_cast<validation::CatCommand>(itr - s_namesCatCommand.cbegin());
}

template <>
std::future<response::Value> ModifiedResult<validation::CatCommand>::convert(service::FieldResult<validation::CatCommand>&& result, ResolverParams&& params)
{
	return resolve(std::move(result), std::move(params),
		[](validation::CatCommand&& value, const ResolverParams&)
		{
			response::Value result(response::Type::EnumValue);

			result.set<response::StringType>(std::string(s_namesCatCommand[static_cast<size_t>(value)]));

			return result;
		});
}

} /* namespace service */

namespace validation {
namespace object {

Query::Query()
	: service::Object({
		"Query"
	}, {
		{ "dog", [this](service::ResolverParams&& params) { return resolveDog(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ "__schema", [this](service::ResolverParams&& params) { return resolve_schema(std::move(params)); } },
		{ "__type", [this](service::ResolverParams&& params) { return resolve_type(std::move(params)); } }
	})
	, _schema(std::make_shared<introspection::Schema>())
{
	introspection::AddTypesToSchema(_schema);
	validation::AddTypesToSchema(_schema);
}

service::FieldResult<std::shared_ptr<Dog>> Query::getDog(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Query::getDog is not implemented)ex");
}

std::future<response::Value> Query::resolveDog(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getDog(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<Dog>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<response::Value> Query::resolve_typename(service::ResolverParams&& params)
{
	return service::ModifiedResult<response::StringType>::convert(response::StringType{ R"gql(Query)gql" }, std::move(params));
}

std::future<response::Value> Query::resolve_schema(service::ResolverParams&& params)
{
	return service::ModifiedResult<service::Object>::convert(std::static_pointer_cast<service::Object>(_schema), std::move(params));
}

std::future<response::Value> Query::resolve_type(service::ResolverParams&& params)
{
	auto argName = service::ModifiedArgument<response::StringType>::require("name", params.arguments);

	return service::ModifiedResult<introspection::object::Type>::convert<service::TypeModifier::Nullable>(_schema->LookupType(argName), std::move(params));
}

Dog::Dog()
	: service::Object({
		"Pet",
		"Dog"
	}, {
		{ "name", [this](service::ResolverParams&& params) { return resolveName(std::move(params)); } },
		{ "nickname", [this](service::ResolverParams&& params) { return resolveNickname(std::move(params)); } },
		{ "barkVolume", [this](service::ResolverParams&& params) { return resolveBarkVolume(std::move(params)); } },
		{ "doesKnowCommand", [this](service::ResolverParams&& params) { return resolveDoesKnowCommand(std::move(params)); } },
		{ "isHousetrained", [this](service::ResolverParams&& params) { return resolveIsHousetrained(std::move(params)); } },
		{ "owner", [this](service::ResolverParams&& params) { return resolveOwner(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } }
	})
{
}

service::FieldResult<response::StringType> Dog::getName(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Dog::getName is not implemented)ex");
}

std::future<response::Value> Dog::resolveName(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getName(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::StringType>::convert(std::move(result), std::move(params));
}

service::FieldResult<std::optional<response::StringType>> Dog::getNickname(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Dog::getNickname is not implemented)ex");
}

std::future<response::Value> Dog::resolveNickname(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getNickname(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::StringType>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::FieldResult<std::optional<response::IntType>> Dog::getBarkVolume(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Dog::getBarkVolume is not implemented)ex");
}

std::future<response::Value> Dog::resolveBarkVolume(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getBarkVolume(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::IntType>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::FieldResult<response::BooleanType> Dog::getDoesKnowCommand(service::FieldParams&&, DogCommand&&) const
{
	throw std::runtime_error(R"ex(Dog::getDoesKnowCommand is not implemented)ex");
}

std::future<response::Value> Dog::resolveDoesKnowCommand(service::ResolverParams&& params)
{
	auto argDogCommand = service::ModifiedArgument<DogCommand>::require("dogCommand", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getDoesKnowCommand(service::FieldParams(params, std::move(params.fieldDirectives)), std::move(argDogCommand));
	resolverLock.unlock();

	return service::ModifiedResult<response::BooleanType>::convert(std::move(result), std::move(params));
}

service::FieldResult<response::BooleanType> Dog::getIsHousetrained(service::FieldParams&&, std::optional<response::BooleanType>&&) const
{
	throw std::runtime_error(R"ex(Dog::getIsHousetrained is not implemented)ex");
}

std::future<response::Value> Dog::resolveIsHousetrained(service::ResolverParams&& params)
{
	auto argAtOtherHomes = service::ModifiedArgument<response::BooleanType>::require<service::TypeModifier::Nullable>("atOtherHomes", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getIsHousetrained(service::FieldParams(params, std::move(params.fieldDirectives)), std::move(argAtOtherHomes));
	resolverLock.unlock();

	return service::ModifiedResult<response::BooleanType>::convert(std::move(result), std::move(params));
}

service::FieldResult<std::shared_ptr<Human>> Dog::getOwner(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Dog::getOwner is not implemented)ex");
}

std::future<response::Value> Dog::resolveOwner(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getOwner(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<Human>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<response::Value> Dog::resolve_typename(service::ResolverParams&& params)
{
	return service::ModifiedResult<response::StringType>::convert(response::StringType{ R"gql(Dog)gql" }, std::move(params));
}

Alien::Alien()
	: service::Object({
		"Sentient",
		"Alien"
	}, {
		{ "name", [this](service::ResolverParams&& params) { return resolveName(std::move(params)); } },
		{ "homePlanet", [this](service::ResolverParams&& params) { return resolveHomePlanet(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } }
	})
{
}

service::FieldResult<response::StringType> Alien::getName(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Alien::getName is not implemented)ex");
}

std::future<response::Value> Alien::resolveName(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getName(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::StringType>::convert(std::move(result), std::move(params));
}

service::FieldResult<std::optional<response::StringType>> Alien::getHomePlanet(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Alien::getHomePlanet is not implemented)ex");
}

std::future<response::Value> Alien::resolveHomePlanet(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getHomePlanet(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::StringType>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<response::Value> Alien::resolve_typename(service::ResolverParams&& params)
{
	return service::ModifiedResult<response::StringType>::convert(response::StringType{ R"gql(Alien)gql" }, std::move(params));
}

Human::Human()
	: service::Object({
		"Sentient",
		"Human"
	}, {
		{ "name", [this](service::ResolverParams&& params) { return resolveName(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } }
	})
{
}

service::FieldResult<response::StringType> Human::getName(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Human::getName is not implemented)ex");
}

std::future<response::Value> Human::resolveName(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getName(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::StringType>::convert(std::move(result), std::move(params));
}

std::future<response::Value> Human::resolve_typename(service::ResolverParams&& params)
{
	return service::ModifiedResult<response::StringType>::convert(response::StringType{ R"gql(Human)gql" }, std::move(params));
}

Cat::Cat()
	: service::Object({
		"Pet",
		"Cat"
	}, {
		{ "name", [this](service::ResolverParams&& params) { return resolveName(std::move(params)); } },
		{ "nickname", [this](service::ResolverParams&& params) { return resolveNickname(std::move(params)); } },
		{ "doesKnowCommand", [this](service::ResolverParams&& params) { return resolveDoesKnowCommand(std::move(params)); } },
		{ "meowVolume", [this](service::ResolverParams&& params) { return resolveMeowVolume(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } }
	})
{
}

service::FieldResult<response::StringType> Cat::getName(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Cat::getName is not implemented)ex");
}

std::future<response::Value> Cat::resolveName(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getName(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::StringType>::convert(std::move(result), std::move(params));
}

service::FieldResult<std::optional<response::StringType>> Cat::getNickname(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Cat::getNickname is not implemented)ex");
}

std::future<response::Value> Cat::resolveNickname(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getNickname(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::StringType>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::FieldResult<response::BooleanType> Cat::getDoesKnowCommand(service::FieldParams&&, CatCommand&&) const
{
	throw std::runtime_error(R"ex(Cat::getDoesKnowCommand is not implemented)ex");
}

std::future<response::Value> Cat::resolveDoesKnowCommand(service::ResolverParams&& params)
{
	auto argCatCommand = service::ModifiedArgument<CatCommand>::require("catCommand", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getDoesKnowCommand(service::FieldParams(params, std::move(params.fieldDirectives)), std::move(argCatCommand));
	resolverLock.unlock();

	return service::ModifiedResult<response::BooleanType>::convert(std::move(result), std::move(params));
}

service::FieldResult<std::optional<response::IntType>> Cat::getMeowVolume(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Cat::getMeowVolume is not implemented)ex");
}

std::future<response::Value> Cat::resolveMeowVolume(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto result = getMeowVolume(service::FieldParams(params, std::move(params.fieldDirectives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::IntType>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<response::Value> Cat::resolve_typename(service::ResolverParams&& params)
{
	return service::ModifiedResult<response::StringType>::convert(response::StringType{ R"gql(Cat)gql" }, std::move(params));
}

} /* namespace object */

Operations::Operations(std::shared_ptr<object::Query> query)
	: service::Request({
		{ "query", query }
	})
	, _query(std::move(query))
{
}

void AddTypesToSchema(const std::shared_ptr<introspection::Schema>& schema)
{
	auto typeDogCommand = std::make_shared<introspection::EnumType>("DogCommand", R"md()md");
	schema->AddType("DogCommand", typeDogCommand);
	auto typeCatCommand = std::make_shared<introspection::EnumType>("CatCommand", R"md()md");
	schema->AddType("CatCommand", typeCatCommand);
	auto typeCatOrDog = std::make_shared<introspection::UnionType>("CatOrDog", R"md()md");
	schema->AddType("CatOrDog", typeCatOrDog);
	auto typeDogOrHuman = std::make_shared<introspection::UnionType>("DogOrHuman", R"md()md");
	schema->AddType("DogOrHuman", typeDogOrHuman);
	auto typeHumanOrAlien = std::make_shared<introspection::UnionType>("HumanOrAlien", R"md()md");
	schema->AddType("HumanOrAlien", typeHumanOrAlien);
	auto typeSentient = std::make_shared<introspection::InterfaceType>("Sentient", R"md()md");
	schema->AddType("Sentient", typeSentient);
	auto typePet = std::make_shared<introspection::InterfaceType>("Pet", R"md()md");
	schema->AddType("Pet", typePet);
	auto typeQuery = std::make_shared<introspection::ObjectType>("Query", R"md(GraphQL validation [sample](http://spec.graphql.org/June2018/#example-26a9d))md");
	schema->AddType("Query", typeQuery);
	auto typeDog = std::make_shared<introspection::ObjectType>("Dog", R"md()md");
	schema->AddType("Dog", typeDog);
	auto typeAlien = std::make_shared<introspection::ObjectType>("Alien", R"md()md");
	schema->AddType("Alien", typeAlien);
	auto typeHuman = std::make_shared<introspection::ObjectType>("Human", R"md()md");
	schema->AddType("Human", typeHuman);
	auto typeCat = std::make_shared<introspection::ObjectType>("Cat", R"md()md");
	schema->AddType("Cat", typeCat);

	typeDogCommand->AddEnumValues({
		{ std::string{ service::s_namesDogCommand[static_cast<size_t>(validation::DogCommand::SIT)] }, R"md()md", std::nullopt },
		{ std::string{ service::s_namesDogCommand[static_cast<size_t>(validation::DogCommand::DOWN)] }, R"md()md", std::nullopt },
		{ std::string{ service::s_namesDogCommand[static_cast<size_t>(validation::DogCommand::HEEL)] }, R"md()md", std::nullopt }
	});
	typeCatCommand->AddEnumValues({
		{ std::string{ service::s_namesCatCommand[static_cast<size_t>(validation::CatCommand::JUMP)] }, R"md()md", std::nullopt }
	});

	typeCatOrDog->AddPossibleTypes({
		schema->LookupType("Cat"),
		schema->LookupType("Dog")
	});
	typeDogOrHuman->AddPossibleTypes({
		schema->LookupType("Dog"),
		schema->LookupType("Human")
	});
	typeHumanOrAlien->AddPossibleTypes({
		schema->LookupType("Human"),
		schema->LookupType("Alien")
	});

	typeSentient->AddFields({
		std::make_shared<introspection::Field>("name", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String")))
	});
	typePet->AddFields({
		std::make_shared<introspection::Field>("name", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String")))
	});

	typeQuery->AddFields({
		std::make_shared<introspection::Field>("dog", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("Dog"))
	});
	typeDog->AddInterfaces({
		typePet
	});
	typeDog->AddFields({
		std::make_shared<introspection::Field>("name", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String"))),
		std::make_shared<introspection::Field>("nickname", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("String")),
		std::make_shared<introspection::Field>("barkVolume", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("Int")),
		std::make_shared<introspection::Field>("doesKnowCommand", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("dogCommand", R"md()md", schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("DogCommand")), R"gql()gql")
		}), schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("Boolean"))),
		std::make_shared<introspection::Field>("isHousetrained", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("atOtherHomes", R"md()md", schema->LookupType("Boolean"), R"gql()gql")
		}), schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("Boolean"))),
		std::make_shared<introspection::Field>("owner", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("Human"))
	});
	typeAlien->AddInterfaces({
		typeSentient
	});
	typeAlien->AddFields({
		std::make_shared<introspection::Field>("name", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String"))),
		std::make_shared<introspection::Field>("homePlanet", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("String"))
	});
	typeHuman->AddInterfaces({
		typeSentient
	});
	typeHuman->AddFields({
		std::make_shared<introspection::Field>("name", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String")))
	});
	typeCat->AddInterfaces({
		typePet
	});
	typeCat->AddFields({
		std::make_shared<introspection::Field>("name", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("String"))),
		std::make_shared<introspection::Field>("nickname", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("String")),
		std::make_shared<introspection::Field>("doesKnowCommand", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("catCommand", R"md()md", schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("CatCommand")), R"gql()gql")
		}), schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("Boolean"))),
		std::make_shared<introspection::Field>("meowVolume", R"md()md", std::nullopt, std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("Int"))
	});

	schema->AddQueryType(typeQuery);
}

} /* namespace validation */
} /* namespace graphql */
