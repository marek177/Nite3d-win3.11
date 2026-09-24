#pragma once

#include <cstdint>
#include <optional>

namespace nitemare3d::win16 {

// State corresponding to one of the game's 32-bit logical counters.
// Initialize both fields from the emulated build's data before sampling.
struct TickCounterState {
    std::uint32_t previous_bucket;
    std::uint32_t logical_counter;
};

struct CalibrationResult {
    std::uint16_t stored_raw_average_ms;
    std::uint16_t frame_rate;
    std::uint16_t movement_step;
    std::uint16_t angle_step;
    std::uint16_t projectile_substeps;
    std::uint32_t effective_average_ms;
};

// Reproduces ((uint32_t(time_ms) << 3) modulo 2^32) / 1000.
std::uint32_t slow_time_bucket_8hz(std::uint32_t time_ms) noexcept;

// Advance exactly once when the sampled bucket differs. Skipped buckets are
// not replayed as catch-up ticks.
std::uint32_t advance_slow_counter(std::uint32_t time_ms,
                                  TickCounterState& state) noexcept;

// Reproduces low32(sign_extend_16(frame_rate) * time_ms) / 1000.
std::uint32_t frame_time_bucket(std::uint32_t time_ms,
                                std::uint16_t frame_rate) noexcept;

// Advance the calibrated frame counter once when its bucket changes.
std::uint32_t advance_frame_counter(std::uint32_t time_ms,
                                   std::uint16_t frame_rate,
                                   TickCounterState& state) noexcept;

// Model the normal, bounded calibration path. elapsed_ms is the measured
// total for the five render/present calls. Inputs whose raw mean exceeds a
// 16-bit value are rejected because extreme/overflow behavior is still open.
std::optional<CalibrationResult> derive_calibration_from_five_render_calls(
    std::uint32_t elapsed_ms) noexcept;

}  // namespace nitemare3d::win16
