# C++ Project Rules & Guidelines

## Language Standard
- Use C++23 unless explicitly specified otherwise.
- Do not use compiler-specific extensions.
- Prefer standard library facilities over third-party libraries.
- Note: std::expected is C++23; for earlier standards use std::optional or appropriate alternatives.

---

## Design Philosophy

### 1. Prefer Rule of Zero
- Use RAII for all resource management.
- Prefer std::unique_ptr, std::shared_ptr, std::vector, std::string.
- Avoid raw owning pointers.
- Only implement Rule of Five when managing resources manually.

### 2. Value-Oriented Design
- Prefer value semantics.
- Types should behave like values unless identity is required.
- Avoid unnecessary heap allocation.

### 3. Make Invalid States Unrepresentable
- Use strong types.
- Use std::optional, std::variant, std::expected for state modeling.
- Use constructors to enforce invariants.

### 4. Explicit Ownership
- Ownership must be clear from type.
- Raw pointer = non-owning. Use observer_ptr when possible
- unique_ptr = exclusive ownership.
- shared_ptr only when shared lifetime is required.

---

## Error Handling
- Do not use exceptions for normal control flow.
- Prefer std::expected for recoverable errors.
- Use assertions (assert/static_assert) for programmer errors and invariants.
- Reserve exceptions for truly exceptional conditions (resource exhaustion, hardware failures).
- Never ignore return values.
- Use [[nodiscard]] for error-returning functions and functions where ignoring the return value is an error.

---

## Modern C++ Features (C++20/23)
- Prefer ranges and views over raw iterators.
- Use concepts to constrain templates explicitly.
- Use std::span and std::string_view for non-owning views.
- Use designated initializers for aggregate types.
- Consider std::format over iostream for formatting.
- Use constexpr liberally for compile-time computation.
- Modules are preferred over headers when tooling supports them.

---

## Performance Rules
- Avoid unnecessary dynamic allocations.
- "Small types" = trivially copyable types ≤ 16 bytes.
- Pass small types by value, large types by const reference.
- For non-owning references, prefer std::span/std::string_view over const&.
- Avoid virtual dispatch unless required.
- Prefer compile-time polymorphism (templates, concepts).
- Use move semantics correctly.
- Mark functions constexpr when possible for compile-time execution.
- Profile before optimizing; measure, don't guess.

---

## Concurrency
- Prefer std::jthread over std::thread.
- Avoid data races.
- Use std::mutex, std::scoped_lock for locking.
- Use std::atomic for lock-free single variables.
- Default to std::memory_order_seq_cst unless you understand relaxed ordering.
- Avoid lock-free data structures unless proven necessary.
- Never expose raw synchronization primitives in interfaces.
- Design for immutability when possible.
- Consider std::latch, std::barrier, std::counting_semaphore for coordination.

---

## API Design
- Mark single-argument constructors explicit unless conversion is intentional.
- Use [[nodiscard]] for functions where ignoring return value is an error.
- Prefer free functions over member functions when appropriate.
- Keep classes small and focused.
- Follow Single Responsibility Principle.
- Avoid god objects.
- Mark functions const when possible.
- Use noexcept where appropriate.
- Prefer enum class over plain enum.
- Avoid bool parameters; use enum or separate functions for clarity.
- Provide both const& and && overloads for setter methods when appropriate.

---

## Standard Library Guidance
- Prefer algorithms (std::ranges, &lt;algorithm&gt;) over raw loops.
- Use std::array over C arrays.
- Use std::string_view for read-only string parameters.
- Use std::span for array/container views.
- Prefer emplace over push for containers.
- Use structured bindings for clearer tuple/pair decomposition.

---

## Header Rules
- Use include guards or #pragma once.
- Minimize includes in headers.
- Do not include heavy headers in other headers.
- Prefer forward declarations.
- Never use `using namespace std` in headers.

---

## Naming Conventions
- **Types**: PascalCase (e.g., `MyClass`, `ErrorCode`)
- **Functions**: camelCase (e.g., `calculateSum`, `processData`)
- **Variables**: snake_case (e.g., `user_count`, `max_value`)
- **Member variables**: snake_case_ with trailing underscore (e.g., `data_`, `count_`)
- **Constants**: kConstantName (e.g., `kMaxSize`, `kDefaultTimeout`)
- **Namespaces**: lowercase, short, no underscores (e.g., `utils`, `net`)
- **Template parameters**: PascalCase (e.g., `T`, `ValueType`, `Allocator`)
- **Enum class members**: PascalCase or kConstantName

---

## Quality Assurance

### Compiler Settings
- Enable all warnings: `-Wall -Wextra -Wpedantic` (GCC/Clang) or `/W4` (MSVC)
- Treat warnings as errors in CI: `-Werror` (GCC/Clang) or `/WX` (MSVC)
- Use appropriate optimization levels for release builds

### Static Analysis
- Run clang-tidy with modern checks enabled
- Use compiler sanitizers regularly:
  - AddressSanitizer (ASan) for memory errors
  - UndefinedBehaviorSanitizer (UBSan) for undefined behavior
  - ThreadSanitizer (TSan) for data races

### Testing
- All non-trivial logic must have unit tests
- Prefer modern testing frameworks: Catch2, Google Test, or doctest
- Design for testable interfaces (dependency injection, pure functions)
- Test edge cases, not just happy paths
- Aim for high code coverage on critical paths

---

## Documentation & Code Style
- Use `///` or `/** */` for API documentation
- Document preconditions, postconditions, and invariants
- Explain *why*, not *what* (code shows what)
- Keep line length ≤ 100-120 characters
- Use consistent formatting (clang-format recommended)
- Document non-obvious design decisions
- Add examples for complex APIs

---

## What NOT To Do
- No raw new/delete
- No C-style arrays; use std::array or std::vector
- No global mutable state
- No macros unless absolutely necessary
- No `using namespace std` in headers
- No inheritance for code reuse — prefer composition
- No C-style casts; use static_cast, const_cast, etc. sparingly
- No implicit conversions; mark single-argument constructors explicit
- No multiple inheritance except for pure interfaces (abstract base classes)
- No manual memory management when standard containers suffice

---

## When Generating Code

Claude must:
- Explain design decisions briefly
- Prefer clarity over cleverness
- Avoid overengineering
- Show complete compilable examples
- Follow all rules above unless explicitly told otherwise
- Use modern C++ idioms and features
- Include necessary headers and namespace declarations
- Ensure code is const-correct and noexcept-aware