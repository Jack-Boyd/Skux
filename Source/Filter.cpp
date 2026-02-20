#include "Filter.h"

void Filter::prepare(const juce::dsp::ProcessSpec& spec)
{
  m_sampleRate = spec.sampleRate;
  m_filterMixBuffer.setSize(static_cast<int>(spec.numChannels),
                            static_cast<int>(spec.maximumBlockSize));
  m_filter.prepare(spec);
}

void Filter::reset()
{
  m_filter.reset();
  m_lastFilterCutoff = -1.f;
  m_lastFilterQ = -1.f;
}

void Filter::process(juce::AudioBuffer<float>& buffer, float cutoff, float q, float mix)
{
  const auto numChannels = buffer.getNumChannels();
  const auto numSamples = buffer.getNumSamples();

  if (cutoff != m_lastFilterCutoff || q != m_lastFilterQ) {
    *m_filter.get<0>().state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(m_sampleRate, cutoff, q);
    *m_filter.get<1>().state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(m_sampleRate, cutoff, 0.707f);

    m_lastFilterCutoff = cutoff;
    m_lastFilterQ = q;
  }

  juce::dsp::AudioBlock<float> block(buffer);
  juce::dsp::ProcessContextReplacing<float> context(block);

  if (mix >= 1.f) {
    m_filter.process(context);
    return;
  }

  for (int ch = 0; ch < numChannels; ++ch) {
    m_filterMixBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
  }

  m_filter.process(context);

  for (int ch = 0; ch < numChannels; ++ch) {
    auto* wet = buffer.getWritePointer(ch);
    const auto* dry = m_filterMixBuffer.getReadPointer(ch);

    for (int s = 0; s < numSamples; ++s) {
      wet[s] = wet[s] * mix + dry[s] * (1.f - mix);
    }
  }
}
