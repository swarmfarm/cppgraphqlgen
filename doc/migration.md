# Migration Guide for v4.x

Most of the changes in v4.x affect services generated with `schemagen` more than clients generated with `clientgen`.

## Adopting C++20
  
This version takes advantage of and requires the following C++20 features:

- Coroutines (in either the `std` or `std::experimental` namespace)
- Concepts

There is enough support for these features in the following compiler versions, your mileage may vary with anything older than these:

- Microsoft Windows: Visual Studio 2019
- Linux: Ubuntu 20.04 LTS with gcc 10.3.0
- macOS: 11 (Big Sur) with AppleClang 13.0.0.

## Using Coroutines

The template methods generated by `schemagen` will construct the expected [awaitable](awaitable.md) type using the result of your field getter methods. If you want to implement your field getter as a coroutine, you can explicitly return the expected awaitable type, otherwise return any type that is implicitly convertible to the awaitable-wrapped type.

Take a look at `Query::getNode` in [samples/today/TodayMock.cpp](../samples/today/TodayMock.cpp) for an example. The `auto operator co_await(std::chrono::duration<_Rep, _Period> delay)` operator overload in the same file is also an example of how you can integrate custom awaitables in your field getters.

## Type Erasure Instead of Inheritance

Change your class declarations so that they no longer inherit from the generated `object` namespace classes. If you need the `shared_from_this()` method, you can replace that with `std::enable_shared_from_this<T>`. Remove the `override` or `final` keyword from any virtual field getter method declarations which were inherited from the object type.

In cases where the return type has changed from `std::shared_ptr<service::Object>` to `std::shared_ptr<object::Interface>` or `std::shared_ptr<object::Union>`, wrap the concrete type in `std::make_shared<T>(...)` for the polymorphic type and return that.

## Simplify Field Accessor Signatures

Examine the generated object types and determine what the expected return type is from each field getter method. Replace the `service::FieldResult` wrapped type with the expected return type (possibly including the awaitable wrapper).

Make methods `const` or non-`const` as appropriate. The `const` type erased object has a `const std::shared_ptr<T>` to your type, but the type inside of the `std::shared_ptr` is not `const`, so it can call non-`const` methods on your type. You can get rid of a lot of `mutable` fields or `const_cast` calls by doing this and make your type `const` correct.

Parameters can be passed as a `const&` reference, a `&&` r-value reference, or by value. The generated template methods will forward an r-value reference which will be implicitly converted into any of these types when calling your method.

Remove any unused `service::FieldParams` arguments. If your method does not take that as the first parameter, the generated template method will drop it and pass the rest of the expected arguments to your method.

## Decoupling Implementation Types from the Generated Types

If your implementation is tightly coupled with the object hierarchy from the schema, here's an example of how you might decouple them. Let's assume that you have a schema that looks something like this:
```graphql
interface Node
{
    id: ID!
}

type NodeTypeA implements Node
{
     id: ID!
     # More fields specific to NodeTypeA...
}

type NodeTypeB implements Node
{
     id: ID!
     # More fields specific to NodeTypeB...
}

# ...and so on for NodeTypeC, NodeTypeD, etc.
```
If you want a collection of `Node` interface objects, the C++ implementation using inheritance in prior versions might look something like:
```c++
class NodeTypeA : public object::NodeTypeA
{
    // Implement the field accessors with an exact match for the virtual method signature...
    service::FieldResult<response::IdType> getId(service::FieldParams&&) const override;
};

class NodeTypeB : public object::NodeTypeB
{
    // Implement the field accessors with an exact match for the virtual method signature...
    service::FieldResult<response::IdType> getId(service::FieldParams&&) const override;
};

std::vector<std::shared_ptr<service::Object>> nodes {
    std::make_shared<NodeTypeA>(),
    std::make_shared<NodeTypeB>(),
    // Can insert any sub-class of service::Object...
};
```
It's up to the you to make sure the `nodes` vector in this example only contains objects which actually implement the `Node` interface. If you want to do something more sophisticated like performing a lookup by `id`, you'd either need to request that before inserting an element and up-casting to `std::shared_ptr<service::Object>`, or you'd need to preserve the concrete type of each element, e.g. in a `std::variant` to be able to safely down-cast to the concrete type.

