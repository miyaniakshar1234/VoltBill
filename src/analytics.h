/**
 * @file analytics.h
 * @brief Consumption analytics, Unicode sparkline charts, and carbon footprint engine.
 * @author Akshar Miyani
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
 * @brief Renders an inline Unicode sparkline waveform for historical data series.
 */
void analytics_draw_sparkline(const double *values, int count);

/**
 * @brief Computes environmental carbon footprint metrics for a given kWh consumption.
 */
void analytics_print_carbon_footprint(double total_kwh, double solar_kwh);

/**
 * @brief Renders real-time SCADA substation telemetry and grid load distribution.
 */
void analytics_scada_grid_monitor(void);

/**
 * @brief Dynamic Under-Frequency Load Shedding (UFLS) telemetry result.
 */
typedef struct {
    double initial_freq_hz;
    double rocof_hz_per_sec;        /* df/dt */
    int stage_tripped;              /* 0=Normal, 1=Stage 1, 2=Stage 2, 3=Stage 3 */
    double load_shed_kw;            /* Amount shed */
    double recovered_freq_hz;       /* Projected stabilized frequency */
    int feeders_tripped;
    const char *tripped_sectors;
} UFLSResult;

/**
 * @brief Simulates dynamic Under-Frequency Load Shedding (UFLS) algorithm.
 */
void analytics_simulate_ufls(double initial_freq_hz, double rocof_hz_per_sec, UFLSResult *out_res);

/**
 * @brief Interactive screen demonstrating UFLS automatic load shedding.
 */
void analytics_render_ufls_screen(double trigger_freq, double rocof_hz_per_sec);

#endif /* ANALYTICS_H */
