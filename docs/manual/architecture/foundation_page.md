@page foundation_page Foundation architecture

Foundational architecture defines the stable rules of the system:
construction boundaries (CR vs runtime), dependency injection points, visibility rules,
and the taxonomy used to classify modules.

## Foundation map

| Topic | Description |
|---|---|
| @subpage foundation_architecture_overview "Foundation overview" | The core model (CR/runtime/teardown), ctx split (cfg/env), DI rules, and invariants. |
| @subpage module_taxonomy "Module taxonomy" | How modules are classified (port/adapter/handle-based/factory-provider/stateful) and what layout rules follow. |