As of 4.x, the implementation might look more like this:
```c++
class NodeTypeImpl
{
public:
    // Need to override this in the sub-classes to construct the correct sub-type wrappers.
    virtual std::shared_ptr<object::Node> make_node() const = 0;

    const response::IdType& getId() const noexcept final;

    // Implement/declare any other accessors you want to use without downcasting...

private:
    const response::IdType _id;
};

class NodeTypeA
    : public NodeTypeImpl
    , public std::enable_shared_from_this<NodeTypeA>
{
public:
    // Convert to a type-erased Node.
    std::shared_ptr<object::Node> make_node() const final
    {
        return std::make_shared<object::Node>(std::make_shared<object::NodeTypeA>(shared_from_this()));
    }

    // Implement NodeTypeA and any NodeTypeImpl override accessors...
};

class NodeTypeB
    : public NodeTypeImpl
    , public std::enable_shared_from_this<NodeTypeB>
{
public:
    // Convert to a type-erased Node.
    std::shared_ptr<object::Node> make_node() const final
    {
        return std::make_shared<object::Node>(std::make_shared<object::NodeTypeB>(shared_from_this()));
    }

    // Implement NodeTypeB and any NodeTypeImpl override accessors...
};

std::vector<std::shared_ptr<NodeTypeImpl>> nodes {
    std::make_shared<NodeTypeA>(),
    std::make_shared<NodeTypeB>(),
    // Can only insert sub-classes of NodeTypeImpl...
};

std::vector<std::shared_ptr<object::Node>> wrap_nodes()
{
    std::vector<std::shared_ptr<object::Node>> result(nodes.size());

    std::transform(nodes.cbegin(), nodes.cend(), result.begin(), [](const auto& node) {
        return node
            ? node->make_node()
            : std::shared_ptr<object::Node> {};
    });

    return result;
}
```
This has several advantages over the previous version.

- You can declare your own inheritance heirarchy without any constraints inherited from `service::Object`, such as already inheriting from `std::enable_shared_from_this<service::Object>` and defininig `shared_from_this()` for that type.
- You can add your own common implementation for the interface methods you want, e.g. `NodeTypeImpl::getId`.
- Best of all, you no longer need to match an exact method signature to override the `object::NodeType*` accessors. For example, `NodeTypeImpl::getId` uses a more efficient return type, does not require a `service::FieldParams` argument (which is likely ignored anyway), and it can be `const` and `noexcept`. All of that together means you can use it as an internal accessor from any of these types as well as the field getter implementation.

The type erased implementation gives you a lot more control over your class hierarchy and makes it easier to use outside of the GraphQL service.

## CMake Changes

By default, earlier versions of `schemagen` would generate a single header and a single source file for the entire schema, including the declaration and definition of all of the object types. For any significantly complex schema, this source file could get very big. Even the `Today` sample schema was large enough to require a special `/bigobj` flag when compiling with `MSVC`. It also made incremental builds take much longer if you only added/removed/modified a few types, because the entire schema needed to be recompiled.

For a long time, `schemagen` also supported a `--separate-files` flag which would output a separate header and source file for each object type in the schema. This requires more complicated build logic since the set of files that need to be built can change based on the schema, but the end result is much easier to read and incremental builds are faster.

In v4.x, the separate files option is not only the default, it's the only option. Supporting both modes of code generation would have added too much complexity and too many tradeoffs for the simplified build logic. Instead, v4.x adds several CMake helper functions in [cmake/cppgraphqlgen-functions.cmake](../cmake/cppgraphqlgen-functions.cmake) which encapsulate the best practices for regenerating and building the schema targets dynamically when the schema file changes. These functions are automatically included by `find_package(cppgraphqlgen)`.

Replace custom CMake logic to invoke `schemagen` and `clientgen` with these helper functions:
- `update_graphql_schema_files`: Runs `schemagen` with required parameters and additional optional parameters.
- `update_graphql_client_files`: Runs `clientgen` with required parameters and additional optional parameters.

The output is generated in the CMake build directory. The files are compared against the contents of the source directory, and any changed/added files will be copied over to the sources directory. Files which were not regenerated will be deleted from the source directory.

_IMPORTANT_: The `update_graphql_schema_files` and `update_graphql_client_files` functions expect to generate sources in a separate sub-directory from any other source code. They will check for any source files that don't match the naming patterns of the code generators and fail the build rather than deleting them. Just in case, it's a good idea to make sure you have your source code backed up or under source control (e.g. committed in a git repository) before invoking these CMake functions.

Declare library targets which automatically include all of the generated files with these helper functions:
- `add_graphql_schema_target`: Declares a library target for the specified schema which depends on the output of `update_graphql_schema_files` and automatically links all of the shared library dependencies needed for a service.
- `add_graphql_client_target`: Declares a library target for the specified client which depends on the output of `update_graphql_client_files` and automatically links all of the shared library dependencies needed for a client.

With all of the refactoring in v4.x, there ceased to be any separation between the `graphqlintrospection` and `graphqlservice` libraries. Even if you use the `--no-introspection` flag with `schemagen`, the generated code still depends on the general schema types which remained in `graphqlintrospection` to perform query validation. As part of the v4.x release, the 2 libraries were combined back into a single `graphqlservice` target. If you use `add_graphql_schema_target` you do not need to worry about this, otherwise you should replace any references to just `graphqlintrospection` with `graphqlservice`.