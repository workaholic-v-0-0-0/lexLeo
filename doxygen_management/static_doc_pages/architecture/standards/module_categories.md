@page module_categories Module categories

## Module classification (multi-axis)

A module is described by multiple **orthogonal axes** (not by a single exclusive category).
Each axis can be expressed as either:

- a **partition** (mutually exclusive choices), or
- a **binary tag** (property / not property).

---

### 1) Architectural role (partition)

Each module must be classified as exactly **one** of the following:

- **Port**
    - Defines a stable contract (API) that is consumed by other modules.

- **Adapter**
    - Implements a port using a specific backend/technology.

- **Service**

---

### 2) API shape (binary)

- **Handle-based** / **Stateless**
    - **Handle-based**: exposes an opaque instance handle (`module_t *`).
    - **Stateless**: exposes only functions/types (no instance handle).

---

### 3) Construction responsibility (binary)

- **Factory** / **Not a factory**
    - A **factory module** can create/destroy handles of **another module**.
    - A factory may be:
        - **handle-based** or
        - **stateless** (set of creation/destruction functions).

---

### 4) External dependency boundary (binary)

- **Boundary module** / **Pure internal module**
    - A **boundary module** sits at a boundary with something external to the module:
        - third-party libraries (Flex/Bison, SDL, etc.),
        - operating system / platform / libc specifics,
    - Its responsibility is to **contain and hide** the external dependency behind a project-owned API.
        - Example: `lexleo_flex` contains Flex and exposes a clean lexer adapter contract.

---

### 5) Wiring / composition boundary (binary)

- **Wiring module** / **Runtime module**
    - A **wiring module** belongs to the composition root.
    - A **runtime module** is used by normal consumers:
        - exposes a stable runtime API (opaque handle and/or port interface),
        - must not require wiring-only objects (`*_ctx_t`) to remain alive after creation,
        - is usable without any knowledge of the composition root.
