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
  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
  spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

  m_filterProcessor.prepare(spec);
}

void SkuxAudioProcessor::releaseResources()
{
    m_filterProcessor.reset();
}


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

  const auto distMix = m_distMixParam->get();
  const auto distDrive = m_distDriveParam->get();
  const auto distType = m_distTypeParam->getIndex();
  const auto distFilterCutoff = m_distFilterCutoffParam->get();
  const auto distFilterRouting = m_distFilterRoutingParam->getIndex();
  const auto distFilterQ = m_distFilterQParam->get();
  
  if (distFilterRouting == 1)
    m_filterProcessor.process(buffer, distFilterCutoff, distFilterQ, distMix);

  m_distortionProcessor.process(buffer, distDrive, distMix, distType);

  if (distFilterRouting == 2)
    m_filterProcessor.process(buffer, distFilterCutoff, distFilterQ, distMix);

  for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
      buffer.clear(i, 0, buffer.getNumSamples());
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
                                                         20000.f));
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
