# Coding conventions

- Use 2 spaces for indenting.
- Prioritize simple code with minimal dependencies.
- Add comments ONLY if it's not immediately obvious from a cursory glance of the code.
- Organize your class files in the method order:
  1. Constructor/destructor
  2. Helper/setup methods
  3. Getter/setter methods
  4. Event handlers (optional, prefer lambdas)
  5. Signals (in headers)
- Top level static definitions (functions and variables) need to be **directly before** the function they are used in.
- For class implementations with many functions, separate each relevant section by comments. Comments MUST follow the format below. For instance:
  ```cpp
  //
  // Markdown Parsing
  //
  ```
- Store header files in `src/include/notes`.
- When commiting, do not add anything to the message body. Instead, summarize what the commit does within only the commit title. Use the conventional commit format for the title: `type: description`.

## UI

- Qt 6 is used for UI
- Menu actions, buttons have to be in title-case. Capitalize the first level of every content word.

## Documents

All AI-generated text (Markdown files, not code) must have the following preamble:

```markdown
> **Warning:** This file was generated with AI assistance.
```

Put all draft documents into `docs/draft`. Do NOT use them as reference unless explicitly allowed.

