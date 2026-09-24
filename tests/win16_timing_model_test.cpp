#include "nitemare3d/win16_timing_model.hpp"

#include <cassert>
#include <cstdint>

using nitemare3d::win16::TickCounterState;
using nitemare3d::win16::advance_frame_counter;
using nitemare3d::win16::advance_slow_counter;
using nitemare3d::win16::derive_calibration_from_five_render_calls;
using nitemare3d::win16::frame_time_bucket;
using nitemare3d::win16::slow_time_bucket_8hz;

int main() {
    assert(slow_time_bucket_8hz(0U) == 0U);
    assert(slow_time_bucket_8hz(124U) == 0U);
    assert(slow_time_bucket_8hz(125U) == 1U);
    assert(slow_time_bucket_8hz(1000U) == 8U);
    assert(slow_time_bucket_8hz(0x20000000U) == 0U);  // 32-bit left-shift wrap

    TickCounterState slow{0U, 0U};
    assert(advance_slow_counter(1000U, slow) == 1U);  // one tick, not eight
    assert(advance_slow_counter(1000U, slow) == 1U);
    assert(advance_slow_counter(1125U, slow) == 2U);

    assert(frame_time_bucket(40U, 25U) == 1U);
    TickCounterState frame{0U, 0U};
    assert(advance_frame_counter(1000U, 25U, frame) == 1U);
    assert(advance_frame_counter(1000U, 25U, frame) == 1U);

    const auto fast = derive_calibration_from_five_render_calls(50U);
    assert(fast.has_value());
    assert(fast->stored_raw_average_ms == 10U);
    assert(fast->effective_average_ms == 40U);
    assert(fast->frame_rate == 25U);
    assert(fast->movement_step == 10U);
    assert(fast->angle_step == 5U);
    assert(fast->projectile_substeps == 20U);

    const auto normal = derive_calibration_from_five_render_calls(500U);
    assert(normal.has_value());
    assert(normal->stored_raw_average_ms == 100U);
    assert(normal->effective_average_ms == 100U);
    assert(normal->frame_rate == 10U);
    assert(normal->movement_step == 25U);
    assert(normal->angle_step == 13U);
    assert(normal->projectile_substeps == 50U);

    assert(!derive_calibration_from_five_render_calls(327680U).has_value());
}
