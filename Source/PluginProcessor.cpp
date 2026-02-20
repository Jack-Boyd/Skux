#include "PluginProcessor.h"
#include "PluginEditor.h"

SkuxAudioProcessor::SkuxAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
  m_distDriveParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Drive"));
  m_distMixParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Mix"));
  m_distTypeParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Type"));
  
  m_distFilterCutoffParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Filter Cutoff"));
  m_distFilterRoutingParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Filter Routing"));
  m_distFilterQParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Filter Q"));
  
  jassert(m_distDriveParam != nullptr);
  jassert(m_distMixParam != nullptr);
  jassert(m_distTypeParam != nullptr);
  jassert(m_distFilterCutoffParam != nullptr);
  jassert(m_distFilterRoutingParam != nullptr);
  jassert(m_distFilterQParam != nullptr);
}

SkuxAudioProcessor::~SkuxAudioProcessor() {}

const juce::String SkuxAudioProcessor::getName() const
{
  return JucePlugin_Name;
}

bool SkuxAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SkuxAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SkuxAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SkuxAudioProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

int SkuxAudioProcessor::getNumPrograms()
{
  return 1;
}

int SkuxAudioProcessor::getCurrentProgram()
{
  return 0;
}

void SkuxAudioProcessor::setCurrentProgram (int index) {}

const juce::String SkuxAudioProcessor::getProgramName (int index)
{
  return {};
}

void SkuxAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void SkuxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  m_filterMixBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);
  
  juce::dsp::ProcessSpec spec;
  spec.maximumBlockSize = samplesPerBlock;
  spec.numChannels = getTotalNumOutputChannels();
  spec.sampleRate = sampleRate;

  m_filter.prepare(spec);
}

void SkuxAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SkuxAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SkuxAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
  juce::ScopedNoDenormals noDenormals;
  const auto totalNumInputChannels  = getTotalNumInputChannels();
  const auto totalNumOutputChannels = getTotalNumOutputChannels();
  const auto numSamples = buffer.getNumSamples();

  const auto distMix = m_distMixParam->get();
  const auto distDrive = m_distDriveParam->get();
  const auto distType = m_distTypeParam->getIndex();
  const auto distFilterCutoff = m_distFilterCutoffParam->get();
  const auto distFilterRouting = m_distFilterRoutingParam->getIndex();
  const auto distFilterQ = m_distFilterQParam->get();
  
  if (distFilterCutoff != m_lastDistFilterCutoff || distFilterQ != m_lastDistFilterQ) {
    *m_filter.get<0>().state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
        getSampleRate(), distFilterCutoff, distFilterQ);
    *m_filter.get<1>().state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
        getSampleRate(), distFilterCutoff, 0.707f);

    m_lastDistFilterCutoff = distFilterCutoff;
    m_lastDistFilterQ = distFilterQ;
  }

  juce::dsp::AudioBlock<float> block(buffer);
  juce::dsp::ProcessContextReplacing<float> context(block);
  
  auto applyFilterWithMix = [&]() {
    if (distMix >= 1.f) {
      m_filter.process(context);
    }
    else {
      for (int ch = 0; ch < totalNumInputChannels; ++ch) {
        m_filterMixBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
      }
      
      m_filter.process(context);

      for (int ch = 0; ch < totalNumInputChannels; ++ch) {
        auto* wet = buffer.getWritePointer(ch);
        const auto* dry = m_filterMixBuffer.getReadPointer(ch);

        for (int s = 0; s < numSamples; ++s) {
          wet[s] = wet[s] * distMix + dry[s] * (1.f - distMix);
        }
      }
    }
  };
  
  
  if (distFilterRouting == 1)
    applyFilterWithMix();
  
  if (distMix > 0.f) {
    const auto dryGain = 1.f - distMix;
    const auto wetGain = (distType == 0) ? distMix / std::pow(distDrive, 0.45f) : distMix / std::pow(distDrive, 0.6f);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      auto* channelData = buffer.getWritePointer(channel);
      
      if (distType == 0) {
        for (int s = 0; s < numSamples; ++s) {
          const auto drySample = channelData[s];
          channelData[s] = drySample * dryGain + fastTanh(drySample * distDrive) * wetGain;
        }
      }
      else {
        for (int s = 0; s < numSamples; ++s) {
          const auto drySample = channelData[s];
          channelData[s] = drySample * dryGain + std::clamp(drySample * distDrive, -1.f, 1.f) * wetGain;
        }
      }
    }
  }
  
  if (distFilterRouting == 2)
    applyFilterWithMix();
  
  for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
    buffer.clear(i, 0, numSamples);
  }
}

bool SkuxAudioProcessor::hasEditor() const
{
  return true;
}

juce::AudioProcessorEditor* SkuxAudioProcessor::createEditor()
{
//  return new SkuxAudioProcessorEditor (*this);
  return new juce::GenericAudioProcessorEditor(*this);
}

void SkuxAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
  auto state = apvts.copyState();
  auto xml = state.createXml();
  copyXmlToBinary(*xml, destData);
}

void SkuxAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
  auto xml = getXmlFromBinary(data, sizeInBytes);
  if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
    apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SkuxAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SkuxAudioProcessor::createParameterLayout()
{
  APVTS::ParameterLayout layout;
  
  layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Filter Cutoff", 1),
                                                         "Filter Cutoff",
                                                         juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                         20.f));
  layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("Filter Routing", 1),
                                                          "Filter Routing",
                                                          juce::StringArray { "Off", "Pre", "Post" },
                                                          0));
  layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Filter Q", 1),
                                                         "Filter Q",
                                                         juce::NormalisableRange<float>(0.707f, 10.f, 0.01f, 0.5f),
                                                         0.707f));
  layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("Type", 1),
                                                          "Type",
                                                          juce::StringArray { "Soft Clip", "Hard Clip" },
                                                          0));
  layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Drive", 1),
                                                         "Drive",
                                                         juce::NormalisableRange<float>(1.f, 12.f, 0.01f, 0.5f),
                                                         1.f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Mix", 1),
                                                         "Mix",
                                                         juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f),
                                                         0.f));

  return layout;
}
