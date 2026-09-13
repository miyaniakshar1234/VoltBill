/**
 * @file modbus.h
 * @brief Industrial Modbus RTU/TCP Protocol Decoder, CRC-16 Engine & Telemetry Parser.
 * @author Akshar Miyani
 */

#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief 32-bit Floating Point Word Endianness Models
 */
typedef enum {
    MODBUS_ENDIAN_ABCD = 0, /* Big-Endian (Motorola): High word first, High byte first */
    MODBUS_ENDIAN_CDAB = 1, /* Word-Swapped (Mid-Little): Low word first, High byte first */
    MODBUS_ENDIAN_BADC = 2, /* Byte-Swapped: High word first, Low byte first */
    MODBUS_ENDIAN_DCBA = 3  /* True Little-Endian (Intel): Low word first, Low byte first */
} ModbusEndian;

/**
 * @brief Decoded Industrial Meter Telemetry
 */
typedef struct {
    uint8_t slave_id;
    uint8_t function_code;
    double voltage_v;           /* Line-to-Neutral / Line-to-Line (V) */
    double current_a;           /* Phase current (A) */
    double active_power_kw;     /* Active Real Power P (kW) */
    double reactive_power_kvar; /* Reactive Power Q (kVAR) */
    double apparent_power_kva;  /* Apparent Power S (kVA) */
    double power_factor;        /* cos phi (0.0 to 1.0) */
    double frequency_hz;        /* System frequency (Hz) */
    double active_energy_kwh;   /* Total cumulative active energy (kWh) */
    int is_crc_valid;
    uint16_t computed_crc;
    uint16_t packet_crc;
} ModbusTelemetry;

/**
 * @brief Computes standard Modbus RTU CRC-16 (Polynomial 0xA001, Initial 0xFFFF).
 */
uint16_t modbus_crc16(const uint8_t *buffer, size_t length);

/**
 * @brief Parses IEEE 754 32-bit float from two 16-bit Modbus registers.
 */
float modbus_registers_to_float(uint16_t reg0, uint16_t reg1, ModbusEndian endian);

/**
 * @brief Encodes a float into two 16-bit Modbus registers.
 */
void modbus_float_to_registers(float val, ModbusEndian endian, uint16_t *out_reg0, uint16_t *out_reg1);

/**
 * @brief Parses raw Modbus RTU response frame into telemetry metrics.
 */
int modbus_parse_rtu_frame(const uint8_t *frame, size_t len, ModbusEndian endian, ModbusTelemetry *out);

/**
 * @brief Synthesizes an authentic Modbus RTU telemetry frame for testing/simulation.
 */
size_t modbus_synthesize_rtu_frame(uint8_t slave_id, double voltage, double current,
                                   double power_kw, double pf, double freq, double energy_kwh,
                                   ModbusEndian endian, uint8_t *out_buffer, size_t max_len);

/**
 * @brief Renders lavish interactive Modbus hex packet inspector & decoded telemetry card.
 */
void modbus_render_packet_inspector(const uint8_t *frame, size_t len, const ModbusTelemetry *tel);

/**
 * @brief CLI entrypoint to inspect or simulate Modbus RTU streams.
 */
void modbus_cli_demo(const char *hex_str);

#endif /* MODBUS_H */
