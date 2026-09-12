/**
 * @file tariff.h
 * @brief Dynamic tariff configuration, slab rules, and surcharge engine.
 * @author Akshar Miyani
 */

#ifndef TARIFF_H
#define TARIFF_H

#include "models.h"

/**
 * @brief Initializes tariff system, loading from config/tariffs.cfg or defaults.
 */
void tariff_init(void);

/**
 * @brief Retrieves tariff configuration pointer for a category.
 */
TariffConfig *tariff_get(ConnectionCategory cat);

/**
 * @brief Saves current active tariffs to config/tariffs.cfg.
 */
int tariff_save_to_file(void);

/**
 * @brief Loads tariffs from config/tariffs.cfg.
 */
int tariff_load_from_file(void);

/**
 * @brief Resets tariffs to standard defaults.
 */
void tariff_reset_defaults(void);

/**
 * @brief Interactive screen to display and inspect all category tariffs.
 */
void tariff_display_all(void);

/**
 * @brief Interactive screen allowing administrator to edit slab rates and taxes.
 */
void tariff_edit_menu(void);

/**
 * @brief Returns category string name.
 */
const char *category_to_string(ConnectionCategory cat);

#endif /* TARIFF_H */
