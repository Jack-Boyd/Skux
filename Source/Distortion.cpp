#include "Distortion.h"

void Distortion::process(juce::AudioBuffer<float>& buffer,
                         float drive, float mix, int clipType)
{
  if (mix <= 0.f)
    return;

  const auto numChannels = buffer.getNumChannels();
  const auto numSamples = buffer.getNumSamples();
  const auto dryGain = 1.f - mix;
  const auto wetGain = (clipType == 0)
    ? mix / std::pow(drive, 0.45f)
    : mix / std::pow(drive, 0.6f);

  for (int ch = 0; ch < numChannels; ++ch) {
    auto* data = buffer.getWritePointer(ch);

    for (int s = 0; s < numSamples; ++s) {
      const auto dry = data[s];
      const auto driven = dry * drive;

      data[s] = dry * dryGain
        + ((clipType == 0)
          ? fastTanh(driven)
          : std::clamp(driven, -1.f, 1.f))
        * wetGain;
    }
  }
}
