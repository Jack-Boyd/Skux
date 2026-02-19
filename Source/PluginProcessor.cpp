/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
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
//  typeParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Distortion Type"));
//  driveParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Distortion Drive"));
//  mixParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Distortion Mix"));
  filterParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Filter Value"));
//  
//  jassert(typeParam != nullptr);
//  jassert(driveParam != nullptr);
//  jassert(mixParam != nullptr);
  jassert(filterParam !=   nullptr);
}

SkuxAudioProcessor::~SkuxAudioProcessor() {}

//==============================================================================
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
  return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
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

//==============================================================================
void SkuxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  juce::dsp::ProcessSpec spec;
  spec.maximumBlockSize = samplesPerBlock;
  spec.numChannels = getTotalNumOutputChannels();
  spec.sampleRate = sampleRate;

  filter.prepare(spec);
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
  //  const auto totalNumInputChannels  = getTotalNumInputChannels();
  //  const auto totalNumOutputChannels = getTotalNumOutputChannels();
  //  const auto numSamples = buffer.getNumSamples();
  
  float cutoff = filterParam->get();
  auto coefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
    cutoff,
    getSampleRate(),
    4.f
  );

  if (coefficients.size() > 1) {
    *filter.get<0>().state = *coefficients[0];
    *filter.get<1>().state = *coefficients[1];
  }

  juce::dsp::AudioBlock<float> block(buffer);
  juce::dsp::ProcessContextReplacing<float> context(block);
  filter.process(context);
  
//  const auto drive = driveParam->get();
//  const auto mix = mixParam->get();
//  
//  if (mix == 0.f)
//    return;
//  
//  const auto clipType = typeParam->getIndex();
//  const auto dryGain = 1.f - mix;
//  const auto wetGain = (clipType == 0)
//    ? mix / std::sqrt(std::max(1.f, drive))
//    : mix / std::pow(drive, 0.1f);
//  
//  for (int channel = 0; channel < totalNumInputChannels; ++channel) {
//    auto* channelData = buffer.getWritePointer(channel);
//    
//    if (clipType == 0) {
//      for (int s = 0; s < numSamples; ++s) {
//        const auto drySample = channelData[s];
//        channelData[s] = drySample * dryGain + fastTanh(drySample * drive) * wetGain;
//      }
//    }
//    else {
//      for (int s = 0; s < numSamples; ++s) {
//        const auto drySample = channelData[s];
//        channelData[s] = drySample * dryGain + std::clamp(drySample * drive, -1.f, 1.f) * wetGain;
//      }
//    }
//  }
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

//==============================================================================
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SkuxAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SkuxAudioProcessor::createParameterLayout()
{
  APVTS::ParameterLayout layout;
  
  layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Filter Value", 1),
                                                         "Filter Value",
                                                         juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                         20.f));
  
//  layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("Distortion Type", 1),
//                                                          "Distortion Type",
//                                                          juce::StringArray { "Soft Clip", "Hard Clip" },
//                                                          0));
//  layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Distortion Mix", 1),
//                                                         "Distortion Mix",
//                                                         juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f),
//                                                         0.f));
//  layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Distortion Drive", 1),
//                                                         "Distortion Drive",
//                                                         juce::NormalisableRange<float>(1.f, 12.f, 0.01f, 0.5f),
//                                                         1.f));

  return layout;
}
