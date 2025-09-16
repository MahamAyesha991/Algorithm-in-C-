
## Why is it dangerous to return a reference when you must return an object?
Returning a reference to a local object is risky because the object is destroyed once the function ends. This leaves the reference pointing to invalid memory, which can cause unexpected behavior or crashes.

## What is an implicit interface in the context of compile-time polymorphism?
An implicit interface means that a class doesn not need to formally inherit or implement an interface. Instead, templates check at compile time if required functions exist this is often used in generic programming where the structure just "fits" the needs without explicitly declaring it.

## Why should you prefer range member functions to their single-element counterparts?
Range-based functions operate over entire sections of a container, making them more efficient and readable. They often result in fewer function calls and can help avoid mistakes when dealing with loops or iterators manually.

## Why should you avoid in-place modification of keys in a set?
The set relies on the key's value to maintain its order. If a key is modified in place, it might break the internal structure, making the set's behavior unreliable and potentially corrupt.

## Why should predicate functions be pure?
A pure predicate doesn't rely on or change any external state. This makes it predictable, easy to test, and safe to use in algorithms that might expect consistent and repeatable outcomes.

## Why should you avoid the default capture modes of lambdas?
Using default captures like [=] or [&] can accidentally include variables you didnt intend to use. This can lead to unexpected dependencies or bugs. It is safer to capture only what you really need.

## What do std::move and std::forward do?
std::move casts an object to an rvalue so its resources can be transferred instead of copied. std::forward is used in templates to perfectly pass values with their original value category (whether lvalue or rvalue).

## How are the strings of a std::setstd::string* sorted? How would you make them sorted lexicographically?
By default, std::set<std::string*> sorts based on the memory address of the pointers, not the strings they point to. To sort them alphabetically, you can use a custom comparator that compares the actual strings, like this:

struct LexCompare 
{
    bool operator()(const std::string* a, const std::string* b) const 
{
        return *a < *b;
    }
};
std::set<std::string*, LexCompare> mySet;
