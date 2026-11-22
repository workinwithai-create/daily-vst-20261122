# Autumn Veil — Daily VST 20261122

**Today’s Instrument: Autumn Veil**  
A dark cinematic pad synthesizer that breathes with aftertouch and holds notes via the sustain pedal, evoking late-autumn mist.

## Features
- 8-voice polyphonic subtractive pad (sine + StateVariableTPT lowpass)
- Channel & poly aftertouch modulate amplitude (breathing) and filter
- Sustain pedal (CC64) fully supported via juce::Synthesiser
- APVTS parameters with smoothing
- Safety limiter
- Full state save/load
- Dark cinematic UI

## Build
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
Artefacts appear in `build/AutumnVeil_artefacts/Release/`.

Requires CMake ≥ 3.22, C++20. JUCE 8.0.14 is fetched automatically.

## GitHub Actions
Push to `main` or run the workflow manually to build macOS + Windows Release VST3 + Standalone and create a GitHub Release with zipped artefacts.
