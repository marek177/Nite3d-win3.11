# Nitemare 3D Win16 reverse-engineering notes

This repository records findings for the Windows 3.x / Windows 3.11 16-bit builds of Nitemare 3D. The available audit covers four supplied executables identified as versions 1.3, 1.6, 1.8, and 1.10.

## Contents

- [`docs/win16/audit-2026-09-24.md`](docs/win16/audit-2026-09-24.md) — the detailed, original-language audit, including executable hashes, cross-version findings, evidence limits, open questions, and proposed runtime checks.
- [`include/nitemare3d/win16_timing_model.hpp`](include/nitemare3d/win16_timing_model.hpp) and [`src/win16_timing_model.cpp`](src/win16_timing_model.cpp) — a small portable C++ reconstruction of the confirmed 8 Hz and calibrated-frame counters and the bounded calibration formulas.
- [`tests/win16_timing_model_test.cpp`](tests/win16_timing_model_test.cpp) — checks for the recovered arithmetic, including skipped-tick behavior and the documented 10 ms calibration example.

## Source-code status

The original game source code was not present in the analyzed materials. The Ghidra C exports are decompiler output, not the authors' source. The C++ files in this repository are an independent, partial reconstruction from the static findings; they are not a complete game port and do not claim runtime equivalence.

The timing implementation covers the confirmed counter arithmetic and the normal bounded calibration path. It does not model the full scheduler, the nonzero-mode branch, all game systems, or unresolved extreme/overflow behavior. See the audit for confidence labels and remaining questions. Runtime samples were not collected for that audit.

## Build and run the checks

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```
