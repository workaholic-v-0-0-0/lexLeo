@page development_workflow Development workflow

## Rebuild, generate documentation and open it with a web browser

   ```bash
   ./scripts/re_debug_build-make_and_open_doc.sh
   ```

## Rebuild and run unit tests without memory checking

   ```bash
   ./scripts/re_debug_build-check.sh 
   ```

## Rebuild and run unit tests with memory checking

   ```bash
   ./scripts/re_debug_build-check_memory.sh
   ```

## Rebuild and update documentation on droplet (if SSH access is configured) :

   ```bash
   ./scripts/update_docs_on_droplet.sh 
   ```
