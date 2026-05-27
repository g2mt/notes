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
- Store header files in `src/include/notes`

- When commiting, do not add anything to the message body. Instead, summarize what the commit does within only the commit title. Use the conventional commit format for the title: `type: description`.

## Libraries

- Qt 6 is used for this program.
- ProseMirror is used for the rich-text editor, embedded via QWebEngineView.
- The frontend TypeScript is compiled into a single bundle with esbuild.

## Architecture

- The Editor widget wraps a QWebEngineView that loads a ProseMirror editor page.
- Communication between C++ and JS uses QWebChannel for state notifications and runJavaScript for commands.
- Block the constructor on loadFinished with a QEventLoop so that runJs is safe immediately after construction.
- Use the callback variant of runJavaScript when a C++ side-effect must happen after the JS completes (e.g. resetting modified state after setMarkdown).

## UI

- Menu actions, buttons have to be in title-case. Capitalize the first level of every content word.

### Frontend

- TypeScript sources live in `frontend/src/`.
- Static assets (HTML, CSS) live in `frontend/static/`.
- Bundle output goes to `${CMAKE_BINARY_DIR}/frontend/prose.js`, referenced via a generated resources.qrc.
