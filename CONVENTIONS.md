# Coding conventions

- Use 2 spaces for indenting.
- Prioritize simple code with minimal dependencies.
- Add comments ONLY if it's not immediately obvious from a cursory glance of the code.
- Organize your class files in the method order:
  1. Constructor/destructor
  2. Setup methods
  3. Event handlers (optional, prefer lambdas)
  4. Signals (in headers)
- Store header files in `src/include/notes`

- When commiting, do not add anything to the message body. Instead, summarize what the commit does within only the commit title. Use the conventional commit format for the title: `type: description`.

## Libraries

- The md4c library is used to parse Markdown. Its source is available in `vendor/md4c/src`.
