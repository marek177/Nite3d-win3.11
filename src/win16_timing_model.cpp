#include "nitemare3d/win16_timing_model.hpp"

#include <algorithm>
#include <limits>

namespace nitemare3d::win16 {
namespace {

std::uint32_t advance_counter(std::uint32_t bucket,
                              TickCounterState& state) noexcept {
    if (bucket != state.previous_bucket) {
        state.previous_bucket = bucket;
        ++state.logical_counter;  // unsigned overflow matches a 32-bit counter
    }
    return state.logical_counter;
}

}  // namespace

std::uint32_t slow_time_bucket_8hz(std::uint32_t time_ms) noexcept {
    // Unsigned arithmetic intentionally wraps modulo 2^32 before division.
    return (time_ms * std::uint32_t{8}) / std::uint32_t{1000};
}

std::uint32_t advance_slow_counter(std::uint32_t time_ms,
                                  TickCounterState& state) noexcept {
    return advance_counter(slow_time_bucket_8hz(time_ms), state);
}

std::uint32_t frame_time_bucket(std::uint32_t time_ms,
                                std::uint16_t frame_rate) noexcept {
    const std::int32_t signed_rate =
        (frame_rate & 0x8000U) != 0U
            ? static_cast<std::int32_t>(frame_rate) - 0x10000
            : static_cast<std::int32_t>(frame_rate);

    // Conversion to uint32_t and multiplication both wrap modulo 2^32.
    const auto rate_bits = static_cast<std::uint32_t>(signed_rate);
    const std::uint32_t product = rate_bits * time_ms;
    return product / std::uint32_t{1000};
}

std::uint32_t advance_frame_counter(std::uint32_t time_ms,
                                   std::uint16_t frame_rate,
                                   TickCounterState& state) noexcept {
    return advance_counter(frame_time_bucket(time_ms, frame_rate), state);
}

std::optional<CalibrationResult> derive_calibration_from_five_render_calls(
    std::uint32_t elapsed_ms) noexcept {
    const std::uint32_t raw_mean_ms = elapsed_ms / std::uint32_t{5};
    if (raw_mean_ms > std::numeric_limits<std::uint16_t>::max()) {
        return std::nullopt;
    }

    const std::uint32_t effective_ms = std::max(raw_mean_ms, std::uint32_t{40});
    const std::uint32_t frame_rate =
        (std::uint32_t{1000} + effective_ms / std::uint32_t{2}) / effective_ms;
    const std::uint32_t movement_step = std::max(
        std::uint32_t{1}, (effective_ms + std::uint32_t{2}) / std::uint32_t{4});
    const std::uint32_t angle_step = std::max(
        std::uint32_t{1},
        (std::uint32_t{360} * effective_ms + std::uint32_t{1400}) /
            std::uint32_t{2800});
    const std::uint32_t projectile_substeps = std::uint32_t{2} * movement_step;

    return CalibrationResult{
        static_cast<std::uint16_t>(raw_mean_ms),
        static_cast<std::uint16_t>(frame_rate),
        static_cast<std::uint16_t>(movement_step),
        static_cast<std::uint16_t>(angle_step),
        static_cast<std::uint16_t>(projectile_substeps),
        effective_ms,
    };
}

}  // namespace nitemare3d::win16
