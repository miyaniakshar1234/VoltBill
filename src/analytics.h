/**
 * @file analytics.h
 * @brief Consumption analytics, Unicode sparkline charts, and carbon footprint engine.
 * @author Akshar Miyani (MCA 1st Sem, Manipal University Jaipur)
 */

#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "models.h"

/**
 * @brief Displays high-level system metrics, revenue collected, and grid load.
 */
void analytics_system_overview(void);

/**
 * @brief Displays detailed historical bar chart and carbon footprint for a consumer.
 */
void analytics_consumer_deepdive(void);

/**
 * @brief Renders a sleek Unicode bar chart for given data values.
 */
void analytics_draw_barchart(const double *values, const char **labels, int count, double max_val);

/**
 * @brief Computes environmental carbon footprint metrics for a given kWh consumption.
 */
void analytics_print_carbon_footprint(double total_kwh, double solar_kwh);

#endif /* ANALYTICS_H */
