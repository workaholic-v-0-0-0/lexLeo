

#ifndef LEXLEO_LEXLEO_APP_H
#define LEXLEO_LEXLEO_APP_H

#include "lexleo_app/lexleo_app_types.h"

lexleo_app_cfg_t lexleo_app_default_cfg(void);
void lexleo_app_cfg_set_input_file(
	lexleo_app_cfg_t *cfg,
	const char *path);
void lexleo_app_cfg_set_output_file(
	lexleo_app_cfg_t *cfg,
	const char *path);
void lexleo_app_cfg_set_error_file(
	lexleo_app_cfg_t *cfg,
	const char *path);

lexleo_app_status_t lexleo_app_create(
	lexleo_app_t **out,
	const lexleo_app_cfg_t *cfg
);
void lexleo_app_destroy(lexleo_app_t **app);

lexleo_app_status_t lexleo_app_complete_default_init(lexleo_app_t *app);

lexleo_app_status_t lexleo_app_run(lexleo_app_t *app);

/* todo
lexleo_app_status_t lexleo_app_write_output(lexleo_app_t *app, const char *message);
lexleo_app_status_t lexleo_app_write_input(lexleo_app_t *app, const char *message);
lexleo_app_status_t lexleo_app_write_error(lexleo_app_t *app, const char *message);
lexleo_app_status_t lexleo_app_log(lexleo_app_t *app, const char *message);
*/

#endif /* LEXLEO_LEXLEO_APP_H */
