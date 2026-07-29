

#ifndef LEXLEO_LEXLEO_APP_CR_OPAQUE_API_H
#define LEXLEO_LEXLEO_APP_CR_OPAQUE_API_H

#include "lexleo_app/cr_opaque/lexleo_app_cr_opaque_types.h"
#include "lexleo_app/borrowers/lexleo_app_borrowers_types.h"

#include "policy/lexleo_cstd_types.h"

/**
 * @brief Returns the default LexLeo application configuration.
 *
 * @return Default LexLeo application configuration value.
 *
 * See contract:
 * - @ref specifications_lexleo_app_default_cfg
 */
lexleo_app_cfg_t lexleo_app_default_cfg(void);

/**
 * @brief Creates and initializes a LexLeo application handle.
 *
 * @param out Receives the created application handle on success.
 * @param cfg Application configuration.
 * @return true on success, false on failure.
 *
 * See contract:
 * - @ref specifications_lexleo_app_create_init
 */
bool lexleo_app_create_init(
	lexleo_app_t **out,
	const lexleo_app_cfg_t *cfg
);

/**
 * @brief Destroys a LexLeo application handle.
 *
 * @details
 * Releases the application handle and every owned runtime resource attached
 * to it, then resets `*vm` to `NULL`.
 *
 * @param[in,out] app Address of the application handle to destroy.
 *
 * See contract:
 * - @ref specifications_lexleo_app_destroy
 */
void lexleo_app_destroy(lexleo_app_t **app);

#endif /* LEXLEO_LEXLEO_APP_CR_OPAQUE_API_H */
